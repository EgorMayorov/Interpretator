#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>
#include <cstring>
using namespace std;

enum type_of_lex {
    LEX_NULL, /* 0 */
    /* NULL, "and", "or", "goto", "bool", "do", "while", "if", "for", "else", 
    "true", "false", "not", "int", "read", "write", "break", "program" */
    LEX_AND, LEX_OR, LEX_GOTO, LEX_BOOL, LEX_DO, LEX_WHILE, LEX_IF, LEX_FOR, LEX_ELSE, 
    LEX_TRUE, LEX_FALSE, LEX_NOT, LEX_INT, LEX_READ, LEX_WRITE, LEX_BREAK, LEX_PROGRAM, LEX_STRING, LEX_STRUCT, /* 19 */
    LEX_FIN, /* 20 */
    /* NULL, "@", ";", ":", ",", "==", "(", ")", 
    "<", ">", "+", "-", "*", "/", ">=", 
    "<=", "!=", "=", "{", "}" */
    LEX_AT, LEX_SEMICOLON, LEX_COLON, LEX_COMMA, LEX_EQUAL, LEX_OPENBRACKET, LEX_CLOSEBRACKET, 
    LEX_LESS, LEX_GREAT, LEX_PLUS, LEX_MINUS, LEX_MULTIPLY, LEX_DIVIDE, LEX_GREATOREQUAL, 
    LEX_LESSOREQUAL, LEX_NOTEQUAL, LEX_ASSIGN, LEX_OPENBRACE, LEX_CLOSEBRACE, LEX_DOT, /* 40 */
    LEX_NUM, /* 41 */
    LEX_ID, /* 42 */
    LEX_STR, /* 43 */
    POLIZ_LABEL, /* 44 */
    POLIZ_ADDRESS, /* 45 */
    POLIZ_GO, /* 46 */
    POLIZ_FGO /* 47 */
};

class Lex {     // класс Lex, объект класса хранит тип лексемы и значение
        type_of_lex t_lex;
        int v_lex;
    public:
        Lex (type_of_lex t = LEX_NULL, int v = 0): t_lex(t), v_lex(v) {}
        type_of_lex get_type () {return t_lex;}
        int get_value () {return v_lex;};
        void minus() {v_lex = 0 - v_lex;}
        friend ostream& operator << (ostream &s, Lex l);    // Вывод лексемы на экран в понятном для представления виде
};

class Ident {       // класс Ident, содержит информацию о лексемах
        string name;
        bool declare;
        type_of_lex type;
        bool assign;
        int value;
        string str_value;
    public:
        Ident (): declare(false), assign(false) {}
        bool operator == (const string& s) const {return name == s;}
        Ident (const string n) {
            name = n;
            declare = false;
            assign = false;
        }
        string get_name () {return name;}
        void put_name (const string n) {name = n;}
        bool get_declare () {return declare;}
        void put_declare () {declare = true;}
        type_of_lex get_type () {return type;}
        void put_type (type_of_lex t) {type = t;}
        bool get_assign () {return assign;}
        void put_assign () {assign = true;}
        int get_value () {return value;}
        void put_value (int v) {value = v;}
        string get_strvalue () const {return str_value;}
        void put_strvalue (string s) {str_value = s;}
};

vector <Ident> TID;     // массив идентификаторов, которые не объявлены как служебные слова

int put (const string & buf) { // функиця, записывающая в таблицу новый идентификатор или возвращающая номер записи
    vector<Ident>::iterator k;
    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end()) return k - TID.begin();
    TID.push_back(Ident(buf));
    return TID.size () - 1;
}

vector <string> TStr;       // массив строк string, используемых в программе

int put_str (const string & buf) {  // аналогичная функция для строк
    vector<string>::iterator k;
    if ((k = find(TStr.begin(), TStr.end(), buf)) != TStr.end()) return k - TStr.begin();
    TStr.push_back(buf);
    return TStr.size() - 1;
}

//static int in_struct = 0;   // количество структур

class struc{
    public:
        string name;
        vector <Ident> TIDS;
};

vector <struc> TSTRUCT;


// класс содержащий информацию о типе и имени каждой переменной - структуры
class st_type_name{                                                                                                  
    public:
        string type;
        string name;
};
// вектор классов содержащих информацию, для сохранения информации обо всех структурах
vector <st_type_name> Struct_type_name;                                                                                
// функция вывода на экран информации, содержащейся в векторе Struct_type_name
void print_Struct_type_name () {                                                                                    
    if (Struct_type_name.empty()) return;
    cout << endl << "Информация о структурах:" << endl << "Тип:    Имя:" << endl;
    for (int i = 0; i < Struct_type_name.size(); i++) 
        cout << Struct_type_name[i].type << "\t" << Struct_type_name[i].name << endl;
}



int check_struct_name (const string & buf) {        // проверка имени структуры
    for (int i = 0; i < TSTRUCT.size(); i++) {
        if (TSTRUCT[i].name == buf) {
            return i;
        }
    }
    cout << "Нет такой структуры" << endl;
    throw "Используется неинициализированный тип структуры";
}

class Scanner {     // класс Scanner, просматривает файл и выделяет из файла лексемы
        FILE *fp;
        char c;
        // функция, просматривающая, есть ли слово buf в таблице list и возвращающая номер этого слова в таблице
        int look (const string buf, const char ** list ) {
            int i = 0;
            while (list[i]) {
                if (buf == list[i]) return i;
                i++;
            }
            return 0;
        }
        void gc () {c = fgetc(fp);}
    public:
        static const char *TW[], *TD[];
        Scanner (const char* program) {
            if (!(fp = fopen(program, "r"))) throw "Невозможно открыть файл";
        }
        Lex get_lex ();
        ~Scanner () {       // в деструкторе нужно закрыть файл program
            fclose(fp);
        }
};

int bracket_balance = 0;
int brace_balance = 0;

const char * Scanner::TW[] = {
    "", "and", "or", "goto", "bool", "do", "while", "if", "for", "else",
  // 0    1      2      3       4      5      6       7     8       9     
    "true", "false", "not", "int", "read", "write", "break", "program", "string", "struct",  NULL
  //  10       11      12     13     14       15      16        17         18        19       20
}; 

const char * Scanner::TD[] = {
    "lex_fin", "@", ";", ":", ",", "==", "(", ")", "<", ">", "+", "-", "*", "/", ">=", 
  //    0       1    2    3    4     5    6    7    8    9    10   11   12   13   14
    "<=", "!=", "=", "{", "}", ".", NULL                                                      
  // 15    16    17   18  19   20    21
};

ostream & operator<< ( ostream &s, Lex l ) {
    string n;
    string table;
    if (l.t_lex < LEX_FIN) {
        n = Scanner::TW[l.t_lex];
        table = "TW";
    }else if (l.t_lex >= LEX_FIN && l.t_lex < LEX_NUM) {
        n = Scanner::TD[l.t_lex - LEX_FIN];
        table = "TD";
    }else if (l.t_lex == LEX_NUM) {
        n = "NUM";
        table = "number";
    }else if (l.t_lex == LEX_ID) {
        n = TID[l.v_lex].get_name ();
        table = "TID";
    }else if (l.t_lex == LEX_STR) {
        n = TStr[l.v_lex];
        table = "TStr";
    }else if (l.t_lex == POLIZ_LABEL) {
        n = "Label";
        table = "LABEL";
    }else if (l.t_lex == POLIZ_ADDRESS) {
        n = "Addr";
        table = "ADDRESS";
    }else if (l.t_lex == POLIZ_GO) {
        n = "!";
        table = "JUMP";
    }else if (l.t_lex == POLIZ_FGO) {
        n = "!F";
        table = "COND JUMP";
    }else throw l;
    s << table << " (" << n << ',' << l.v_lex << ");" << endl;
    return s;
}

Lex Scanner::get_lex () {
    enum state {H, IDENT, NUMB, COM, ALE, NEQ, STR};
    int d, j;    // d - число, которое мы можем встретить в программе, j - номер служебного слова
    string buf, str;    // buf - любое имя используемое в программе, str - строка string из программы
    state CS = H;
    do {
        gc();       // читаем символ в начале каждого цикла
        if (c == EOF) return Lex(LEX_FIN);     // Если файл закончился, то выходим из цикла
        switch (CS) {
            case H:
                if (c == ' ' || c == '\n' || c == '\r' || c == '\t');   // пропуск пробельных символов
                else if (isalpha(c)) {      // поиск слова 
                    buf.push_back(c);
                    CS = IDENT;
                }else if (isdigit(c)) {     // поиск числа
                    d = c - '0';
                    CS = NUMB;
                }else if (c == '/'){        // проверка на начало комментария
                    gc();
                    if (c == '*') CS = COM;
                    else{
                        ungetc(c, fp);
                        buf.push_back('/');
                        if (j = look(buf, TD)) return Lex((type_of_lex)(j + (int) LEX_FIN), j);
                        else throw c;
                    }
                }else if (c == '=' || c == '<' || c == '>') {   // поиск операции сравнения
                    buf.push_back(c);
                    CS = ALE;
                }else if (c == '@') return Lex(LEX_FIN);    // символ обоначающий завершение программы
                else if (c == '!') {        // проверка на неравенство !=
                    buf.push_back(c);
                    CS = NEQ;
                }else if (c == '"') CS = STR;       // случай строки string
                else if (c == '(' || c == ')' || c == '{' || c == '}'){
                    if (c == '(') bracket_balance++;
                    else if (c == ')') bracket_balance--;
                    else if (c == '{') brace_balance++;
                    else if (c == '}') brace_balance--;
                    if (brace_balance < 0) throw "Баланс фигурных скобок нарушен";
                    if (bracket_balance < 0) throw "Баланс круглых скобок нарушен";
                    buf.push_back(c);
                    if (j = look(buf, TD)) return Lex((type_of_lex)(j + (int) LEX_FIN), j);
                    else throw c;
                }else{
                    buf.push_back(c);
                    if (j = look(buf, TD)) return Lex((type_of_lex)(j + (int) LEX_FIN), j);
                    else throw c;
                }
                break;
            case IDENT: 
                if (isalpha(c) || isdigit(c) || c == '.') buf.push_back(c); // получаем слово идентификатор (или служебное слово)
                else {
                    ungetc(c, fp); 
                    if (j = look(buf, TW)) return Lex((type_of_lex) j, j); 
                    else {
                        j = put(buf);
                        return Lex(LEX_ID, j);
                    }
                }
                break;
            case NUMB:
                if (isdigit(c)) d = d * 10 + (c - '0'); // получаем число int
                else {
                    ungetc(c, fp);
                    return Lex(LEX_NUM, d);
                }
                break;
            case COM: 
                if (c == '*') {         // ищем конец комментария
                    gc();
                    if (c == '/') CS = H;
                    else ungetc(c, fp);     // на случай если встретим **/, чтобы комментарий завершился
                }else if (c == '@') throw c;    // даже внутри комментария заканчиваем программу если встретим @
                break;
            case ALE:
                if (c == '=') {     // >=, <=, или ==
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int) LEX_FIN), j);
                }else{              // >, <, или =
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int) LEX_FIN), j);
                }
                break;
            case NEQ:
                if (c == '=') {     // !=
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex(LEX_NOTEQUAL, j);
                }else throw '!';    //ошибка
                break;
            case STR:
                str.clear();    // очистка буфера
                while (c != '"') {      // запись строки в буфер
                    str.push_back(c); 
                    gc();
                }
                j = put_str(str);       // добавление новой строки в TStr - таблицу строк
                return Lex(LEX_STR, j);
                break;
        }
    } while (true);     // выход из цикла либо по throw либо по return
};



void print_TID () {     // вывод на экран таблицы TID
    cout << "TID:" << endl;
    if (TID.size() == 0) cout << "Таблица пустая" << endl;
    for (int k = 0; k < TID.size(); k++) 
        cout << TID[k].get_name() << " - " << k << endl;
}

void print_TStr () {    // вывод на экран таблицы TStr
    cout << "TStr:" << endl;
    if (TStr.size() == 0) cout << "Таблица пустая" << endl;
    for (int k = 0; k < TStr.size(); k++)
        cout << TStr[k] << " - " << k << endl;
}

///// Parser /////

// аргументы функции - стек и элемент такого же типа, что и элементы стека
// функция присваивает второму операнду верхушку стека и изымает этот элемент из стека
template <class T, class T_EL>          
void from_st (T & st, T_EL & i) {
    i = st.top(); 
    st.pop();
}

stack <vector <int> > stbreak;      // стек векторов для прерываний циклов с помощью break
// это стек на случай вложенных циклов. Вектор int содержит информацию о места прерываний
// то есть о местах в ПОЛИЗе, куда нужно будет положить адреса для выходов по break
// это вектор


// в TGOTO и checkJ хранится номер идентификатора (метки) в таблице TID
// в numJ хранится адрес первого встретившегося в программе goto с данной меткой - адрес перехода в ПОЛИЗе 
vector <int> TGOTO;             // вектор меток для оператора GoTo, откуда можно переходить и куда можно переходить
                                // он содержит информацию обо всех метках
vector <int> numJ;              // вектор номеров меток для оператора GoTo, куда можно переходить и откуда можно переходить
vector <int> checkJ;            // вектор меток, по которым мы переходим

void print_goto_vec (vector <int> A) {    // вывод на экран векторов выше
    if (A.size() == 0) cout << "Вектор пустой" << endl;
    for (int k = 0; k < A.size(); k++)
        cout << A[k] << " - " << k << endl;
}

void check_jump (int n){        // проверка метки GoTo не объявлена ли она дважды и добавление новой метки в таблицу
    vector<int>::iterator k;
    if ((k = find(checkJ.begin(), checkJ.end(), n)) != checkJ.end()) {
        cout<<"Дважды объявлена одна метка GoTo" << endl;
        throw "Метка GoTo объявлена дважды";
    }else checkJ.push_back(n);
}

int putgoto (const int buf, const int n) {      // добавление новой метки GoTo в вектор адресов меток
    vector<int>::iterator k;
    if ((k = find(TGOTO.begin(), TGOTO.end(), buf)) != TGOTO.end())  
        return numJ[k-TGOTO.begin()];
    if (n != 0) {
        TGOTO.push_back(buf);
        numJ.push_back(n);
    }
    return 0;
}

class Parser {
    Lex          curr_lex;      // текущая лексема
    type_of_lex  c_type;        // тип текущей лексемы
    int          c_val;         // значение текущей лексемы
    Scanner      scan;          // сканер, получающий очередную лексему
    stack <int> st_int;         // стек чисел - переменных, которые будут инициализованы
    stack <type_of_lex> st_lex; // стек типов лексем - для ПОЛИЗа
    stack <string> st_struct;   // стек имен структур для проверки верности присваивания                                
    void  P();                  // проверка начала программы и инициализации переменных
    void  STRUC();              // проверка верности структур
    void  D1();                 // проверка верности начальной инициализации
    void  D();                  // проверка названий переменных в начальной инициализации
    void  B();                  // проверка целостности программы от { до }
    void  S();                  // проверка зарезервированных функций программы (for, if, write, goto, while ...)
    void  A();                  // проверка присваивания
    void  OR();                 // проверка верности or
    void  AND();                // проверка верности and
    void  E();                  // проверка сравнений
    void  PM();                 // проверка + и -
    void  MD();                 // проверка * и /
    void  Fin();                // проверка верности переменных, строк и констант
    void  dec (type_of_lex type);   // внесение информации об идентификаторе класса Ident
    void  check_id ();          // проверка была ли инициализирована переменная (идентификатор)
    void  check_op ();          // проверка возможности применить операцию (проверка типов операндов)
    void  check_not ();         // проверка возможности применить операцию not (применима только к boolean)
    void  eq_type ();           // проверка равенства типов при присваивании
    void  eq_bool ();           // проверка принадлежности к типу boolean
    void  check_id_in_read ();  // проверка 
    void  gl () {               // проверка была ли внесена информация об идентификаторе (Ident)
        curr_lex  = scan.get_lex ();        // получение и печать на экран очередной лексемы
        c_type    = curr_lex.get_type ();
        cout << curr_lex;
        c_val     = curr_lex.get_value ();
    }
public:
    vector <Lex> poliz;         // вектор ПОЛИЗа
    Parser (const char *program): scan (program) {}
    void analyze();
};

// функция, проверяющая чтобы код программы верно закончился и выводит полученный ПОЛИЗ
void Parser::analyze () {       
    //cout << "ANALYZE" << endl;
    gl();       // получение первой лексемы
    P();        // обработка кода программы
    if (c_type != LEX_FIN) throw curr_lex;      // если последняя не FIN то ошибка
    
    /*cout << endl << "TGOTO" << endl;
    print_goto_vec(TGOTO);
    cout << "numJ" << endl;
    print_goto_vec(numJ);
    cout << "checkJ" << endl;
    print_goto_vec(checkJ);*/
    
    if (TGOTO.size() != checkJ.size()) throw "Есть проблема с метками goto";
    
    /*cout << endl;
    print_TID();*/
    
    cout << endl << "Полученный ПОЛИЗ" << endl;
    for (Lex l : poliz) cout << l;          // вывод полученных лексем ПОЛИЗа
    cout << "Конец ПОЛИЗа" << endl << endl;
    
    // вывод информации из вектора Struct_type_name, который содержит информацию о типе и имени каждой переменной-структуры
    //print_Struct_type_name();                                                                                      
}

// функция, проверяющая program в начале кода программы и вызывающая фунции проверки верности инициализации структор и переменных
// и начинающая проверку программы после инициализации
void Parser::P () {             
    //cout << "P" << endl;
    if (c_type == LEX_PROGRAM) {        // проверка что программа начинается со слова "program"
        gl();
    }else throw curr_lex;           // если программа начинается не с program то это ошибка
    while (c_type == LEX_STRUCT) {      // обработка структур, struct идут сразу после "program"
        STRUC();
        gl();
    }
    D1();                  // когда структуры закончились мы обрабатываем инициализацию переменных
    if (c_type != LEX_OPENBRACE) throw curr_lex;        // проверка наличия { после инициализации        
    B();                   // проверка тела кода программы (в {})
}

/*
Все имена переменных попадают в таблицу TID на этапе лексического анализа.
Если переменная инициализируется, то поле declare в классе Ident принимает значение tue (изначально false).
Если же переменная объявляется только как поле структуры, то она попадает в таблицу TIDS данной структуры, а значение declare 
идентификатора остается false, поэтому обратиться к переменной (идентификатору), как к самостоятельной переменной нельзя.
Если идентификатор (переменная) объявляется и среди переменных, используемых в программе и как поле структуры, то эти два
идентификатора будут иметь разные "смыслы" несмотря на одинаковые названия.
*/

// проверка верности описания структуры
void Parser::STRUC(){
    //cout << "STRUC" << endl;
    type_of_lex buft1;  // тип лексемы поля структуры
    Ident buft2;        // сохранение информации о поле структуры
    struc buft;         // сохранение информации о структуре
    gl();               // получение новой лексемы сразу после слова "struct"
    if (c_type != LEX_ID) {       // проверка существования имени структуры, если имя не написано - ошибка
        cout<<"Не найдено имя структуры" << endl;
        throw curr_lex;
    }
    buft.name = TID[c_val].get_name();     // записываем в буфер информации о структуре имя структуры
    gl();                               // взятие следующей за именем структуры лексемы
    if (c_type != LEX_OPENBRACE) {       // проверка наличия { после имени, если нет - ошибка
        cout<<"Необходима {" << endl;
        throw curr_lex;
    }
    gl();
    while (c_type != LEX_CLOSEBRACE) {       // проверка полей структуры пока не встретим }
        if (c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING) {      // выбирается типа переменной
            if (c_type == LEX_STRING) buft1 = LEX_STR;  // если тип string, то следующие идентификаторы (переменные) именют тип лексемы LEX_STR
            else buft1 = c_type;     // если тип int или boolean, то идентификаторы также имеют тип int или boolean
        }else{                    // если тип полей описан неверно, то это ошибка
            cout << "Неизвестный тип" << endl;
            throw curr_lex;
        }
        gl();
        while (c_type != LEX_SEMICOLON) {        // перебор всех полей данного типа, объявленных через запятую
            if (c_type != LEX_ID) {     // если идентификатор имеет запрещенное имя, то это ошибка
                cout << "Ошибка инициализации в структуре" << endl;
                throw curr_lex;
            }
            buft2.put_name(TID[c_val].get_name());      // кладем имя поля в буфер (класса Ident)
            buft2.put_type(buft1);                      // и тип поля кладем туда же
            buft.TIDS.push_back(buft2);                 // записываем этот буфер в таблицу TIDS, содержащую информацию о полях данной структуры
            gl();
            if (c_type == LEX_COMMA) gl();              // пропускаем запятые, они в обработке при инициализации не нуждаются
            else if(c_type != LEX_SEMICOLON){   // после имени нового поля должна быть , или ; иначе ошибка 
                cout << "Не встречено ; где это необходимо" << endl;
                throw curr_lex;
            }
        }
        gl();
    }
    TSTRUCT.push_back(buft);        // записываем в вектор содержащий информацию о структурах новую структуру
}

// проверка верности начальной инициализации переменных
void Parser::D1 () {
    //cout << "D1" << endl;
    if (c_type == LEX_INT || c_type == LEX_BOOL || c_type == LEX_STRING) {     // нахождение типа переменной
        D ();
        while (c_type == LEX_SEMICOLON) {
            gl ();
            if (c_type != LEX_INT && c_type != LEX_BOOL && c_type != LEX_STRING) break;     // проверка чтобы не был дважды объявлен тип
            D();
        }
    }else if (c_type == LEX_ID) {    // нахождение имени структуры
        int j, val_buf;
        string struct_type;     // строка, для хранения типа структуры                                                  
        st_type_name type_name;       // переменная, которая хранит в себе тип и имя структуры                         
        j = check_struct_name(TID[c_val].get_name());
        struct_type = TID[c_val].get_name();        // запоминаем тип структуры                                         
        type_name.type = struct_type;       // записываем это имя в переменную, для дальнейшей записи в вектор          
        gl();
        type_name.name = TID[c_val].get_name();     // запоминаем имя переменной (тип переменной - структура)          
        Struct_type_name.push_back(type_name);     // кладем информацию о переменной в вектор                           
        // теперь элемент вектора содержит информацию о имени структуры и ее типе
        /* например если есть запись    
            struct s {...}
            s s1, s2;
        то элемент вектора будет хранить тип - s, и имя - s1;
        */
        string buft3;
        st_int.push(c_val);
        dec(LEX_STRUCT);
        for (int i = 0; i < TSTRUCT[j].TIDS.size(); i++) {      // нахождение и проверка переменных в струкрутах
            buft3 = TID[c_val].get_name();
            buft3 = buft3 + '.' + TSTRUCT[j].TIDS[i].get_name(); // переменные внутри структуры записываются через точку
            val_buf = put(buft3);           // добавление имени поля структуры в таблицу полей структуры
            st_int.push(val_buf);
            dec(TSTRUCT[j].TIDS[i].get_type());
        }
        gl();
        while (c_type == LEX_COMMA) {
            gl();
            type_name.type = struct_type;       // аналогичные действия по запоминанию типа                            
            type_name.name = TID[c_val].get_name();     // и имени                                                     
            Struct_type_name.push_back(type_name);     // и запись в вектор                                            
            // аналогично как c типом s и именем s1, в вектор попадает новая запись о структуре с типом s и именем s2
            st_int.push(c_val);
            dec(LEX_STRUCT);
            for (int i = 0; i < TSTRUCT[j].TIDS.size(); i++) {      // нахождение и проверка переменных в струкрутах
                buft3 = TID[c_val].get_name();
                buft3 = buft3 + '.' + TSTRUCT[j].TIDS[i].get_name();
                val_buf = put(buft3);           // добавление имени поля структуры в таблицу полей структуры
                st_int.push(val_buf);
                dec(TSTRUCT[j].TIDS[i].get_type());
            }
            gl();
        }
        if (c_type != LEX_SEMICOLON) {
            cout << "Не встречено ; где это необходимо" << endl;
            throw curr_lex;
        }
        gl();
        if(c_type != LEX_OPENBRACE) D1();       // новый тип переменной - новый оборот в рекурсии
    }else throw curr_lex;
}

// функция, проверяющая верность присваивания переменным начальных значений при инициализации и имена переменных
void Parser::D () {
    //cout << "D" << endl;
    type_of_lex buf1;
    buf1 = c_type;
    gl();
    if (c_type != LEX_ID) throw curr_lex;       // если инициализируется не переменная, то это ошибка
    else{
        st_int.push(c_val);
        int buf5 = c_val;
        gl ();
        if (c_type == LEX_ASSIGN) {       // проверка происходящего при присваивании
            poliz.push_back(Lex(POLIZ_ADDRESS, buf5));      // в стек ПОЛИЗа попадает значение переменной
            gl();                       // тут начинается проверка того, что присваивается переменной
            if ((buf1 == LEX_INT && (c_type == LEX_MINUS || c_type == LEX_NUM)) ||
               (buf1 == LEX_STRING && c_type == LEX_STR)) {  
                if (c_type == LEX_MINUS) {      // например a = -b; идет проверка минуса
                    gl();
                    if (c_type == LEX_NUM) curr_lex.minus();
                    else{
                        cout << "Не найдено число" << endl;
                        throw curr_lex;
                    }
                }
                poliz.push_back(curr_lex);      // в стек ПОЛИЗа попадает присваиваемое значение 
                gl();
                poliz.push_back(Lex(LEX_ASSIGN));       // в стек ПОЛИЗа кладем знак =
            }else if (buf1 == LEX_BOOL && (c_type == LEX_TRUE || c_type == LEX_FALSE)) {
                if (c_type == LEX_TRUE) poliz.push_back(Lex(LEX_TRUE, 1));      // кладем true в стек ПОЛИЗа
                else if (c_type == LEX_FALSE) poliz.push_back(Lex(LEX_FALSE, 0));     // кладем в стек ПОЛИЗа false
                gl();
                poliz.push_back(Lex(LEX_ASSIGN));       // в стек ПОЛИЗа кладем знак =
            }else{
                cout << "Неверная начальная инициализация" << endl;
                throw curr_lex;
            }
        }
        while (c_type == LEX_COMMA) {           // повторяем описанные выше действия, если в программе они идут через запятую
            gl ();
            if (c_type != LEX_ID) throw curr_lex;   // если что-то присваивается не переменной, то это ошибка
            else{
                st_int.push(c_val);
                buf5 = c_val;
                gl();
                if (c_type == LEX_ASSIGN) {     // проверка происходящего при присваивании
                    poliz.push_back(Lex(POLIZ_ADDRESS, buf5));      // в стек ПОЛИЗа попадает значение переменной
                    gl();                       // тут начинается проверка того, что присваивается переменной
                    if ((buf1 == LEX_INT && (c_type == LEX_MINUS || c_type == LEX_NUM)) ||
                       (buf1 == LEX_STRING && c_type == LEX_STR)) {  
                        if (c_type == LEX_MINUS) {      // например a = -b; идет проверка минуса
                            gl();
                            if (c_type == LEX_NUM) curr_lex.minus();
                            else{
                                cout << "Не найдено число" << endl;
                                throw curr_lex;
                            }
                        }
                        poliz.push_back(curr_lex);      // в стек ПОЛИЗа попадает присваиваемое значение 
                        gl();
                        poliz.push_back(Lex(LEX_ASSIGN));       // в стек ПОЛИЗа кладем знак =
                    }else if (buf1 == LEX_BOOL && (c_type == LEX_TRUE || c_type == LEX_FALSE)) {
                        if (c_type == LEX_TRUE) poliz.push_back(Lex(LEX_TRUE, 1));      // кладем true в стек ПОЛИЗа
                        else if (c_type == LEX_FALSE) poliz.push_back(Lex(LEX_FALSE, 0));     // кладем в стек ПОЛИЗа false
                        gl();
                        poliz.push_back(Lex(LEX_ASSIGN));       // в стек ПОЛИЗа кладем знак =
                    }else{
                        cout << "Неверная начальная инициализация" << endl;
                        throw curr_lex;
                    }
                }
            }
        }
        if (c_type != LEX_SEMICOLON) throw curr_lex;    // ошибка если нет ; в конце инициализации переменной
        else{
            if (buf1 == LEX_INT) dec(LEX_INT);      // переменная типа int инициализирована - это записывается в идентификатор Ident переменной
            else if (buf1 == LEX_BOOL) dec(LEX_BOOL);       // переменная типа bool инициализирована 
            else if (buf1 == LEX_STRING) dec(LEX_STR);      // переменная типа string инициализирована 
            else throw curr_lex;                // иначе лексема имеет неверный тип
        }
    }
}

// функция, проверяющая что тело программы начинается с { и заканчивается }
// эта функция вызывает функцию S(), проверяющую верность всех операций
void Parser::B () {
    //cout << "B" << endl;
    if (c_type == LEX_OPENBRACE) {      // проверка наличия {
        //gl ();
        //S ();     // хотя бы один раз вызывается функция S - тело программы не пустое
                    // если тело программы пустое, то получаем бесконечный цикл
                    // но от него легко избавиться, удалив строки gl(); S();    (сейчас это сделано)
        gl();
        while (c_type != LEX_CLOSEBRACE) {      // цикл, пока не встретится }
            S ();
            if (!(c_type == LEX_CLOSEBRACE && brace_balance == 0)) gl();
            if (c_type == LEX_FIN) throw "Не найдена } в конце программы";
        }
        if (c_type == LEX_CLOSEBRACE) {         // корректное завершение при встрече последней }
            cout << "Код программы введен корректно" << endl;
            gl ();
        }else throw curr_lex;
    }else throw curr_lex;
}

// функция, выполняющая проверку всех операций программы, таких как if, for, while, read, и других из таблицы TW
void Parser::S () {
    //cout << "S" << endl;
    int pl0, pl1, pl2, pl3, pl4, pl5, pl6, pl7;
    if (c_type == LEX_OPENBRACE) {        // встретилось выражение в фигурных скобках
        gl();
        while (c_type != LEX_CLOSEBRACE) {    // вызов себя же пока не встретится }
            S();
            gl();
        }
    }// операторы в {} закончились
    else if (c_type == LEX_ID) {        // случай, если мы встретили идентификатор (переменную)
        if (TID[c_val].get_declare() == false) {    // если она встретилась первый раз 
            int buf7 = c_val;
            gl();
            if (c_type == LEX_COLON) {          // и после стоит двоеточие
                check_jump(buf7);               // то возможно это метка перехода - проверим это
                int buf8;
                if ((buf8 = putgoto(buf7, poliz.size())) != 0) {
                    int buf9 = poliz.size();
                    poliz[buf8] = Lex(POLIZ_LABEL, buf9);   // вставляем в нужное место стека ПОЛИЗа адрес перехода GoTo
                }
                gl();  
                S();
            }else{
                cout<<"Переменная не инициализирована" << endl;
                throw curr_lex;
            }
        }else A();                  // если встреченная переменная уже инициализована (функцией dec) то идем дальше
    }// конец случая идентификатора
    else if (c_type == LEX_IF) {      // проверка верности функции if и внесение всех необходимых данных в стек ПОЛИЗа
        gl ();
        if (c_type != LEX_OPENBRACKET) throw curr_lex;    // обязательно наличие ( сразу после "if"
        gl();
        A();
        eq_bool();
        pl2 = poliz.size();                     // запоминаем куда класть адрес перехода по false
        poliz.push_back(Lex());                 // кладем пустую лексему, чтобы потом заменить ее адресом перехода
        poliz.push_back(Lex(POLIZ_FGO));        // кладем в стек ПОЛИЗа команду перехода по false
        if (c_type == LEX_CLOSEBRACKET) {       // обязательно наличие ) после условия
            gl();   
            S();                // проверка и заполнение тела if 
            pl3 = poliz.size();                 // запоминаем куда класть адрес безусловного перехода
            poliz.push_back(Lex());             // кладем пустую лексему, чтобы заменить ее потом адресом безусловного перехода
            poliz.push_back(Lex(POLIZ_GO));     // кладем в полиз безусловный переход
            poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());    // перед условным переходом кладем метку - куда переходить
            if (c_type == LEX_CLOSEBRACE) gl();         // если встретили } - берем следующую лексему
            if (c_type == LEX_SEMICOLON) gl();          // если встретили ; - берем следующую лексему
            if (c_type == LEX_ELSE) {           // если встретили else - берем новую лексему и вызываем рекурсивно функцию S()
                gl();
                S();                            // заполнение и проверка тела else
                poliz[pl3] = Lex(POLIZ_LABEL, poliz.size());        // адрес перехода по else 
            }else throw curr_lex;
        }else throw curr_lex;
    }// конец if
    else if (c_type == LEX_WHILE) {         // проверка верности цикла while и внесение необходимых операций в стек ПОЛИЗа
        pl0 = poliz.size();
        gl();
        stbreak.push(vector<int>());
        if (c_type != LEX_OPENBRACKET) throw curr_lex;      // условие while обязательно должно начинаться с (
        gl();
        A();
        eq_bool();              // условие while должно быть типа boolean
        pl1 = poliz.size();
        poliz.push_back(Lex());         // кладем в ПОЛИЗ пустую лексему чтобы позже положить туда адрес перехода
        poliz.push_back(Lex(POLIZ_FGO));        // кладем в полиз переход по false
        if (c_type == LEX_CLOSEBRACKET) {           // условие while должнно заканчиваться ), иначе - ошибка
            gl();
            S();
            poliz.push_back(Lex(POLIZ_LABEL, pl0));         // после тела while кладем в стек ПОЛИЗа адрес перехода в начало цикла
            poliz.push_back(Lex(POLIZ_GO));                 // кладем в ПОЛИЗ команду безусловного перехода
            poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());    // на место пустой лексемы кладем адрес переода по false
            while (!stbreak.top().empty()) {                    // из стека stbreak переносим в ПОЛИЗ адрес для break, если такой есть
                poliz[stbreak.top().back()] = Lex(POLIZ_LABEL, poliz.size());
                stbreak.top().pop_back();
            }
            stbreak.pop();
        }else throw curr_lex;
    }// конец while
    else if (c_type == LEX_BREAK) {     // проверка верности оператора break и внесение необходимых данных в ПОЛИЗ
        if (stbreak.size() == 0) {      // если стек stbreak отвечающий за прерывания пустой (то есть мы находимся не в цикле), то это ошибка
            cout << "Нельза прерваться с break - нет цикла" << endl;
            throw curr_lex;
        }
        stbreak.top().push_back(poliz.size());  // кладем в стек stbreak размер стека ПОЛИЗа - чтобы знать место прерывания
        poliz.push_back(Lex());     // кладем в стек ПОЛИЗа пустую лексему, которая будет заменена адресом
        poliz.push_back(Lex(POLIZ_GO));     // кладем в стек ПОЛИЗа коменду перехода
        gl();
        if (c_type != LEX_SEMICOLON) {      // после оператора break необходима ;
            cout<<"Необходима ; после оператора break" << endl;
            throw curr_lex;
        }
    }// конец break
    else if (c_type == LEX_GOTO) {      // проверка верности оператора GoTo и внесение необходимых данных в стек ПОЛИЗа
        gl();
        if (c_type == LEX_ID) {         // после оператора должна стоять метка - идентификатор перехода
            int buf6 = putgoto(c_val, 0);   // проверяем значение, куда нужно перейти
            if (buf6 != 0) {            // если такое уже встречалось, то
                poliz.push_back(Lex(POLIZ_LABEL, buf6));    // кладем в стек ПОЛИЗа адрес перехода
                poliz.push_back(Lex(POLIZ_GO));             // и команду перехода
            }
            else{               // если не встречалось, то запоминаем его
                putgoto(c_val, poliz.size());       
                poliz.push_back(Lex());         // кладем пустую лексему в стек ПОЛИЗа, чтобы потом заменить на адрес перехода
                poliz.push_back(Lex(POLIZ_GO)); // операция безусловного перехода в стек ПОЛИЗа
            }
            gl();
            if (c_type != LEX_SEMICOLON) {      // в конце обязательно должна быть ;
                cout << "Необходима ; псле метки GoTo" << endl;
                throw curr_lex;
            }
        }else{                  // если после слова goto нет метки, то это ошибка
            cout<<"Ошибка GoTo";
            throw curr_lex;
        }
    }// конец GoTo
    else if (c_type == LEX_READ) {      // проверка верности оператора read и внесение необходимых данных в ПОЛИЗ
        gl ();
        if (c_type == LEX_OPENBRACKET) {        // после слова read должна стоять (
            gl ();
            if (c_type == LEX_ID) {         // проверка идентификатора (переменной) для операции read
                check_id_in_read();
                poliz.push_back(Lex(POLIZ_ADDRESS, c_val));         // кладем в ПОЛИЗ адрес переменной
                gl();
            }else throw curr_lex;
            if (c_type == LEX_CLOSEBRACKET) {       // после обязательно должна быть )
                gl();
                if (c_type != LEX_SEMICOLON) {      // после оператора должна стоять ;
                    cout << "Необходима ;" << endl;
                    throw curr_lex;
                }
                poliz.push_back(Lex(LEX_READ));     // кладем лексему read в ПОЛИЗ
            }else throw curr_lex;
        }else throw curr_lex;
    }// конец read
    else if (c_type == LEX_WRITE) {         // проверка верности оператора write и внесение необходимых данных в стек ПОЛИЗа
        gl();
        if (c_type == LEX_OPENBRACKET) {        // после слова write должна стоять (
            gl();
            A();       // нахождение операндов write
            poliz.push_back(Lex(LEX_WRITE));        // внесение лексемы write в стек ПОЛИЗа
            while (c_type == LEX_COMMA) {   // если операндов несколько, то учтем их все
                gl();
                A();        // нахождение операндов write
                poliz.push_back(Lex(LEX_WRITE));
            }
            if (c_type == LEX_CLOSEBRACKET) {       // после операндов должна стоять )
                gl();
                if (c_type != LEX_SEMICOLON) {      // в конце обязательно нужно ;
                    cout << "Необходима ;" << endl;
                    throw curr_lex;
                }
            }else throw curr_lex;
        }else throw curr_lex;
    }// конец write
    else if (c_type == LEX_FOR) {       // проверка верности цикла for и внесение необходимых операций в стек ПОЛИЗа
        stbreak.push(vector<int>());
        gl();
        if(c_type != LEX_OPENBRACKET) throw curr_lex;       // условие цикла for обязательно должно начинаться с (
        gl();
        if (c_type != LEX_SEMICOLON) {          // проверка условия в for (нужное; *; *)
            A();                           
            if (c_type != LEX_SEMICOLON) throw curr_lex;
        }
        gl();
        pl6 = poliz.size();                 // запоминание индекса ПОЛИЗа, куда после положится адрес перехода
        if (c_type != LEX_SEMICOLON) {      // проверка второго условия в for(*; нужное; *)
            A();
            if (c_type != LEX_SEMICOLON) throw curr_lex;
        }
        gl();
        pl4 = poliz.size();                 // запоминание инндекса элемента ПОЛИЗа, куда после будет положен адрес перехода
        poliz.push_back(Lex());             // кладем в ПОЛИЗ пусую лексему, чтобы потом заменить ее на нужный адрес перехода
        poliz.push_back(Lex(POLIZ_FGO));    // кладем в ПОЛИЗ операцию безусловного перехода
        pl5 = poliz.size();                 // запоминаем индекс элемента ПОЛИЗа, куда после будет положен адрес перехода
        poliz.push_back(Lex());             // кладем пустую лексему, чтобы после заменить ее на нужный адрес перехода
        poliz.push_back(Lex(POLIZ_GO));     // кладем в ПОЛИЗ команду безусловного перехода
        pl7 = poliz.size();                 // запоминаем индекс элемента ПОЛИЗа, куда позже положим адрес перехода
        if (c_type != LEX_SEMICOLON && c_type != LEX_CLOSEBRACKET) {    // проверка последнего условия в цикле for (*; *; нужное)
            A();                            
            if (c_type != LEX_CLOSEBRACKET) throw curr_lex;     // в конце условия for должна быть )
        }
        poliz.push_back(Lex(POLIZ_LABEL, pl6));         // кладем адрес выхода из ПОЛИЗа после условий цикла
        poliz.push_back(Lex(POLIZ_GO));                 // кладем в ПОЛИЗ операцию безусловного перехода
        poliz[pl5] = Lex(POLIZ_LABEL, poliz.size());    // кладем адрес перехода в нужное место (pl5) ПОЛИЗа
        if (c_type != LEX_CLOSEBRACKET) throw curr_lex;  // тело цикла должно заканчиваться }
        gl();
        S();                                        // заполняем тело цикла for
        poliz.push_back(Lex(POLIZ_LABEL, pl7));     // кладем в ПОЛИЗ адрес перехода на начало цикла
        poliz.push_back(Lex(POLIZ_GO));             // и кладем операцию безусловного перехода
        poliz[pl4] = Lex(POLIZ_LABEL, poliz.size());    // в нужное место (pl4) ПОЛИЗа кладем необходимый адрес перехода
        while (!stbreak.top().empty()) {            // заполняем адреса выходов по break если они есть
            poliz[stbreak.top().back()] = Lex(POLIZ_LABEL, poliz.size());
            stbreak.top().pop_back();
        }
        stbreak.pop();
    }// конец for
}

// функция, проверяющая наличие и верность присваивания в программе
// и вызывающая нижележащие функции, которые также вызывают функции ниже до F()
void Parser::A (){
    //cout << "A" << endl;
    OR();
    if (c_type == LEX_ASSIGN) {
        gl();
        A();            // возможность нескольких присваиваний
        eq_type();      // проверка типов переменных стоящих по разные стороны от знака =
        poliz.push_back(Lex(LEX_ASSIGN));       // кладем в стек знак =
    }
}

// функция, проверяющая наличие и верность операции or
void Parser::OR (){
    //cout << "OR" << endl;
    AND();
    if (c_type == LEX_OR) {
        st_lex.push(c_type);    // кладем в стек операций or, если такая операция встретилась
        gl();
        AND();
        check_op();        // проверка типов переменных, стоящих с разных сторон от or
    }
}

// функция, проверяющая наличие и верность операции and
void Parser::AND (){
    //cout << "AND" << endl;
    E();
    if (c_type == LEX_AND) {
        st_lex.push(c_type);    // кладем в стек операций and, если такая операция встретилась
        gl();
        E();
        check_op();         // проверка типов переменных, стоящих с разных сторон от and
    }
}

// функция, проверяющая наличие и верность операций сравнения
void Parser::E () {
    //cout << "E" << endl;
    PM();
    if (c_type == LEX_EQUAL  || c_type == LEX_LESS || c_type == LEX_GREAT ||
       c_type == LEX_LESSOREQUAL || c_type == LEX_GREATOREQUAL || c_type == LEX_NOTEQUAL) {
        st_lex.push(c_type);    // кладем операцию в стек операций, если такая встретилась
        gl();
        PM();
        check_op();         // проверка типов переменных, стоящих в с разных сторон операции
    }
}

// функция, проверяющая наличие и верность операций + и -
void Parser::PM () {
    //cout << "PM" << endl;
    MD();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS) {
        st_lex.push(c_type);    // кладем операцию в стек операций
        gl();
        MD();
        check_op();        // проверяем верность типов операндов
    }
}

// функция, проверяющая наличие и верность операций * и /
void Parser::MD () {
    //cout << "MD" << endl;
    Fin();
    while (c_type == LEX_MULTIPLY || c_type == LEX_DIVIDE) {
        st_lex.push(c_type);    // кладем операцию в стек операций
        gl();
        Fin();       
        check_op();         // проверка верности типов операндов
    }
}

// функция, проверяющая знаки, скобки, строки, переменные и числа
// и заполняющая стек ПОЛИЗа
void Parser::Fin () {
    //cout << "Fin" << endl;
    if (c_type == LEX_ID) {         // проверка переменной
        check_id();
        int buf2;
        buf2 = c_val;
        gl();
        // если мы присваиваем две структуры, то нам нужно положить в стек ПОЛИЗа не адрес структуры, а имя
        // чтобы на этапе исполнения присвоить поля структуры по именам
        if (c_type == LEX_ASSIGN) {
            if (TID[buf2].get_type() == LEX_STRUCT) poliz.push_back(Lex(LEX_ID, buf2));     // !!!!!
            else poliz.push_back(Lex(POLIZ_ADDRESS, buf2));     // при присваивании кладем в стек ПОЛИЗа адрес переменной
        }else poliz.push_back(Lex(LEX_ID, buf2));       // если не присваивание, то кладем в стек ПОЛИЗа значение переменной

        
    }else if (c_type == LEX_NUM) {      // проверка числа
        st_lex.push(LEX_INT);
        poliz.push_back(curr_lex);      // кладем в стек ПОЛИЗа число
        gl();
    }else if (c_type == LEX_STR) {      // проверка строки
        st_lex.push(LEX_STR);
        poliz.push_back(curr_lex);      // кладем в стек ПОЛИЗа строку
        gl();
    }else if (c_type == LEX_TRUE) {     // проверка true
        st_lex.push(LEX_BOOL);
        poliz.push_back(Lex(LEX_TRUE, 1));      // кладем true в стек ПОЛИЗа
        gl();
    }else if (c_type == LEX_FALSE) {     // проверка false
        st_lex.push(LEX_BOOL);
        poliz.push_back(Lex(LEX_FALSE, 0));     // кладем в стек ПОЛИЗа false
        gl();
    }else if (c_type == LEX_NOT) {      // проверка not
        gl();
        Fin();      // рекурсивный вызов функции для дальнейшей проверки
        check_not();
    }else if (c_type == LEX_OPENBRACKET) {      // проверка происходящего в скобках
        gl();
        A();
        if (c_type == LEX_CLOSEBRACKET) gl();
        else throw curr_lex;            // ошибка, если не найдена закрывающая скобка )
    }else if (c_type == LEX_MINUS) {     // проверка унарного минуса
        Lex zero(LEX_NUM, 0);
        cout << zero;
        st_lex.push(LEX_INT);
        poliz.push_back(zero);          // кладем в стек 0 для изменения выражения справа от унарного минуса
    }else throw curr_lex;               // ошибка, при неверном типе лексемы
}

// считывание из стека номер строки таблицы TID и занесение информации о типе соответствующих переменных,
// о наличии их описаний и контроллирует повторное описание переменных
void Parser::dec (type_of_lex type) {       // внесение информации о типе идентификатора класса Ident
    //cout << "dec" << endl;
    int i;                                  // и идентификация переменной в программе
    while (!st_int.empty()) {
        from_st(st_int, i);
        if (TID[i].get_declare()) throw "Идентификация переменной второй раз";
        else{
            TID[i].put_declare();
            TID[i].put_type(type);
        }
    }
}

// проверка была ли переменная описана, если да то ее тип надо занести в стек, чтобы потом проверить равенство типов для операции
void Parser::check_id () {              // проверка идентификатора - был ли он инициализирован
    //cout << "check_id" << endl;
    if (TID[c_val].get_declare()) {                 // если идектификатор инициализирован (с помощью dec), то 
        st_lex.push(TID[c_val].get_type());     // положить в st_lex его тип
        
        // если мы присваиваем структуры друг другу, то кладем имя структуры в стек для проверки при присваивания типа структуры
        if (TID[c_val].get_type() == LEX_STRUCT) st_struct.push(TID[c_val].get_name());                             
        
    }else throw "Не объявленный идентификатор";
}

// проверка соответствия типов операндов данной двуместной операции
void Parser::check_op () {      // проверка операции - возможно ли такое в программе или запрещено языком
    //cout << "check_op" << endl;
    type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;
    from_st(st_lex, t2);        // берем из стека операцию и операнды
    from_st(st_lex, op);
    from_st(st_lex, t1);
    if (t1 == t2 && t1 == LEX_STR) {    // случай для строки
        if (op == LEX_LESS || op == LEX_GREAT || op == LEX_EQUAL || op == LEX_NOTEQUAL) st_lex.push(LEX_BOOL);
        else if(op == LEX_PLUS || op == LEX_EQUAL) st_lex.push(LEX_STR);
        else throw "Неверные типы операндов"; 
    }else{              // случай для числа
        if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_MULTIPLY || op == LEX_DIVIDE) r = LEX_INT;
        if (op == LEX_OR || op == LEX_AND) t = LEX_BOOL;
        if (t1 == t2 && t1 == t) st_lex.push(r);        // кладем обратно в стек тип операции на случай нескольких операций
        else throw "Неверные типы операндов";
    }
    poliz.push_back(Lex(op));       // кладем лексему - операцию в ПОЛИЗ
}

void Parser::check_not () {     // проверка возможности применить операцию not
    //cout << "check_not" << endl;
    if (st_lex.top() != LEX_BOOL) throw "Неверный тип для операции not";
    else poliz.push_back(Lex(LEX_NOT));
}

void Parser::eq_type () {       // проверка равенства типов при присваивании
    //cout << "eq_type" << endl;
    type_of_lex t;
    from_st(st_lex, t);
    if (t != st_lex.top()) throw "Неверные типы операндов в присваивании";
    
    // если присваиваются структуры, то нужно проверить чтобы они были одного типа                                   
    if (t == LEX_STRUCT) { 
        string n1, n2, t1, t2;
        // берем имена двух структур из стека st_struct, они там оказались в результате работы функции check_id()
        from_st(st_struct, n1); 
        n2 = st_struct.top();       // одно имя достаем из стека, а второе читаем и оставляем для нескольких присваиваний
        // в векторе Struct_type_name находим нужные имена структур n1 и n2, и получаем соответствующие им типы t1 и t2
        for (int i = 0; i < Struct_type_name.size(); i++) {
            if (n1 == Struct_type_name[i].name) t1 = Struct_type_name[i].type;
            if (n2 == Struct_type_name[i].name) t2 = Struct_type_name[i].type;
        }
        //cout << endl << n1 << " " << n2 << " " << t1 << " " << t2 << endl << endl;
        // если типы не совпали - это ошибка
        if (t1 != t2) throw "Присваивание структур разных типов";
    }
    
}

void Parser::eq_bool () {       // проверка принадлежности к классу boolean
    //cout << "eq_bool" << endl;
    if (st_lex.top() != LEX_BOOL) throw "Выражение не является типом boolean";
    st_lex.pop();
}

// проверка была ли внесена информация об индентификаторе класса в Ident
void Parser::check_id_in_read () {      
    //cout << "check_id_in_read" << endl;
    if (TID[c_val].get_type() == LEX_STRUCT) throw "Нельзя читать структуры";
    if (!TID[c_val].get_declare()) throw "Идентификатор для read не объявлен";      // проверка идентификатора для оператора read
}


///// Executer & Interpretator /////

// функция, находящая размер структуры по имени структуры
// функция описана ниже 
int find_struct_size (string name);     // !!!!!

class Executer {
public:
    void execute ( vector<Lex> & poliz );
};

void Executer::execute ( vector<Lex> & poliz ) {
    cout << "Выполнение программы" << endl;
    Lex pc_el;          // текущая лексема ПОЛИЗа
    stack <int> args;       // стек аргументов, для исполнения программы
    stack <type_of_lex> argt;       // стек типов аргументов, нужен только для того, чтобы верно исполнять действия со строками
    int i, j, index = 0, size = poliz.size();   // переменные для операций, для текущего положения в ПОЛИЗе и размера ПОЛИЗа
    stack <string> structname;      // стек для запоминания имен структур для присваивания структур     !!!!!
    string x, y;        // аналогичные переменные для операций со строками
    type_of_lex buf3;   // буфер для использования типа лексемы, применяется для стека argt
    while (index < size) {      // цикл по всему ПОЛИЗу
        pc_el = poliz[index];       // берем текущую лексему
        switch (pc_el.get_type()) {     // исходя из типа полученной лексемы решаем что делать дальше
            // если эту лексему нужно положить в стек ПОЛИЗа, , т.е. если это адрес, или логическое значение, или число, или строка,
            // то кладем ее и ее тип в стек
            case LEX_TRUE: case LEX_FALSE: case LEX_NUM: case POLIZ_ADDRESS: case POLIZ_LABEL: case LEX_STR:           
                args.push(pc_el.get_value());       // кладем лексему в стек ПОЛИЗа
                argt.push(pc_el.get_type());        // кладем тип лексемы в стек ПОЛИЗа
                break;

            case LEX_ID:        // если это переменная, то
                i = pc_el.get_value();      // берем ее номер в таблице TID
                // !!!!!
                if (TID[i].get_type() == LEX_STRUCT) {      // проверка для структуры, нужно для присваивания
                    //cout << "Структура " << TID[i].get_name() << endl;
                    structname.push(TID[i].get_name());         // !!!!!
                    args.push(TID[i].get_value());      
                    argt.push(TID[i].get_type());
                }else if (TID[i].get_assign()) {      // проверяем, была ли переменная инициализирована (имеет ли она какое то значение)
                    args.push(TID[i].get_value());      // если да, то кладем ее значение и ее тип в стеки
                    argt.push(TID[i].get_type());
                    break;
                }
                else {
                    cout << pc_el;
                    throw "ПОЛИЗ: Неизвестный идентификатор";      // если нет, то это ошибка
                }
                // мы не можем выводить структуры потому что нигде не объявим что структуре что-то присвоено
                // TID[..].get_assign() всегда будет false для структуры, но из-за присваивания структур нужно отдельно проверить

            case LEX_NOT:       // если это операция not
                from_st(args, i);       // берем из стека верхний элемент
                args.push(!i);          // кладем обратно измененный элемент (взяли i, положили !i)
                break;

            case LEX_OR:        // случай операции or в ПОЛИЗе
                from_st(args, i);       // берем два верхних элемента со стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // и снимаем со стека типов одно значение
                                            // потому что результат положенный в стек будет иметь тот же тип
                args.push(j || i);          // кладем в ПОЛИЗ результат вычисления
                break;

            case LEX_AND:       // случай операции and в ПОЛИЗе
                from_st(args, i);           // берем два верхних элемента со стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // и снимаем со стека типов одно значение (pop)
                args.push(j && i);          // кладем в стек ПОЛИЗа результат вычисления
                break;

            case POLIZ_GO:      // в ПОЛИЗе мы встретили операцию безусловного перехода
                from_st(args, i);           // берем из стека ПОЛИЗа адрес перехода
                index = i - 1;              // и переходим в нужное место вектора-ПОЛИЗа
                from_st(argt, buf3);        // из стека типов забираем один элемент, так как адрес перехода из стека ПОЛИЗа был взят
                break;

            case POLIZ_FGO:     // в ПОЛИЗе мы встретили операцию условного перехода
                from_st(args, i);           // берем два верхних значения со стека ПОЛИЗа
                from_st(args, j);
                if (!j) index = i - 1;          // проверяем нужно ли нам менять адрес в векторе-ПОЛИЗе и меняем если надо
                from_st(argt, buf3);            // вместе с двумя знамениями из стека-ПОЛИЗа нужно снять два значения из стека типов
                from_st(argt, buf3);            // потому что его "наполнение" должно соответствовать стеку ПОЛИЗа
                break;

            case LEX_WRITE:     // случай операции write
                from_st(args, j);           // берем элемент из стека ПОЛИЗа
                from_st(argt, buf3);        // и убираем из стека типов его тип
                if (buf3 == LEX_BOOL) 
                    if (j) cout << "boolean: true" << endl; 
                    else cout << "boolean: false" << endl;
                else if (buf3 == LEX_STRUCT) {
                    cout << "Структуры выводить нельзя" << endl;            // !!!!!
                    // можно добавить вывод структур, с алгоритмом поиска полей как в присваивании
                    /*
                    // Возможность выводить структуры
                    string s;
                    from_st(structname, s);     // получили имя присваиваемой структуры
                    //cout << "Вывод структуры " << s << ":   "l;
                    int is;
                    for (int r = 0; r < TID.size(); r++)        // находим начало описания поля структур в TID
                        if (s == TID[r].get_name()) is = r;
                    is++;
                    int size = find_struct_size(s);
                    if (size == -1) throw "Неверные поля структуры (они пустые)";
                    for (int r = 0; r < size; r++) {            // пройдем по всем переменным - полям структуры и выведем нужные значения
                        if (!TID[is+r].get_assign()) throw "Одно из полей присваиваемой структуры не инициализировано";
                        cout << TID[is+r].get_name() << ":  ";
                        if (TID[is+r].get_type() == LEX_STR || TID[is+r].get_type() == LEX_STRING) 
                            cout << TStr[TID[is+r].get_value()];
                        if (TID[is+r].get_type() == LEX_BOOL)
                            if (TID[i].get_value()) cout << "boolean: true";
                            else cout << "boolean: false";
                        else cout << TID[is+r].get_value();
                        cout << "   ";
                    }
                    cout << endl;
                    */
                }
                else if (buf3 == LEX_STRING || buf3 == LEX_STR) cout << TStr[j] << endl;    
                        // если это строка - выводим соответствующую строку из таблицы строк
                else cout << j << endl;         // иначе выводим значение элемента из ПОЛИЗа
                break;
            
            case LEX_READ:      // случай операции read
                int k;
                from_st(args, i);           // берем верхний элемент со стека ПОЛИЗа, это переменная
                from_st(argt, buf3);        // и его тип 
                if (TID[i].get_type () == LEX_INT) {        // если это переменная типа int
                    cout << "Введите значение для " << TID[i].get_name() << endl;
                    if (!scanf("%d", &k)) throw "Введите число, а не строку"; 
                    //cin >> k;       // вводим получаем число с клавиатуры
                }else if (TID[i].get_type() == LEX_BOOL){       // если это переменная типа boolean
                    string j;
                    while (1) {         // то просим пользователя ввести true или false, пока не добьемся от него верного результата
                        cout << "Введите значение типа boolean (true or false) для " << TID[i].get_name() << endl;
                        cin >> j;
                        if (j != "true" && j != "false") {          // ничего кроме true и false не принимается
                            cout << "Ошибка ввода: true/false" << endl;
                            continue;
                        }
                        k = ( j == "true" ) ? 1 : 0;        // присваиваем k 1 или 0 в зависимости от того что введено с клавиатуры
                        break;
                    }
                }else{          // если это перемнная типа string
                    cout << "Введите значение типа string для " << TID[i].get_name() << endl;
                    string buf4;
                    cin >> buf4;      // читаем строку с клавиатуры
                    k = put_str(buf4);  // кладем ее в список строк, а k получает номер этой строки в списке строк
                }
                TID[i].put_value(k);        // записываем полученное значение k в поля идентификатора-переменной
                                            // это или число, или значения 1 и 0 для boolean или номер строки в TStr для строк
                TID[i].put_assign();        // указываем что переменная была инициализирована (имеет какое то значение, а не "мусор")
                break;

            case LEX_PLUS:      // случай операции +
                from_st(argt, buf3);        // снимаем один элемент со стека типов
                from_st(args, i);           // берем два верхних значения со стека ПОЛИЗа
                from_st(args, j);
                if (buf3 == LEX_STR || buf3 == LEX_STRING) {        // если это строки
                    string buf4;
                    buf4 = TStr[j] + TStr[i];           // то мы складываем строки
                    args.push(put_str(buf4));           // и кладем результат в таблицу TStr, а номер записи в таблице кладем в стек ПОЛИЗа
                }else args.push(i + j);         // если это числа, то складываем их и результат кладем в стек ПОЛИЗа
                break;

            case LEX_MULTIPLY:  // случай операции *
                from_st(args, i);       // берем из стека ПОЛИЗа два верхних элемента
                from_st(args, j);
                args.push(i * j);       // перемножаем их и кладем значение обратно в стек
                from_st(argt, buf3);    // так как в стеке на одно значение меньше, то один элемент из стека типов убираем
                break;

            case LEX_MINUS:     // случай операции -
                from_st(args, i);           // берем два верхних элемента из стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // убираем один элемент из стека типов, так как элементов в стеке ПОЛИЗа стало на 1 меньше
                                            // а тип разности совпадает с типом уменьшаемого
                args.push(j - i);           // вычитаем из нижнего элемента верхний и кладем результат в стек ПОЛИЗа
                break;

            case LEX_DIVIDE:    // случай операции /
                from_st(args, i);           // берем два верхних элемента из стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // убираем один элемент из стека типов (как и в случаях выше)
                if (i) args.push(j / i);    // если делитель не равен 0, то мы делим и результат записываем в стек ПОЛИЗа
                else throw "ПОЛИЗ: деление на 0";       // если делитель равен 0, то это ошибка
                break;

            case LEX_EQUAL:     // случай проверки на равенство
                from_st(args, i);           // берем два верхних элемента из стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // и два верхних элемента из стека типов
                from_st(argt, buf3);
                if (buf3 == LEX_STR || buf3 == LEX_STRING) args.push(TStr[j] == TStr[i]);  
                    // если это строки - то сравниваем соответствующие строки из TStr и в стек ПОЛИЗа кладем результат сравнения 1 или 0
                else args.push(i == j);     // если это не строки, то сравниваем числа и в стек ПОЛИЗа кладем результат сравнения чисел
                argt.push(LEX_BOOL);        // верхний элемент стека ПОЛИЗа имеет тип boolean, поэтому в стек типов кладем LEX_BOOL
                break;

            case LEX_LESS:      // случай сравнения <
                from_st(args, i);           // берем два верхних элемента из стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // и из стека типов убираем их типы
                from_st (argt, buf3);
                if (buf3 == LEX_STR || buf3 == LEX_STRING) args.push(TStr[j] < TStr[i]);  
                // если тип элементов string, то сравниваем соответствующие записи в TStr и кладем результат сравнения в стек ПОЛИЗа
                else args.push(j < i);      // если же это числа, то сравниваем их и кладем результат сравнения в стек ПОЛИЗа
                argt.push(LEX_BOOL);        // в стек типов кладем LEX_BOOL, потому что верхний элемент стека ПОЛИЗа имеет тип boolean
                break;

            case LEX_GREAT:     // случай сравнения >
                from_st(args, i);           // берем два верхних элемента из стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // и из стека типов убираем их типы
                from_st(argt, buf3);
                if (buf3 == LEX_STR || buf3 == LEX_STRING) args.push(TStr[j] > TStr[i]); 
                // если тип элементов string, то сравниваем соответствующие записи в TStr и кладем результат сравнения в стек ПОЛИЗа
                else args.push(j > i);      // если же это числа, то сравниваем их и кладем результат сравнения в стек ПОЛИЗа
                argt.push(LEX_BOOL);        // в стек типов кладем LEX_BOOL, потому что верхний элемент стека ПОЛИЗа имеет тип boolean
                break;
            
            case LEX_LESSOREQUAL:   // случай сравнения <=
                from_st(args, i);           // берем два верхних элемента из стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // и из стека типов убираем их типы
                from_st(argt, buf3);
                argt.push(LEX_BOOL);        // в стек типов кладем LEX_BOOL, потому что верхний элемент стека ПОЛИЗа имеет тип boolean
                args.push(j <= i);          // сравниваем числа и кладем результат сравнения в стек ПОЛИЗа
                break;
            
            case LEX_GREATOREQUAL:  // случай сравнения >=
                from_st(args, i);           // берем два верхних элемента из стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // и из стека типов убираем их типы
                from_st(argt, buf3);
                argt.push(LEX_BOOL);        // в стек типов кладем LEX_BOOL, потому что верхний элемент стека ПОЛИЗа имеет тип boolean
                args.push(j >= i);          // сравниваем числа и кладем результат сравнения в стек ПОЛИЗа
                break;

            case LEX_NOTEQUAL:      // случай сравнения !=
                from_st(args, i);           // берем два верхних элемента из стека ПОЛИЗа
                from_st(args, j);
                from_st(argt, buf3);        // и из стека типов убираем их типы
                from_st(argt, buf3);
                if (buf3 == LEX_STR || buf3 == LEX_STRING) args.push(TStr[j] != TStr[i]);  
                // если тип элементов string, то сравниваем соответствующие записи в TStr и кладем результат сравнения в стек ПОЛИЗа
                else args.push(j != i);     // если же это числа, то сравниваем их и кладем результат сравнения в стек ПОЛИЗа
                argt.push(LEX_BOOL);        // в стек типов кладем LEX_BOOL, потому что верхний элемент стека ПОЛИЗа имеет тип boolean
                break;

            case LEX_ASSIGN:        // случай операции присваивания
                type_of_lex buf4;
                from_st(args, i);       // берем из стека значение для присваивания
                from_st(args, j);       // и адрес куда будем присваивать
                from_st(argt, buf4);    // и убираем из стека их типы, запоминая тип присваиваемого значения (справа от =)
                from_st(argt, buf3);
                if (buf3 != LEX_STRUCT) {   // если присваиваются не структуры, то buf3 = POLIZ_ADDRESS
                    TID[j].put_value(i);    // записываем нужное значение i по адресу j
                    TID[j].put_assign();    // обозначаем что переменная теперь имеет какое-то значение, а не содержит в себе "мусор"
                    args.push(i);           // в стек ПОЛИЗа кладем присваиваемое значение (для нескольких операций присваивания например)
                    argt.push(buf4);        // и кладем в стек тип присваиваемого значения, соответствующий типу верхнего элемента стека ПОЛИЗа
                }       // !!!!!
                else{          // если же присваиваются структуры:
                    //cout << "Присваивание структур" << endl;
                    string s1, s2; 
                    from_st(structname, s2);    // получаем имена присваиваемых структур
                    s1 = structname.top();      // одно имя забираем из стека, а второе просто читаем и оставляем в стеке
                    //cout << s1 << " = " << s2 << endl;
                    int is1, is2;
                    for (int r = 0; r < TID.size(); r++) {      // находим начала описания полей структур в TID
                        if (s1 == TID[r].get_name()) is1 = r;
                        if (s2 == TID[r].get_name()) is2 = r;
                    }
                    // найдены имена структур в TID. Но TID сформирована так, что сразу после имени идут все поля структуры
                    // прибавив к is1 и is2 по по единице, мы получим номера первых полей соответствующих структур в TID
                    is1++; is2++;
                    //cout << is1 << " " << is2 << endl;
                    int size = find_struct_size(s1);    // находим размер структуры - количество ее полей
                    if (size == -1) throw "Неверные поля структуры (они пустые)";
                    for (int r = 0; r < size; r++) {        // пройдем по всем переменным - полям структуры и присвоим нужные значения
                        // если какое то из полей структуры, которую мы присваиваем не имеет значение, то это ошибка
                        if (!TID[is2+r].get_assign()) throw "Одно из полей присваиваемой структуры не инициализировано";
                        TID[is1+r].put_value(TID[is2+r].get_value());       // присваиваем полю структуры значение
                        TID[is1+r].put_assign();                            // указываем что поле теперь имеет значение
                    }
                    is2--;          // кладем переменную-структуру обратно в стек для нескольких присваиваний(как результат)!!!!!!!!!!
                    args.push(TID[is2].get_value());        // (кладем)                                                     !!!!!!!!!!
                    argt.push(buf4);            // и кладем тип - LEX_STRUCT                                                !!!!!!!!!!
                }
                break;

            default: throw "ПОЛИЗ: неожиданный элемент";        // реакция на иные лексемы
        }// конец switch
        ++index;            // переходим к следующему элементу ПОЛИЗа
    };// конец цикла while, значит мы прошли все элементы вектора-ПОЛИЗа
    cout << "Конец выполнения!!!" << endl;
}

// функция, находящая размер структуры по имени структуры 
int find_struct_size (string name) {        // !!!!!
    // находим в TID нужное имя структуры
    int i;
    for (int r = 0; r < TID.size(); r++) if (name == TID[r].get_name()) i = r;
    // и первое поле этой структуры
    i++;
    //cout << endl << i << endl;
    string s = TID[i].get_name();
    //cout << s << endl;
    int j = -1;
    // находим в имени поля точку, будем искать количество полей структуры ориентируясь по точке
    for (int r = 0; s[r] != '\0'; r++)
        if (s[r] == '.') { 
            j = r;
            break;
        }
    // если в имени первого поля структуры не оказалось точки, то это какая-то ошибка
    if (j == -1) {
        //cout << "Структура оказалась пустой" << endl;
        return -1;      // функция вернет -1 при ошибке
    }
    //cout << j << endl;
    int size = 0;       // размер структуры
    for (; i < TID.size(); i++) {      // проверяем таблицу TID, находя точку в нужном месте идентификаторв
        s = TID[i].get_name();
        int t = 0;
        while (s[t] != '\0') t++;       // на всякий случай находим размер имени переменной, чтобы не вылезти за пределы этого имени
                                        // например поле структуры sssssss.a, а следующая переменная в TID имеет имя b
                                        // в таком случае будут нарушены границы памяти, мы обходим эту ситуацию
        if (t < j) break;   
        if (s[j] == '.') size++;        // если точка в нужном месте нашлась, то увеличиваем размер
        else break;                     // если нет, то выходим из цикла
    }
    //cout << size << endl;
    return size;        // функция возвращает размер структуры
}

class Interpretator {
    Parser   pars;
    Executer E;
public:
    Interpretator (const char* program): pars(program) {}        
    // создаем элемент класса Parser для лексического, семантического и синтаксического анализа
    void interpretation ();
};

void Interpretator::interpretation () {     // функция, запускающая анализ кода
    pars.analyze();
    E.execute(pars.poliz);          // а затем исполнение полученного в результате анализа ПОЛИЗа
}

/*
верный текст программы моего модельного языка:
program
*описание структур*
*описание переменных*
{
*исполняемый код программы*
}
*/

int main(){   
    try{
        Interpretator I("file.txt");
        I.interpretation();
    }catch(char c){
        cout << "Встречен неизвестный символ " << c << endl;
    }catch(Lex l){
        cout << "Встречена неизвестная лексема " << l << endl;
    }catch(const char* str){
        cout << str << endl;
    }
    return 0;
}
