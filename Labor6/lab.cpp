#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <cmath>
#include <fstream>
#include <list>
#include <unordered_map>
using namespace std;
unordered_map<string,int> memoria;

//Consejo: haz commits pequeños: primero los tokens, luego el parser, luego la semántica (eval/execute). TODO:ass


/* E4. TODO: TOKEN POW
    se añadio a:
    - Class Token - enum Type TODO:POW SQRT
    - enum BinaryOp POW};//E4. TODO:
    - Class Parse Exp* parseFexp(); // E4. TODO: sqrt
    - Exp* Parser::parseFexp() { // E4. TODO: (asociatividad derecha).
    - Token* Scanner::nextToken() { TODO: 5.a POTENCIACION
    - Exp* Parser::parseFexp() { // E4. TODO: (asociatividad derecha).
    - Exp* Parser::parseTerm() { // Exp* left = parseFexp(); // E4. TODO: para el caso de que no sea una potencia
    - char Exp::binopToChar(BinaryOp op) { // E4. TODO: pow  ^
    - int BinaryExp::eval() { 
*/

/* E4. TODO: SQRT 
    Observaciones:
    - sqrt es un operador unario() no puede ser añadido a la lista de operadores binarios como: BinaryOp o BinaryExp
    - class SqrtExp : public Exp { //operador unario sqrt(...)
    - Token* Scanner::nextToken() { TODO: Reconocer palabra reservada sqrt 
    - Exp* Parser::parseFactor() {

*/

/* E5.  TODO: para += 
    Explicacion: 
        - (x += y como una sentencia, no como un operador dentro de una expresión.)
        - PLUS_EQ no debería evaluarse como “x + y”, sino como “x = x + y” y asignar de nuevo a x. 
        - Eso se implementa mejor como una nueva clase de sentencia, no como operador binario
    Cambio
        - Token* Scanner::nextToken() { TODO: 5.b PLUS_EQ

*/
    

/*E11. TODO: division por cero
    explicacion:
        Objetivo del “ejercicio 11”
        Cuando el usuario escribe algo que provoca una división entre 0 
        (por ejemplo z = x / y con y = 0), el intérprete debe:
            1.- Detectar que el divisor es 0 en tiempo de ejecución.
            2.- Mostrar el mensaje Error: división por cero (o el que prefieras).
            3.- Detener la ejecución del programa.
        La detección se hace en la rama DIV de BinaryExp::eval(), allí se realiza la operación.
    Cambio
        - case DIV:if()
*/

class Token { 
public:
    enum Type {PLUS, MINUS, MUL,DIV, NUM, ERR, PD, PI, END,PC,ID,PRINT,ASSIGN,POW,SQRT,PLUS_EQ }; // E4 TODO: POW SQRT
    // E5 TODO: PLUS_EQ
    Type type;
    string text;
    Token(Type);
    Token(Type, char c);
    Token(Type, const string& source, int first, int last);
};

class Scanner {
private:
    string input;
    int first, current;
public:
    Scanner(const char* in_s);
    Token* nextToken();
    ~Scanner();

};

///////El BinaryOp y la clase base Exp////////////////////////////////////////

enum BinaryOp { PLUS, MINUS,MUL,DIV,POW};//E4. TODO:
class Exp { //clase base abstracta
public:
    virtual void print() = 0;
    virtual int eval() = 0;
    virtual ~Exp() = 0;
    static char binopToChar(BinaryOp op);
};


/////////Subclases de Exp: BinaryExp, NumberExp, IdExp, SqrtExp ////////////////////

class BinaryExp : public Exp { // 2 + 3, a * b
public:
    Exp *left, *right;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    void print();
    int eval();
    ~BinaryExp();
};

class NumberExp : public Exp { // número literal (por ejemplo, 5).
public:
    int value;
    NumberExp(int v);
    void print();
    int eval();
    ~NumberExp();
};





                        ///////////////////////////////7
                        /// Nuevas clases

class IdExp : public Exp { //  representa una variable|identificador, como a
public:
    string id;
    IdExp(string v);
    void print();
    int eval();
    ~IdExp();
};

IdExp::IdExp(string v) {id = v;}
IdExp::~IdExp(){}
void IdExp::print() {}
int IdExp::eval(){
    return memoria[id];
}



class SqrtExp : public Exp { // clase que representa la funcion sqrt, ella es la que se encarga de evaluar la funcion
    public://operador unario sqrt(...) //E4. TODO: sqrt
        Exp* inside;
        SqrtExp(Exp* e): inside(e) {}
        ~SqrtExp() { delete inside; }
    
        void print() override {
            cout << "sqrt("; inside->print(); cout << ")";
        }
    
        int eval() override {
            return sqrt(inside->eval());  // o int(sqrt(...))
        }
    };

// ─── Sentencias ──────────────────────────────────────────────────────────
class Stm {
public:
    virtual void execute()=0;
    virtual ~Stm() = 0;
};

Stm::~Stm(){};

// ── 1. Asignación id = Exp ─────────────────────────────────────────────
class AssignStatement : public Stm {
private:
    string id;
    Exp* rhs;
public:
    void execute();
    AssignStatement(string id, Exp* e);
    ~AssignStatement();
};

// ── 2. Incremento acumulativo id += Exp
class IncrementStatement : public Stm { // E5. TODO: para +=
    string id;   
    Exp* rhs;
public:
    IncrementStatement(string id, Exp* e): id(id), rhs(e) {}
    ~IncrementStatement() { delete rhs; }
    void execute() override { 
        memoria[id] += rhs->eval(); 
    }
};

void AssignStatement::execute() {
    memoria[id] = rhs->eval();
}

AssignStatement::AssignStatement(std::string ide, Exp *e) {
    id = ide;
    rhs = e;
}

AssignStatement::~AssignStatement(){
    delete rhs;
}

// ── 3. print(Exp) ───────────────────────────────────────────────────────
class PrintStatement : public Stm {
private:
    Exp* e;
public:
    void execute();
    PrintStatement(Exp* e);
    ~PrintStatement();
};

void PrintStatement::execute() {
    cout << e->eval() << endl;
}

PrintStatement::PrintStatement(Exp *ee) {
    e=ee;
}

PrintStatement::~PrintStatement(){
    delete e;
}

class Program {
private:
    list<Stm *> slist;
public:
    Program();
    void add(Stm *s);
    void interprete();
    ~Program();
};

void Program::interprete() {
    for(auto it:slist){
        it->execute();
    }
}

Program::Program() {}
Program::~Program(){
    for(auto it = slist.begin();it!=slist.end();++it){
        delete *it;
    }
    slist.clear();
}

void Program::add(Stm *s) {
    slist.push_back(s);
}
////////////////////////////////








class Parser {
private:
    Scanner* scanner;
    Token *current, *previous;
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
    Exp* parseExpression();
    Exp* parseTerm();
    Exp* parseFexp(); // E4. TODO: sqrt
    Exp* parseFactor();
    Stm* parseStm();
    Program* parseProgram();
    bool tokenToOp(Token* tk, BinaryOp& op);
public:
    Parser(Scanner* scanner);
    Program* parse();
};

////////////////////////////////////////////////


Token::Token(Type type):type(type) { text = ""; }

Token::Token(Type type, char c):type(type) { text = c; }

Token::Token(Type type, const string& source, int first, int last):type(type) {
    text = source.substr(first,last);
}

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
    if (tok.text.empty())
        return outs << tok.type;
    else
        return outs << "TOK" << tok.type <<"(" << tok.text << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}

// SCANNER // 


Scanner::Scanner(const char* s):input(s),first(0), current(0) { }

Token* Scanner::nextToken() {
    Token* token;

/* 1.  Saltar espacios */
    while (input[current]==' ') current++;

/* 2.  Fin de la cadena */
    if (input[current] == '\0') return new Token(Token::END);


    char c  = input[current];
    first = current;

    /*3.  Numeros .................................*/
    if (isdigit(c)) {
        current++;
        while (isdigit(input[current]))
            current++;
        token = new Token(Token::NUM,input,first,current-first);
    }
    
    /*4. Identificadores y palabras reservadas ...*/
    else if (isalpha(c)) {
        current++;
        while (isalnum(input[current]))
            current++;
        if ("print"==input.substr(first,current-first)) {
            token = new Token(Token::PRINT,input,first,current-first);
        }
        else if("sqrt"==input.substr(first,current-first)){  // 4.a TODO: Reconocer palabra reservada sqrt
            token = new Token(Token::SQRT, input,first,current-first);
        }
        
        else {
            token = new Token(Token::ID,input,first,current-first);
        }
    }

    /*5. Operadores y signos de puntacion */
    else{ 
        /*---5.a POTENCIACION ** -----------------------------*/ // E4. TODO: 5.a POTENCIACION
        if ( c == '*' && input[current+1] == '*') { // <-- mirar 1 caracter adelante
            current +=2; // El current avanzar 2 '**'
            return new Token(Token::POW,input,first,current-first); // 2 = longitud de "**"
        // Token(Type, const string& src,int first,int n);// sub‑cadena del source constructores 
        }

        /*----5.b Operador += -----------------------------*/ // E5. TODO: 5.b PLUS_EQ
        if ( c == '+' && input[current+1] == '=') { // <-- mirar 1 caracter adelante
            current +=2; // El current avanzar 2 '+='
            return new Token(Token::PLUS_EQ,input,first,current-first); // 2 = longitud de "+="
        }

        if (strchr("+-*/();=", c)) {
            switch(c) {
                case '+': token = new Token(Token::PLUS,c); break;
                case '-': token = new Token(Token::MINUS,c); break;
                case '*': token = new Token(Token::MUL,c); break;
                case '/': token = new Token(Token::DIV,c); break;
                case '(': token = new Token(Token::PI,c); break;
                case ')': token = new Token(Token::PD,c); break;
                case ';': token = new Token(Token::PC,c); break;
                case '=': token = new Token(Token::ASSIGN,c); break;
                default: cout << "No deberia llegar aca" << endl;
            }
            current++;
        } else {
            token = new Token(Token::ERR, c);
            current++;
        }
    }
        return token;
}

Scanner::~Scanner() { }

// PARSER //

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp =current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR)) {
            cout << "Parse error, unrecognised character: " << current->text << endl;
            exit(0);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

Parser::Parser(Scanner* sc):scanner(sc) {
    previous = current = NULL;
    return;
};

Program* Parser::parse() {
    current = scanner->nextToken();
    if (check(Token::ERR)) {
        cout << "Error en scanner - caracter invalido" << endl;
        exit(0);
    }
    Program* p = parseProgram();
    if (current) delete current;
    return p;
}

Program* Parser::parseProgram() {
    Program* p = new Program();
    p->add(parseStm());
    while(match(Token::PC)){
        p->add(parseStm());
    }
    return p;
}

Stm* Parser::parseStm() {
    Exp* e;
    string id;
    if(match(Token::ID)){ // E5. TODO: id
        id  = previous->text;

        if (match(Token::ASSIGN)){ // id '=' Exp
            e = parseExpression();
            return new AssignStatement(id,e);
        }
        else if (match(Token::PLUS_EQ)){ // id '+=' Exp
            e = parseExpression();
            return new IncrementStatement(id,e);
        }
        else{
            cout << "Error: se esperaba un operador de asignacion" << endl;
            exit(0);
        }
        //match(Token::ASSIGN);
        //e = parseExpression();
        //return new AssignStatement(id,e);
    }
    else if(match(Token::PRINT)){
        match(Token::PI);
        e = parseExpression();
        match(Token::PD);
        return new PrintStatement(e);
    }
    else{
        cout<< "ERROR";
        exit(0);
    }
}

Exp* Parser::parseExpression() {

    Exp* left = parseTerm();

    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS;
        }
        else if (previous->type == Token::MINUS){
            op = MINUS;
        }
        Exp* right = parseTerm();
        left = new BinaryExp(left, right, op);
    }

    return left;
}

Exp* Parser::parseTerm() { // 
    Exp* left = parseFexp(); // E4. TODO: para el caso de que no sea una potencia-----------------------

    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL;
        }
        else if (previous->type == Token::DIV){
            op = DIV;
        }

        Exp* right = parseFexp(); //parseFexp para el caso de que sea una potencia ------------------
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseFexp() { //se encarga de parsear la potencia
    Exp* left = parseFactor(); //parseFactor para el caso de que no sea una potencia
    if (match(Token::POW)) { // E4. TODO: (asociatividad derecha).
        Exp* right = parseFexp(); // parseFexp para el caso de que sea una potencia
        return new BinaryExp(left, right, BinaryOp::POW);//Retorna una nueva expresion binaria porque es una potencia
    }
    return left; // Si no es una potencia, retorna la expresion que ya tiene

}

Exp* Parser::parseFactor() {
    Exp* e;
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::PI)){
        e = parseExpression();
        if (!match(Token::PD)){
            cout << "Falta parentesis derecho" << endl;
            exit(0);
        }
        return e;
    }
    
    else if (match(Token::SQRT)){ // E4. TODO: sqrt
        match(Token::PI); //para abrir el parentesis
        Exp* inside = parseExpression();
        if (!match(Token::PD)){
            cout << "Falta parentesis derecho" << endl;
            exit(0);
        }
        return new SqrtExp(inside); //retorna una nueva expresion binaria con el operador sqrt
    }
    
    else if (match(Token::ID)){
        return new IdExp(previous->text);
    }

    cout << "Error: se esperaba un número." << endl;
    exit(0);
}

char Exp::binopToChar(BinaryOp op) { 
    char  c=' ';
    switch(op) {
        case PLUS: c = '+'; break;
        case MINUS: c = '-'; break;
        case MUL: c = '*'; break;
        case DIV: c = '/'; break;
        case POW: c = '^'; break; // para la impresion de potencia// E4. TODO: pow  ^
        default: c = '$'; 
    }
    return c;
}

// AST //


BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {}
NumberExp::NumberExp(int v):value(v) {}

Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
NumberExp::~NumberExp() { }


void BinaryExp::print() {
    left->print();
    char c = binopToChar(this->op);;
    cout << ' ' << c << ' ';
    right->print();
}



void NumberExp::print() {
    cout << value;
}

int BinaryExp::eval() { 
    int result;
    int v1=left->eval();
    int v2=right->eval();
    switch(this->op) {
        case PLUS: result = v1+v2; break;
        case MINUS: result = v1-v2; break;
        case MUL: result = v1*v2; break;
        case DIV: //result = v1/v2; break;
            if (v2 == 0) { //E11. TODO: division por cero
                cout << "Error: división por cero" << endl;
                exit(1);
            }
            result = v1/v2; break;
        case POW: result = pow(v1,v2); break; // E4. TODO: podria ser necesario, aun no se
        default:
            cout << "Operador desconocido" << endl;
            result = 0;
    }
    return result;
}

int NumberExp::eval() {
    return value;
}


void test_scanner(Scanner * scanner) {
    Token* current;
    current = scanner->nextToken();
    while (current->type != Token::END) {
        if (current->type == Token::ERR) {
            cout << "Error en scanner - caracter invalido: " << current->text << endl;
            break;
        } else
            cout << current << endl;
        current = scanner->nextToken();
    }

    exit(1);

}




int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cout << "Incorrect number of arguments" << endl;
        exit(1);
    }
    ifstream infile(argv[1]);
    if (!infile) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        exit(1);
    }

    string input((istreambuf_iterator<char>(infile)),
                istreambuf_iterator<char>());

    infile.close();

    Scanner scanner(input.c_str());

    //test_scanner(&scanner);

    Parser parser(&scanner);

    Program *p = parser.parse();

    cout << "Ejecucion:" << endl;
    p->interprete();
    cout << endl;
    // cd Labor6
    // g++ lab.cpp -o lab6
    // ./lab6 inputs/input1.txt



    // ./lab6 inputs/input1.txt  --------------------------------------------------------------
        // input:
            //a=5; print(a)
        // output: 
            //Ejecucion:
            //5
            //

    // ./lab6 inputs/input2.txt   --------------------------------------------------------------
    // input:
        //a=2+3*4; print(a)
    // output:
        //Ejecucion:
        //14
        //

    // ./lab6 inputs/input3.txt  -----------------------------------------------------------------
    // input:
        //a=(2+3)*4; print(a)
    // output:
        //Ejecucion:
        //20
        //

    // ./lab6 inputs/input4.txt  --------------------------------------------------------------------
    // input:
        //a=2**3; b=sqrt(16); print(a+b)
    // output:
        //Ejecucion:
        //12
        //

    // ./lab6 inputs/input5.txt  --------------------------------------------------------------------
    // input:
        //x=1; y=2; x+=y; print(x); print(y)
    // output:
        //Ejecucion:
        //3
        //2
        //

    // ./lab6 inputs/input6.txt ---------------------------------------------------------------------- 
    // input:
        //x=2+3**2*2; print(x)
    // output:
        //Ejecucion:
        //20
        //

    // ./lab6 inputs/input7.txt ----------------------------------------------------------------------
    // input:
        //a=6; b=sqrt(a+5*2); print(b); print(a)
    // output:
        //Ejecucion:
        //4
        //6
        //

    // ./lab6 inputs/input8.txt ---------------------------------------------------------------------
    // input:
        //x=1; y=2; z=3; x+=y+z; y+=z*2; print(x); print(y); print(x+y)
    // output:
        //Ejecucion:
        //6
        //8
        //14
        //

    // ./lab6 inputs/input9.txt ----------------------------------------------------------
    // input:
        //x=2; y=3; z=(x+y)**2 + sqrt(49); print(z)
    // output:
        //Ejecucion:
        //32
        //

    // ./lab6 inputs/input10.txt ------------------------------------------------------------
    // input:
        //a=2; b=3; c=4; d=(a+b)*c + 5**2 - sqrt(81); a+=b*c; print(a); print(d); print((a+d)/2)
    // output:
        //Ejecucion:
        //14
        //36
        //25
        //
        
    // ./lab6 inputs/input11.txt
    // input:
        //x=5; y=0; z=x/y; print(z)
    // output:
        //Ejecucion:
        //Error: división por cero
        //

    delete p;
}