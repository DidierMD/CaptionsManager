#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <cctype>

using namespace std;

//////////TimeMoment

class TimeMoment{
	private:
		int Hour;
		int Minute;
		int Second;
		int SecondFraction;

	public:
		inline TimeMoment(void){};
		inline TimeMoment(int h, int m, int s, int sf);
		inline TimeMoment& operator = (const TimeMoment& time);
		inline string sHour(void) const;
		inline string sMinute(void) const;
		inline string sSecond(void) const;
		inline string sSecondFraction(void) const;
		inline int hour(void) const {return Hour;};
		inline int minute(void) const {return Minute;};
		inline int second(void) const {return Second;};
		inline int secondFraction(void) const {return SecondFraction;};
		inline TimeMoment& operator += (const TimeMoment&);
		inline TimeMoment& operator -= (const TimeMoment&);
};

TimeMoment::TimeMoment(int h, int m, int s, int sf)
{
	Hour = h;
	Minute = m;
	Second = s;
	SecondFraction = sf;
}

TimeMoment& TimeMoment::operator = (const TimeMoment& time)
{
	Hour = time.hour();
	Minute = time.minute();
	Second = time.second();
	SecondFraction = time.secondFraction(); 
	return (*this);
}

string TimeMoment::sHour(void) const
{
	string h=to_string(Hour);
	if (h.size() == 1) h = "0" + h;
	return h;
}

string TimeMoment::sMinute(void) const
{
	string m=to_string(Minute);
	if (m.size() == 1) m = "0" + m;
	return m;
}

string TimeMoment::sSecond(void) const
{
	string s=to_string(Second);
	if (s.size() == 1) s = "0" + s;
	return s;
}

string TimeMoment::sSecondFraction(void) const
{
	string sf = to_string(SecondFraction);
	switch(sf.size()){
	case 1:
		sf = "00" + sf;
		break;
	case 2:
		sf = "0" + sf;
		break;
	}
	return sf;
}

TimeMoment& TimeMoment :: operator += (const TimeMoment& der)
{
	Hour = Hour + der.hour() + (
		Minute = Minute + der.minute() + (
			Second = Second + der.second() + (
				SecondFraction = SecondFraction + der.secondFraction()
														)/1000
													)/60
										)/60;
	Minute %= 60;
	Second %= 60;
	SecondFraction %= 1000;

	return *this;
}

TimeMoment& TimeMoment :: operator -= (const TimeMoment& der)
{
	int auxIzq = secondFraction() + second()*1000 + 
					 minute()*60000 + hour()*3600000;
	int auxDer = der.secondFraction() + der.second()*1000 + 
					 der.minute()*60000 + der.hour()*3600000;

	auxIzq -= auxDer;
	SecondFraction = auxIzq % 1000;
	auxIzq /= 1000;
	Second = auxIzq % 60;
	auxIzq /= 60;
	Minute = auxIzq % 60;
	auxIzq /= 60;
	Hour = auxIzq;

	return *this;
}

//////////TimeInterval

class TimeInterval{
	private:
		TimeMoment Start;
		TimeMoment End;
	public:
		inline TimeInterval(void){};
		inline TimeInterval(const TimeMoment& s, const TimeMoment& e);
		inline TimeMoment start(void) const {return Start;};
		inline TimeMoment end(void) const {return End;};
		inline TimeInterval& operator += (const TimeMoment&);
		inline TimeInterval& operator -= (const TimeMoment&);
};

TimeInterval::TimeInterval(const TimeMoment& s, const TimeMoment& e)
{
	Start = s;
	End = e;
}

TimeInterval& TimeInterval :: operator += (const TimeMoment& time)
{
	Start += time;
	End += time;
	return *this;
}

TimeInterval& TimeInterval :: operator -= (const TimeMoment& time)
{
	Start -= time;
	End -= time;
	return *this;
}

//////////Caption

class Caption{
	private:
		TimeInterval Lifetime;
		string Text;

	public:
		inline Caption(void){};
		inline Caption(const TimeInterval& time, const string& text);

		inline TimeInterval lifetime(void) const {return Lifetime;};
		inline string text(void) const {return Text;};

		inline void setLifetime(const TimeInterval& t){Lifetime = t;};
		inline void setText(const string& str){Text = str;};
		inline bool getFromStream(istream& in);
		inline bool getFromStream16(istream& in);
		inline Caption& operator += (const TimeMoment&);
		inline Caption& operator -= (const TimeMoment&);
};

Caption::Caption(const TimeInterval& time, const string& text)
{
	Lifetime = time;
	Text = text;
}

inline ostream& operator << (ostream& out, const Caption& sub)
{
	out << sub.lifetime().start().sHour() << ":" << sub.lifetime().start().sMinute() << ":" 
		 << sub.lifetime().start().sSecond() << "," << sub.lifetime().start().sSecondFraction() << " --> "
	    << sub.lifetime().end().sHour() << ":" << sub.lifetime().end().sMinute() << ":" 
		 << sub.lifetime().end().sSecond() << "," << sub.lifetime().end().sSecondFraction() << endl;
	out << sub.text();
	return out;
}

bool Caption::getFromStream(istream& in)
{
	unsigned int t[4];
	int c;
	string aux, tex; 
	for(int j=0; j<4; ++j){
		while(in.good() && !isdigit(c=in.get())) //Comer basura
			;
		if(isdigit(c)) { //if we found digit
			in.putback(c); 
			in >> t[j];
		}
	}
	while(t[3]>999) t[3] /= 10;
	TimeMoment ini(t[0], t[1], t[2], t[3]); 
	
	for(int j=0; j<4; ++j){
		while(in.good() && !isdigit(c=in.get())) //Comer basura
			;
		if(isdigit(c)) { //if we found digit
			in.putback(c); 
			in >> t[j];
		}
	}
	while(t[3]>999) t[3] /= 10;
	TimeMoment fin(t[0], t[1], t[2], t[3]); 

	bool goodread=in.good();
	while(in.good() && ((c=in.get()) != '\n')); //Comer hasta el final de linea

	getline(in,aux);
	while(aux.size() > 0){
		tex = tex + aux + "\n";
		getline(in, aux);
	}
	if(goodread){
		setLifetime(TimeInterval(ini, fin));
		setText(tex);
	}
	
	return goodread;
}

bool Caption::getFromStream16(istream& in)
{
	unsigned int t[4];
	char c;
	string aux, tex; 
	for(int j=0; j<4; ++j){
		while(in.good() && !isdigit(c=in.get())) //Comer basura
			;
		if(isdigit(c)) { //if we found digit
			in.putback(c); 
			in >> t[j];
		}
	}
	while(t[3]>999) t[3] /= 10;
	TimeMoment ini(t[0], t[1], t[2], t[3]); 
	
	for(int j=0; j<4; ++j){
		while(in.good() && !isdigit(c=in.get())) //Comer basura
			;
		if(isdigit(c)) { //if we found digit
			in.putback(c); 
			in >> t[j];
		}
	}
	while(t[3]>999) t[3] /= 10;
	TimeMoment fin(t[0], t[1], t[2], t[3]); 

	bool goodread=in.good();
	while(in.get(c) && c != '\n'); //Comer hasta el final de linea

	getline(in,aux);
	while(in && aux.size() > 1){ // Leer texto de subtítulo
		tex = tex + aux + "\n";
		getline(in, aux);
	}
	if(goodread){
		setLifetime(TimeInterval(ini, fin));
		setText(tex);
	}
	return goodread;
}

Caption& Caption :: operator += (const TimeMoment& time)
{
	setLifetime(lifetime() += time);
	return *this;
}

Caption& Caption :: operator -= (const TimeMoment& time)
{
	setLifetime(lifetime() -= time);
	return *this;
}

//////////CaptionsManager

class CaptionsManager{
	private:
		list <Caption> Captions;
	public:
		inline CaptionsManager(void){};
		inline void insertCaptionAtEnd(const TimeInterval& time, const string& text);
		inline int insertCaption(int num, const TimeInterval& time, const string& text);
		inline int eraseLastCaption(void);
		inline int saveToFile(string filename) const;
		inline int readFromFile(string filename);
		inline int readFromFile16(string filename);
		inline int eraseCaption(int n);
		inline int eraseCaption(int ini, int fin);
		inline void moveCaptionsPositive(const TimeMoment& time);
		inline void moveCaptionsNegative(const TimeMoment& time);
		inline void toStream(ostream& out, int n); // Muestra los primeros n subtitulos
		inline void toStream(ostream& out);
};

void CaptionsManager::insertCaptionAtEnd(const TimeInterval& time, const string& text)
{
	Captions.emplace_back(time, text);
}

int CaptionsManager::insertCaption(int num, const TimeInterval& time, const string& text)
{
	if(num < 1 || num > Captions.size()+1)
		return 1;

	list <Caption> :: const_iterator iter = Captions.cbegin();
	advance(iter,num-1);
	Captions.emplace(iter, time, text);
	return 0;
}

int CaptionsManager::eraseLastCaption(void) 
{
	if(Captions.empty())
		return 1;
	Captions.pop_back();
	return 0;
}

int CaptionsManager::eraseCaption(int n)
{
	if(n < 1 || n > Captions.size())
		return 1;

	list <Caption> :: const_iterator iter = Captions.cbegin();
	advance(iter, n-1);
	Captions.erase(iter);
	return 0;
}

int CaptionsManager::eraseCaption(int ini, int fin)
{
	if(ini < 1 || fin > Captions.size() || ini > fin)
		return 1;

	list <Caption> :: const_iterator iterini = Captions.cbegin();
	list <Caption> :: const_iterator iterfin;
	advance(iterini, ini-1);
	iterfin = iterini;
	advance(iterfin, fin-ini+1);
	Captions.erase(iterini, iterfin);
	return 0;
}

int CaptionsManager::saveToFile(string filename) const
{
	ofstream arch(filename);
	if(!arch){
		return 1;
	}
   list <Caption>::const_iterator iter;
   list <Caption>::const_iterator iter_final = Captions.end();

	int i;
   for(iter = Captions.begin(), i=1; iter != iter_final; ++iter, ++i){
		arch << i << endl;
		arch << (*iter) << endl;
	}
	return 0;
}

int CaptionsManager::readFromFile(string filename)
{
	ifstream arch(filename);
	string aux;

	if(!arch){
		return 0;
	}

	Captions.clear();
	while(arch.good()){
		getline(arch,aux);
		Caption capt;
		if(capt.getFromStream(arch))
			Captions.push_back(capt);
	}
	return Captions.size();
}

int CaptionsManager::readFromFile16(string filename)
{
	ifstream arch(filename);
	string aux;

	if(!arch.good()){
		return 0;
	}
	Captions.clear();
	while(arch.good()){
		getline(arch,aux);
		Caption capt;
		if(capt.getFromStream16(arch))
			Captions.push_back(capt);
	}
	return Captions.size();
}

void CaptionsManager::moveCaptionsPositive(const TimeMoment& time)
{
	if(Captions.empty())
		return;
	list <Caption> :: iterator iter;
	list <Caption> :: iterator iter_fin = Captions.end();
	
	for(iter = Captions.begin(); iter != iter_fin; ++iter){
		(*iter) += time;
	}
}

void CaptionsManager::moveCaptionsNegative(const TimeMoment& time)
{
	if(Captions.empty())
		return;
	list <Caption> :: iterator iter;
	list <Caption> :: iterator iter_fin = Captions.end();
	
	for(iter = Captions.begin(); iter != iter_fin; ++iter){
		(*iter) -= time;
	}
}

void CaptionsManager::toStream(ostream& out, int n)
{
	if(Captions.empty() || n < 1)
		return;
   list <Caption>::const_iterator iter;
   list <Caption>::const_iterator iter_final = Captions.cbegin();
	if(n>Captions.size()) n = Captions.size();
	advance(iter_final, n);
	int i=1;
   for(iter = Captions.cbegin(); iter != iter_final; ++iter, ++i){
		out << i << endl;
		out << (*iter) << endl;
	}
	return;
}

void CaptionsManager::toStream(ostream& out)
{
	if(Captions.empty())
		return ;

   list <Caption>::const_iterator iter;
   list <Caption>::const_iterator iter_final = Captions.end();

	int i;
   for(iter = Captions.begin(), i=1; iter != iter_final; ++iter, ++i){
		out << i << endl;
		out << (*iter) << endl;
	}
	return ;
}

//////////Main

int main(void)
{
	CaptionsManager manejador;
	bool ejecutar = true;
	string comando;
	cout << "Programa para gestionar un archivo de subtitulos\n";
	cout << "Comandos:\n\tadd -> agregar subtitulo al final" << endl
		  << "\tins n -> insertar subtitulo en la posicion n" << endl
		  << "\tpop -> borrar ultimo" << endl
		  << "\tdel n -> borrar subtitulo n" << endl
		  << "\tdel n m -> borrar subtitulos n a m incluyendo ambos" << endl
		  << "\tshow -> mostrar todos los subtitulos" << endl
		  << "\tshow n -> mostrar los primeros n subtitulos" << endl
		  << "\tmove -> recorre todos los subtítulos una cantidad de tiempo" << endl
		  << "\tread filename -> cargar el archivo de subtitulos filename (descarta el contenido actual)" << endl
		  << "\tread16 filename -> similar al anterior pero para archivos con caracter \"nueva linea\" de 2 bytes" << endl
		  << "\tsave -> guardar en archivo"<< endl
		  << "\tcmds -> mostrar estos comandos" << endl
		  << "\tend -> terminar programa" << endl;
	
	while(ejecutar){
		cout << ">";
		cin >> comando;
		if(comando == "add"){
			int h, m, s, sf;
			string tex, aux;

			cout << "Inicio: h m s ms" << endl;
			while(!isdigit(cin.peek())) cin.get();
			cin >> h;
			while(!isdigit(cin.peek())) cin.get();
			cin >> m;
			while(!isdigit(cin.peek())) cin.get();
			cin >> s;
			while(!isdigit(cin.peek())) cin.get();
			cin >> sf;

			while(sf>999) sf /= 10;
			TimeMoment ini(h, m, s, sf);

			cout << "Final: h m s ms" << endl;
			while(!isdigit(cin.peek())) cin.get();
			cin >> h;
			while(!isdigit(cin.peek())) cin.get();
			cin >> m;
			while(!isdigit(cin.peek())) cin.get();
			cin >> s;
			while(!isdigit(cin.peek())) cin.get();
			cin >> sf;

			while(cin.get() != '\n');

			while(sf>999) sf /= 10;
			TimeMoment fin(h, m, s, sf);

			cout << "Texto:" << endl;
			getline(cin,aux);
			while(aux.size() > 0){
				tex = tex + aux + "\n";
				getline(cin, aux);
			}

			manejador.insertCaptionAtEnd(TimeInterval(ini, fin), tex);
			cout << "Subtitulo agregado" << endl;
		}
		else if(comando == "ins"){
			int h, m, s, sf, num;
			string tex, aux;
			cin >> num;

			cout << "Inicio: h m s ms" << endl;
			while(!isdigit(cin.peek())) cin.get();
			cin >> h;
			while(!isdigit(cin.peek())) cin.get();
			cin >> m;
			while(!isdigit(cin.peek())) cin.get();
			cin >> s;
			while(!isdigit(cin.peek())) cin.get();
			cin >> sf;

			while(sf>999) sf /= 10;
			TimeMoment ini(h, m, s, sf);

			cout << "Final: h m s ms" << endl;
			while(!isdigit(cin.peek())) cin.get();
			cin >> h;
			while(!isdigit(cin.peek())) cin.get();
			cin >> m;
			while(!isdigit(cin.peek())) cin.get();
			cin >> s;
			while(!isdigit(cin.peek())) cin.get();
			cin >> sf;

			while(cin.get() != '\n');

			while(sf>999) sf /= 10;
			TimeMoment fin(h, m, s, sf);

			cout << "Texto:" << endl;
			getline(cin,aux);
			while(aux.size() > 0){
				tex = tex + aux + "\n";
				getline(cin, aux);
			}

			if(!manejador.insertCaption(num, TimeInterval(ini, fin), tex))
				cout << "Subtitulo agregado" << endl;
			else 
				cout << "No se pudo agregar subtitulo" << endl;
		}
		else if(comando == "pop"){
			if(!manejador.eraseLastCaption())
				cout << "Subtitulo borrado" << endl;
			else
				cout << "No hay subtitulo que borrar" << endl;
		}
		else if(comando == "del"){
			int num;
			int num2;
			num = cin.get();
			while(!isdigit(num)) num = cin.get();
			cin.putback(num);
			cin >> num;
			num2 = cin.get();
			while(!isdigit(num2) && num2!='\n') num2 = cin.get();
			if(num2 == '\n'){
				if(!manejador.eraseCaption(num))
					cout << "Subtitulo borrado" << endl;
				else
					cout << "No se pudo borrar subtitulo" << endl;
			}
			else{
				cin.putback(num2);
				cin >> num2;
				if(!manejador.eraseCaption(num, num2))
					cout << "Subtitulos borrados" << endl;
				else
					cout << "No se pudieron borrar los subtitulos" << endl;
			}
		}
		else if(comando == "show"){
			int c;

			c = cin.get();
			while((c != '\n') && (!isdigit(c))) c = cin.get();
			if (isdigit(c)){
				cin.putback(static_cast<char>(c));
				cin >> c;
				manejador.toStream(cout, c);
			}
			else
				manejador.toStream(cout);
		}
		else if(comando == "move"){
			int h, m, s, sf;
			bool positive_move = true;

			cout << "Tiempo: h m s ms (poner un - antes de h si el corrimiento es negativo)" << endl;
			while(!isdigit(cin.peek()) && cin.peek()!='-') cin.get();
			if(cin.peek() == '-'){
				positive_move = false;
			}
			while(!isdigit(cin.peek())) cin.get();
			cin >> h;
			while(!isdigit(cin.peek())) cin.get();
			cin >> m;
			while(!isdigit(cin.peek())) cin.get();
			cin >> s;
			while(!isdigit(cin.peek())) cin.get();
			cin >> sf;
			
			while(sf>999) sf /= 10;
			TimeMoment time(h, m, s, sf);

			if(positive_move)
				manejador.moveCaptionsPositive(time);
			else
				manejador.moveCaptionsNegative(time);
				
			cout << "Se recorrieron los subtítulos" << endl;
		}
		else if(comando == "read"){
			int count;
			string nombrefichero;
			cin >> nombrefichero;
			if( (count = manejador.readFromFile(nombrefichero)) )
				cout << count << " subtitulos leidos desde el archivo " << nombrefichero << endl;
			else
				cout << "No se pudo leer archivo de subtitulos"<< endl;
		}
		else if(comando == "read16"){
			int count;
			string nombrefichero;
			cin >> nombrefichero;
			if( (count = manejador.readFromFile16(nombrefichero)) ){
				cout << count << " subtitulos leidos desde el archivo " << nombrefichero << endl;
			}
			else{
				cout << "No se pudo leer archivo de subtitulos"<< endl;
			}
		}
		else if(comando == "save"){
			string nombrefichero;
			cout << "Nombre del archivo: ";
			cin >> nombrefichero;
			if(!manejador.saveToFile(nombrefichero))
				cout << "Archivo de subtitulos creado" << endl;
			else
				cout << "No se pudo crear archivo de subtitulos"<< endl;
		}
		else if(comando == "cmds"){
			cout << "Comandos:\n\tadd -> agregar subtitulo al final" << endl
		  		<< "\tins n -> insertar subtitulo en la posicion n" << endl
	  			<< "\tpop -> borrar ultimo" << endl
		  		<< "\tdel n -> borrar subtitulo n" << endl
		  		<< "\tdel n m -> borrar subtitulos n a m incluyendo ambos" << endl
		  		<< "\tshow -> mostrar todos los subtitulos" << endl
		  		<< "\tshow n -> mostrar los primeros n subtitulos" << endl
		  		<< "\tmove -> recorre todos los subtítulos una cantidad de tiempo" << endl
		  		<< "\tread filename -> cargar el archivo de subtitulos filename(descartando el contenido actual)" << endl
		  		<< "\tread16 filename -> similar al anterior pero para archivos con caracter \"nueva linea\" de 2 bytes" << endl
		  		<< "\tsave -> guardar en archivo"<< endl
		  		<< "\tcmds -> mostrar estos comandos" << endl
		  		<< "\tend -> terminar programa" << endl;
		}
		else if(comando == "end"){
			ejecutar = false;
		}
	}
	
return 0;
}

