#include<vector>
#include<iostream>
#include<string>
#include<set>
#include<map>
#include<fstream>
#include<algorithm>
using namespace std;
set<string> TD;
set<string> TW;
set<string> TID;
map<string, int> intVars;
map<string, string> stringVars;
map<string, long double> doubleVars;
fstream f;
class finish {};
class error {};
vector <string> oper, stackOper;
int polizInd;
void analis(int i = 0);
int PC = 0, wasTrue = 0;

void initTW(vector<string> v) {
    for(auto i:v)
        TW.insert(i);
}

void ERROR() {
    error er;
    throw er;
}


int toInt(string s) {
    int res=0;
    for(auto i:s) {
        res=res*10+(i)-'0';
    }
    return res;
}

long double toDouble(string s) {
    long double res = 0.0;
    int point = 0;
    long double del=1.0;
    for(auto c : s) {
        if((point == 0) && (c != '.'))
            res = res * 10 + c - '0';
        else if(point == 1) {
            del = del / 10;
            res = res + del * (c-'0');
        }
        if(c == '.')
            point = 1;
    }
    return res;
}


class leksem{
	string lex;
	int type;//1 keyword 2 + - * ..; 3int 4 double 5 string 6 id
public:
	int getType(){
		return type;
	}
	string getName(){
		return lex;
	}
	void setName(string name){
		lex = name;
	}

	leksem(int type,string lex){
		this->type = type;
		this->lex = lex;
	}
};

class ValInPoliz{
	int *intVal;
	long double *doubleVal;
	string *stringVal;
	int type;
	int realType;
public:
	ValInPoliz(){
		intVal = NULL;
		doubleVal = NULL;
		stringVal = NULL;
		type = -1;
	}
	ValInPoliz(leksem l){
		intVal = NULL;
		doubleVal = NULL;
		stringVal = NULL;
		type = l.getType();
		switch (type)
		{
		case 8:
		case 3:
			realType=type;
			intVal = new int();
			*intVal=toInt(l.getName());
		//	doubleVal = new long double(toInt(l.getName()));
			
			break;
		case 4:
			realType=type;
			doubleVal = new long double(toDouble(l.getName()));
			break;
		case 5:
			realType=type;
			stringVal = new string(l.getName());
			break;
		case 6:
			if(intVars.count(l.getName())>0){
				realType=3;
				intVal=&intVars[l.getName()];
				//doubleVal = (long double *)&intVars[l.getName()];
			}
			else
				if(doubleVars.count(l.getName())>0){
					realType=4;
					doubleVal = &doubleVars[l.getName()];
				}
				else
				{
					if(stringVars.count(l.getName())>0){
						realType=5;
						stringVal = &stringVars[l.getName()];
					}
					else 
					ERROR();
				}
			break;
		default:
			ERROR();
			break;
		} 
	}
	int getType(){
		return type;
	}
	int getRealType(){
		return realType;
	}
	ValInPoliz(const ValInPoliz &v){
		realType = v.realType;
		if(v.type!=6){
			if(v.intVal!=NULL){
				intVal = new int(*(v.intVal));
			}
			if(v.doubleVal!=NULL){
				doubleVal = new long double(*(v.doubleVal));
			}
			if(v.stringVal!=NULL){
				stringVal = new string(*(v.stringVal));
			}
			type = v.type;
		}
		else{
			intVal = v.intVal;
			doubleVal = v.doubleVal;
			stringVal = v.stringVal;
			type = v.type;
		}
	}

	int &getIntVal(){
		return *intVal;
	}
	long double &getDoubleVal(){
		return *doubleVal;
	}
	string &getStringVal(){
		return *stringVal;
	}
	~ValInPoliz(){
		switch (type)
		{
		case 8:
		case 3:
			delete intVal;
			break;
		case 4:
			delete doubleVal;
			break;
		case 5:
			delete stringVal;
			break;
		default:
			break;
		}
	}
};

vector<ValInPoliz> stackPolizNum;

void init() {
    int n = 16;
    const char* ca[] = {"int","for","case", "of","string", "real", "if", "read", "write", "not", "and", "or", "step", "until", "do","break"};
    vector<string> v(n);
    for(int i = 0; i < n; i++)
        v[i] = ca[i];
    initTW(v);
	polizInd = 0;
	stackPolizNum.reserve(10000);
    f.open("prog.txt",ios::in);
	oper.push_back("=");
	oper.push_back("or");
    oper.push_back("and");
	oper.push_back("not");
    oper.push_back("<");
    oper.push_back(">");
    oper.push_back(">=");
    oper.push_back("<=");
    oper.push_back("==");
    oper.push_back("!=");
    oper.push_back("+");
    oper.push_back("-");
    oper.push_back("*");
    oper.push_back("/");
    oper.push_back("%");
	oper.push_back("+u");
    oper.push_back("-u");
    //oper.push_back("(");
    //oper.push_back(")");
}

bool isAriOperator(string s) {
    return s=="+" || s=="-" || s=="*" || s=="/" || s=="%";
}

class lexAn {
    vector<leksem> leks;
public:
    static int next;

    leksem getLex() {
        if(next<leks.size())
            return leks[next++];
        finish a;
        throw a;
        return leksem(0,"");
    }

    leksem& getLex(int num) {
		if(isLexNumIn(num))
			return leks[num];
		else ERROR();
    }

    int size() {
        return leks.size();
    }

	void changeLex(int num, string name){
		leks[num].setName(name);
	}

    bool isLexNumIn(int num) {
        return num<leks.size();
    }
    void add(int type, string lex) {
        leks.push_back(leksem(type,lex));
    }
	void add(leksem lex) {
        leks.push_back(lex);
    }

    void print() {
		int i=0;
        for(auto it:leks) {
            cout<<i++<<" "<<it.getName()<<" "<<it.getType()<<'\n';
        }
    }
} leks, poliz;

char* itoa(int a,char *s, int n){
    int i=0;
    int d=1;
	if(a==0){
		s[0] = '0';
		s[1] = '\0';
		return s;
	}
    while(a/d>0)
        d=d*n;
    d=d/n;
    while(d!=0){
        s[i]='0'+(a/d)%n;
        d=d/n;
        i++;
    }
	s[i]='\0';
    return s;
}
int lexAn::next=0;
enum state{
	H, ID, NUM, DLM, ER, FIN
};

state getstr(){
	char c = f.get();
	string buf="";
	while((c != '"') && (c != EOF)){
		buf += c;
		c = f.get();
	}
	if(c == EOF){
		return ER;
	}
	leks.add(5,buf);
	return H;
}

void id_or_word(string buf){
	if(TW.count(buf) > 0){
		leks.add(1,buf);
		return;
	}
	else
		if (TID.count(buf) > 0)
			leks.add(6,buf);
		else
		{
			leks.add(6,buf);
			TID.insert(buf);
		}
}

void scan(){
	state TC = H;
	string buf;
	int d;
	//cout<<"nfafnafaf"<<endl;
	int isreal  = 0;
	string d2;
	char c;
	c = f.get();
	while(((TC != FIN) && (TC != ER)) && (c != EOF)){
		switch (TC)
		{
		case H:
			if((c == ' ')||(c == '\n')||(c == '\t'))
				c = f.get();
			else
				if (isalpha(c))
				{
					buf.clear();
					buf.push_back(c);
					c = f.get();
					TC = ID;
				}
				else
					if (isdigit(c))
					{
						d = c - '0';
						c = f.get();
						TC = NUM;
					}
					else
						if(c=='}'){
							//f>>c;
							leks.add(2,"}");
							c = f.get();
						}
						else

						if(c=='{'){
							//f>>c;
							leks.add(2,"{");
							c = f.get();
						}
						else
						if(c=='('){
							//f>>c;
							leks.add(2,"(");
							c = f.get();
						}
						else

						if(c==')'){
							//f>>c;
							leks.add(2,")");
							c = f.get();
						}
						else
							if((c == '!') || (c == '<') || (c == '>')){
								string s;
								s=c;
								c=f.get();
								if((c=='=')){
									leks.add(2,s+"=");
									c=f.get();
								}
								else
								{
									leks.add(2,s);
								}
							}
							else
							if (c == '='){
								c = f.get();
								if((c == '>') || (c == '<') || (c == '=')){
									leks.add(2, string("=") + c);
									c = f.get();
								}
								else
								{
									leks.add(2,"=");
								}
							}
							else
								if((c == ';') || (c == ':') || (c == '+') || (c == '-') || (c=='*') || (c=='/')){
									string s;
									s = c;
									leks.add(2,s);
									c = f.get();
								}
								else
								{
									if(c == '.'){
										TC = FIN;
									}
									else{
										if(c == '"'){
											TC = getstr();
											c = f.get();
										}
										else
										{
											ERROR();
										}
									}
								}
			break;
		case ID:
			if(isalpha(c) || isdigit(c)){
				buf.push_back(c);
				c=f.get();
			}
			else
			{
				id_or_word(buf);
				buf.clear();
				TC = H;
			}
			break;
		case NUM:
			if(isdigit(c)){
				
				if(isreal == 0)
					d = d * 10 + (c - '0');
				else
					d2=d2+c;
				c = f.get();
			}
			else
			if(c=='.'){
					isreal = 1;
					c = f.get();
					d2="";

			}
			else
			{
				if(isalpha(c)){
					ERROR();
				}
				char *str = new char[80];
				str = itoa(d,str,10);
				//cout<<str;
				if((isreal == 0)||(d2 == "0"))
					leks.add(3,str);
				else{
					//char *str2 = new char[80];
					//str2 = itoa(d2,str2,10);
					leks.add(4,string(str) + "." + d2);
					//delete []str2;
				}
				delete []str;
				TC = H;
				isreal = 0;
			}
			break;
		}

	}
}
// ne testyl
int priority(string name){
	if(name == "-")
		return find(oper.begin(), oper.end(), "-")- oper.begin();
	if((name == "%")||(name  == "/"))
		return find(oper.begin(), oper.end(), "*")- oper.begin();
	return find(oper.begin(), oper.end(), name)- oper.begin();
}

void addId(leksem l, int type, int &wassign, int &wasConstOrVar){
	wassign = 0;
	string name = l.getName();
	if(wasConstOrVar)
        ERROR();
    wasConstOrVar = 1;
	switch (type)
	{
	case 3:
		if(intVars.count(l.getName()))
			poliz.add(l);
		else
		{
			ERROR();
		}
		break;
	case 4:
		if(doubleVars.count(l.getName()))
			poliz.add(l);
		else
		{
			if(intVars.count(l.getName()))
				poliz.add(l);
			else
				ERROR();
		}
		break;
	case 5:
		if(stringVars.count(l.getName()))
			poliz.add(l);
		else
		{
			ERROR();
		}
		break;
	default:
		ERROR();
	}
}

void addConst(leksem l, int type, int &wassign, int &wasConstOrVar){
	wassign = 0;
    string name = l.getName();
	if(wasConstOrVar)
        ERROR();
    wasConstOrVar = 1;
	if(l.getType() == type)
		poliz.add(l);
	else
	{
		if ((l.getType()==3)&&(type==4)){
			leksem l1(4, l.getName()+".0");
			poliz.add(l1);
		}
		else
			ERROR();
	}
}

void throwStack(int prior){
	bool b = true;
	//for( string i : stackOper)
	//	cout << i;
	//cout << endl;
	while((stackOper.size() > 0)&&(b))
		if(!(prior > priority(stackOper[stackOper.size() - 1]))){
			string s = stackOper[stackOper.size() - 1];
			stackOper.pop_back();
			poliz.add(2,s);
		}
		else
		{
			b = false;
		}
}

void addLogOper(leksem l, int type, int &wassign, int &wasConstOrVar){
    string name = l.getName();
    if((name!="not")&&(name!="and")&&(name!="or"))
        ERROR();
    if(type!=3)
        ERROR();
    throwStack(priority(l.getName()));
	stackOper.push_back(l.getName());
	wassign = 1;
	wasConstOrVar = 0;
	//cout<<name<<endl;
}

void addOper(leksem l, int type, int &wassign, int &wasConstOrVar){
	if(wassign){
		if((type == 3)||(type==4)){
			if((l.getName()=="+")||(l.getName()=="-")){
				l.setName(l.getName() +'u');
			}
			else
			{
				ERROR();
			}
		}
		else
		{
			ERROR();
		}
	}
	if(type == 5){
		if((l.getName() != "+")&&(l.getName() !="="))
			ERROR();
	}
	throwStack(priority(l.getName()));
	stackOper.push_back(l.getName());
	wassign = 1;
	wasConstOrVar = 0;
}

void getPolizExpr(int &numlex, string fin, int type){
	int rightType = type;
	int it = 0;
	int wassign = 0;
	int wasConstOrVar = 0;
	if(!leks.isLexNumIn(numlex))
		ERROR();
	while(leks.getLex(numlex).getName() != fin){
			switch (leks.getLex(numlex).getType())
			{
			case 6:
				addId(leks.getLex(numlex), rightType, wassign, wasConstOrVar);
				break;
			case 4:
			case 5:
			case 3: addConst(leks.getLex(numlex), rightType, wassign,wasConstOrVar);
				break;
			case 2:
				addOper(leks.getLex(numlex), rightType, wassign,wasConstOrVar);
				break;
            case 1:
                addLogOper(leks.getLex(numlex), rightType, wassign,wasConstOrVar);
                break;
			default:
				ERROR();

		}

		it++;
		numlex++;
		if(!leks.isLexNumIn(numlex))
			ERROR();
	}
	throwStack(0);
}

void isId(int &numlex){
	string name = leks.getLex(numlex).getName();
	if(intVars.count(name) + doubleVars.count(name) + stringVars.count(name) == 0)
		ERROR();
	if(leks.getLex(numlex+1).getName() == ";"){
		poliz.add(leks.getLex(numlex++));
		poliz.add(leks.getLex(numlex++));
		return;
	}
	if(intVars.count(name) > 0)
		getPolizExpr(numlex, ";",3);
	if(doubleVars.count(name) > 0)
		getPolizExpr(numlex, ";",4);
	if(stringVars.count(name) > 0)
		getPolizExpr(numlex, ";",5);
	return;
}

void isSign(int &numlex){
	if(leks.getLex(numlex).getName() == ";"){
		poliz.add(leks.getLex(numlex));
		numlex++;
		return;
	}
	if(leks.getLex(numlex).getName() == "{")
		try{
			analis();
			ERROR();
		}
		catch(int in){
			numlex = in + 1;
			return;
		}
	if(leks.getLex(numlex).getName() == "}")
		throw numlex;
	ERROR();
}

void isTypeKeyWord(int &numlex){
	if(intVars.count(leks.getLex(numlex).getName())+doubleVars.count(leks.getLex(numlex).getName())
			+stringVars.count(leks.getLex(numlex).getName()) == 0){
			if(leks.getLex(numlex).getName() == "int"){
				intVars[leks.getLex(++numlex).getName()]=0;
				getPolizExpr(numlex,";",3);
			}
			else
				if(leks.getLex(numlex).getName() == "real"){
					doubleVars[leks.getLex(++numlex).getName()] = 0.0;
					getPolizExpr(numlex,";",4);
				}
				else
					if(leks.getLex(numlex).getName() == "string"){
						stringVars[leks.getLex(++numlex).getName()] = "";
						getPolizExpr(numlex,";",5);
					}
	}
}

void makeBlockPoliz(int &numlex, int smech = 0){
	int perehod = poliz.size();
	poliz.add(leksem(8,""));
	poliz.add(leksem(7,"!f"));
	string name = leks.getLex(numlex).getName();
	if(name=="{"){
		numlex++;
		try{
			analis(numlex);
			ERROR();
		}
		catch(int in){
			numlex = in + 1;
		}
		char *str = new char[80];
		str = itoa(poliz.size() + smech,str,10);
		poliz.getLex(perehod).setName(str);
		delete []str;
	}
}

void makeIfPoliz(int &numlex){
	numlex++;
	string name = leks.getLex(numlex).getName();
	if(name == "("){
		numlex++;
		getPolizExpr(numlex,")",3);
		numlex++;
		makeBlockPoliz(numlex);
				//îäèí îïåðàòîð
	}
	else
		ERROR();
}

void makeCasePoliz(int &numlex){
	char *str = new char[80];
	numlex++;
	string name = leks.getLex(numlex).getName();
	if(intVars.count(name) != 1)
		ERROR();
	string nameVar = name;
	numlex++;
	name = leks.getLex(numlex).getName();
	if(name != "of")
		ERROR();
	numlex++;
	name = leks.getLex(numlex).getName();
	while (name != "end"){
		if(leks.getLex(numlex).getType()!=3)
			ERROR();
		poliz.add(6,nameVar);
		poliz.add(3,name);
		poliz.add(2,"==");
		int perehod = poliz.size();
		poliz.add(leksem(8,""));
		poliz.add(leksem(7,"!f"));
		numlex++;
		if(leks.getLex(numlex).getName()!=":")
				ERROR();
		numlex++;
		try{
				analis(numlex);
				ERROR();
		}
		catch(int in){
				numlex = in;
				str = itoa(poliz.size(),str,10);
				poliz.getLex(perehod).setName(str);
		}
		numlex++;
		name = leks.getLex(numlex).getName();
	}
	delete []str;
	numlex++;
}

void makeForPoliz(int &numlex){
	numlex++;
	getPolizExpr(numlex,"step",3);
	int per1t = poliz.size();
    poliz.add(8,"");
	poliz.add(7,"!");
	numlex++;
	int firstLabel = poliz.size();//ïðûãàåì ñ êîíöà
	getPolizExpr(numlex,"until",3);
	int s = poliz.size();
	char *str = new char[80];
	str = itoa(s,str,10);
	poliz.getLex(per1t).setName(str);
	numlex++;
	getPolizExpr(numlex,"do",3);
	numlex++;
	makeBlockPoliz(numlex, 2);
	str = itoa(firstLabel,str,10);
	poliz.add(8,str);
	poliz.add(7,"!");
	delete []str;
}
int defineTypeExpr(int numlex){
	int type;
	if(!leks.isLexNumIn(numlex))
		ERROR();
	if(leks.getLex(numlex).getType() == 6){
						string name = leks.getLex(numlex).getName();
						if(intVars.count(name)>0)
							type = 3;
						else
							if(doubleVars.count(name)>0)
								type = 4;
								else
									if (stringVars.count(name)>0)
										type = 5;
									else
										ERROR;
					}
					else{
						type = leks.getLex(numlex).getType();
						if(type==2){
							if((leks.getLex(numlex).getName()=="-u")||(leks.getLex(numlex).getName()=="+u")){
								return defineTypeExpr(numlex+1);
							}
							ERROR();
						}
						if(type == 1)
							ERROR();
					}
	return type;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!äîäåëàòü
void isKeyWord(int &numlex){
	string name = leks.getLex(numlex).getName();
	if((name == "int")||(name == "string")||(name == "real"))
		isTypeKeyWord(numlex);
	else
	{
		if(name == "if"){
			makeIfPoliz(numlex);
				//âîçìîæíî  îäèí îïåðàòîð íàäî ïðîïèñàòü
			}
		else
			if(name == "for"){
				//cout<<1313131<<endl;
				makeForPoliz(numlex);
			}
			else
				if(name == "write"){
					numlex++;
					name = leks.getLex(numlex).getName();
					if(name!="(")
						ERROR();
					numlex++;
					int type = defineTypeExpr(numlex);
					getPolizExpr(numlex,")",type);
					poliz.add(1,"write");
					numlex++;
				}
				else
				{
					if(name == "read"){
						numlex++;
						name = leks.getLex(numlex).getName();
						if(name!="(")
							ERROR();
						numlex++;
						name = leks.getLex(numlex).getName();
						if(intVars.count(leks.getLex(numlex).getName())+doubleVars.count(leks.getLex(numlex).getName())
			+stringVars.count(leks.getLex(numlex).getName()) == 0)
							ERROR();
						poliz.add(leks.getLex(numlex));
						numlex+=2;
						poliz.add(1,"read");
				}
					else
					{
						if(name == "case"){
							makeCasePoliz(numlex);
						}
						else
						{
							if(name == "break")
								if(leks.getLex(++numlex).getName()==";")
									throw numlex;
							ERROR();
						}
					}

			}
	}
}

void analis(int i){
	while(leks.isLexNumIn(i)){
		leks.getLex(i).getName();
		switch (leks.getLex(i).getType())
		{
		case 6:
			isId(i);
			break;
		case 2:
			isSign(i);
			break;
		case 1:
			isKeyWord(i);
			break;
		default:
			ERROR();
		}
	}
}
void addIntStackPoliz(int a){
	stackPolizNum.push_back(ValInPoliz(leksem(3,"1")));
	stackPolizNum[stackPolizNum.size() - 1].getIntVal() = a;
}
void makeRW(string op){
	//cout<<op;
	if(op=="read"){
		switch (stackPolizNum[stackPolizNum.size() - 1].getRealType())
		{
		case 3:
			cin>>stackPolizNum[stackPolizNum.size() - 1].getIntVal();
			break;
		case 4:
			cin>>stackPolizNum[stackPolizNum.size() - 1].getDoubleVal();
			break;
		case 5:
			cin>>stackPolizNum[stackPolizNum.size() - 1].getStringVal();
			break;
		default:
			break;
		}
		stackPolizNum.pop_back();
	}
	else
		if(op=="write"){
			//cout<<"12121212";
			switch (stackPolizNum[stackPolizNum.size() - 1].getRealType())
			{
			case 3:
				cout<<stackPolizNum[stackPolizNum.size() - 1].getIntVal();
				break;
			case 4:
				cout<<stackPolizNum[stackPolizNum.size() - 1].getDoubleVal();
				break;
			case 5:
				cout<<stackPolizNum[stackPolizNum.size() - 1].getStringVal();
				break;
			default:
				break;
			}
			stackPolizNum.pop_back();
		}
		else
		{
			ERROR();
		}
}

void makeJumpTo(string op){
	int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
	stackPolizNum.pop_back();
	if(op=="!"){
		lexAn::next = a;
	}
	else
		if(!wasTrue){
			lexAn::next = a;
		}
}

void addDoubleStackPoliz(double a){
	stackPolizNum.push_back(ValInPoliz(leksem(4,"1.1")));
	stackPolizNum[stackPolizNum.size() - 1].getDoubleVal() = a;
}
void addStringStackPoliz(string a){
	stackPolizNum.push_back(ValInPoliz(leksem(5,"1")));
	stackPolizNum[stackPolizNum.size() - 1].getStringVal() = a;
}
void intAsgm(){
	int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
	stackPolizNum.pop_back();
	if(stackPolizNum[stackPolizNum.size() - 1].getType()!=6)
		ERROR();
	stackPolizNum[stackPolizNum.size() - 1].getIntVal() = a;

	wasTrue = 1;
}
void doubleAsgm(){
	double a;
	if(stackPolizNum[stackPolizNum.size() - 1].getRealType()==3)
		 a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
	else
	{
		a = stackPolizNum[stackPolizNum.size() - 1].getDoubleVal();
	}
	stackPolizNum.pop_back();
	if(stackPolizNum[stackPolizNum.size() - 1].getType()!=6)
		ERROR();
	stackPolizNum[stackPolizNum.size() - 1].getDoubleVal() = a;
	wasTrue = 1;
}
void stringAsgm(){
	string a = stackPolizNum[stackPolizNum.size() - 1].getStringVal();
	stackPolizNum.pop_back();
	if(stackPolizNum[stackPolizNum.size() - 1].getType()!=6)
		ERROR();
	stackPolizNum[stackPolizNum.size() - 1].getStringVal() = a;
	wasTrue = 1;
}
void intAri(string op){
	int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
	stackPolizNum.pop_back();
	int b = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
	stackPolizNum.pop_back();
	int res=0;
	if(op=="+"){
		res = a + b;
	}
	if(op=="-"){
		res = - a + b;
	}
	if(op=="*"){
		res =  a * b;
	}
	if(op=="/"){
		res =  b / a;
	}
	if(op=="%"){
		res = b % a;
	}
	addIntStackPoliz(res);
	wasTrue = (res!=0);
}

void doubleAri(string op){
	long double a,b;
	if(stackPolizNum[stackPolizNum.size() - 1].getRealType()==4)
		 a = stackPolizNum[stackPolizNum.size() - 1].getDoubleVal();
	if(stackPolizNum[stackPolizNum.size() - 1].getRealType()==3)
		a = 0.0 + stackPolizNum[stackPolizNum.size() - 1].getIntVal();
	stackPolizNum.pop_back();
	if(stackPolizNum[stackPolizNum.size() - 1].getRealType()==4)
		 b = stackPolizNum[stackPolizNum.size() - 1].getDoubleVal();
	if(stackPolizNum[stackPolizNum.size() - 1].getRealType()==3)
		 b = 0.0 + stackPolizNum[stackPolizNum.size() - 1].getIntVal();
	stackPolizNum.pop_back();
	long double res=0;
	if(op=="+"){
		res = a + b;
	}
	if(op=="-"){
		res = - a + b;
	}
	if(op=="*"){
		res =  a * b;
	}
	if(op=="/"){
		res =  b / a;
	}
	if(op=="%"){
		ERROR();
	}
	cout<<a<<' '<<b<<endl;
	addDoubleStackPoliz(res);
	wasTrue = (res!=0);
}

void stringAri(string op){
	string a = stackPolizNum[stackPolizNum.size() - 1].getStringVal();
	stackPolizNum.pop_back();
	string b = stackPolizNum[stackPolizNum.size() - 1].getStringVal();
	stackPolizNum.pop_back();
	string res = "";
	if(op=="+"){
		res = b + a;
	}
	if(op=="-"){
		ERROR();
	}
	if(op=="*"){
		ERROR();
	}
	if(op=="/"){
		ERROR();
	}
	if(op=="%"){
		ERROR();
	}
	addStringStackPoliz(res);
	wasTrue = (res!="");
}
void intUnary(string op){
	int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
	stackPolizNum.pop_back();
	if(op=="-u")
		a*=-1;
	addIntStackPoliz(a);
	wasTrue = (a!=0);
}
void doubleUnary(string op){
	double a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
	stackPolizNum.pop_back();
	if(op=="-u")
		a*=-1;
	addDoubleStackPoliz(a);
	wasTrue = (a!=0);
}
void makeOp(string op){
	if(op == "=="){
			int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
			stackPolizNum.pop_back();
			int b = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
			stackPolizNum.pop_back();
			wasTrue = (b==a);
			if(wasTrue)
				stackPolizNum.push_back(ValInPoliz(leksem(3,"1")));
			else 
				stackPolizNum.push_back(ValInPoliz(leksem(3,"0")));
			return;
	}
	if(op == "!="){
			int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
			stackPolizNum.pop_back();
			int b = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
			stackPolizNum.pop_back();
			wasTrue = (b!=a);
			if(wasTrue)
				stackPolizNum.push_back(ValInPoliz(leksem(3,"1")));
			else 
				stackPolizNum.push_back(ValInPoliz(leksem(3,"0")));
			return;
	}
	if(op == ">"){
			int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
			stackPolizNum.pop_back();
			int b = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
			stackPolizNum.pop_back();
			wasTrue = (b>a);
			if(wasTrue)
				stackPolizNum.push_back(ValInPoliz(leksem(3,"1")));
			else 
				stackPolizNum.push_back(ValInPoliz(leksem(3,"0")));
			return;
	}
	if(op == "<"){
			int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
			stackPolizNum.pop_back();
			int b = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
			stackPolizNum.pop_back();
			wasTrue = (b<a);
			if(wasTrue)
				stackPolizNum.push_back(ValInPoliz(leksem(3,"1")));
			else 
				stackPolizNum.push_back(ValInPoliz(leksem(3,"0")));
			return;
	}
	if(op == "="){
		switch (stackPolizNum[stackPolizNum.size() - 2].getRealType()){
		case 3:
			intAsgm();
			break;
		case 4:
			doubleAsgm();
			break;
		case 5:
			stringAsgm();
			break;
		}
		wasTrue = 1;
		return;
	}
	if((op == "+")||(op == "-")||(op == "*")||(op == "/")||(op == "%")){
		switch (stackPolizNum[stackPolizNum.size() - 2].getRealType()){
		case 3:
			if(stackPolizNum[stackPolizNum.size() - 1].getRealType()==4)
				doubleAri(op);
			else
				intAri(op);
			break;
		case 4:
			doubleAri(op);
			break;
		case 5:
			stringAri(op);
			break;
		}
		wasTrue = 1;
		return;
	}
	if((op == "+u")||(op == "-u")){
		cout<<1<<endl;
		switch (stackPolizNum[stackPolizNum.size() - 1].getRealType()){
		case 3:
			intUnary(op);
			break;
		case 4:
			doubleUnary(op);
			break;
		default:
			//ERROR();
			break;
		}
	}
	if(op == "not"){
				//cout<<1213131;
				int res = 0;
				int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
				stackPolizNum.pop_back();
				//cout<<res<<endl;
				if(a == 0)
					res = 1;
				else
					res = 0;
				addIntStackPoliz(res);
				wasTrue = (res);
			}
			else
			{
				if(op=="and")
				{
				int res = 0;
				int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
				stackPolizNum.pop_back();
				int b = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
				stackPolizNum.pop_back();
				if(a * b == 0)
					res = 0;
				else
					res = 1;
				addIntStackPoliz(res);
				wasTrue = (res);
				}
				else
				{
					if(op=="or"){	
						
					int res = 0;
				int a = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
				stackPolizNum.pop_back();
				int b = stackPolizNum[stackPolizNum.size() - 1].getIntVal();
				stackPolizNum.pop_back();
					if((a  == 0)&&(b==0))
						res = 0;
					else
						res = 1;
					addIntStackPoliz(res);
					wasTrue = (res);
			
					}
			}
		}

}

void Run(){
    leksem l = poliz.getLex();
	int type = l.getType();
	//cout<<l.getName();
	switch(type){
	case 3:
	case 4:
	case 5:
	case 6:
	case 8: 
		//cout<<l.getName()<<endl;
		stackPolizNum.push_back(ValInPoliz(l));
		//cout<<stackPolizNum.size()<<' ';
		//cout<<stackPolizNum[stackPolizNum.size() - 1].getType()<<endl;
		break;
	case 1:
		makeRW(l.getName());
		break;
	case 2:
		makeOp(l.getName());
		break;
	case 7: 
		makeJumpTo(l.getName());
		break;
	}
	PC++;
}

int main(){
    init();
	try{
		scan();
	}
	catch(error e){
		cout << "mistake";
	}
	//leks.print();
	try{
		analis();
		//poliz.print();
	}
	catch(error e){
		cout << "mistake";
	}
	lexAn::next = 0;
	try{
		//stackPolizNum.push_back(ValInPoliz(leksem(8,"9")));
		//cout<<stackPolizNum[0].getIntVal();
		PC = 0;
		wasTrue = 0;
		while (true)
		{
			Run();
		}
        ERROR();
	}
	catch(finish a){

	}
		
}

