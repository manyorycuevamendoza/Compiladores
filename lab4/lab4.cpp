#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

class Token {
public:
    // Se añaden los tokens solicitados: ASSIGN, PRINT, TRUE, FALSE,
    // LT, GT, EQUAL, NOTEQUAL (total de 28 para este ejemplo)
    enum Type { 
        LPAREN=0, RPAREN, PLUS, MINUS, MULT, DIV, POW, 
        NUM, ERR, END, ID, SIN, COS, LOG, PI, E, SEMICOLON, 
        BIN, HEX, OCT,
        ASSIGN, PRINT, TTRUE, TFALSE,
        LT, GT, EQUAL, NOTEQUAL
    };

    static const char* token_names[28];
    
    Type type;
    string lexema;
    int line; // Aunque ya no usamos line en nextToken, lo dejamos para compatibilidad
    
    Token(Type);
    Token(Type, char c);
    Token(Type, const string source);
    Token(Type, int line);
    Token(Type, char c, int line);
    Token(Type, const string source, int line);
};

const char* Token::token_names[28] = { 
    "LPAREN", "RPAREN", "PLUS", "MINUS", "MULT", "DIV", "POW", 
    "NUM", "ERR", "END", "ID", "SIN", "COS", "LOG", "PI", "E", 
    "SEMICOLON", "BIN", "HEX", "OCT",
    "ASSIGN", "PRINT", "TRUE", "FALSE",
    "LT", "GT", "EQUAL", "NOTEQUAL"
};

Token::Token(Type type):type(type) {
    lexema = "";
    line = 0;
}
Token::Token(Type type, char c):type(type) {
    lexema = c;
    line = 0;
}
Token::Token(Type type, const string source):type(type) {
    lexema = source;
    line = 0;
}
Token::Token(Type type, int line):type(type), line(line) {
    lexema = "";
}
Token::Token(Type type, char c, int line):type(type), line(line) {
    lexema = c;
}
Token::Token(Type type, const string source, int line):type(type), line(line) {
    lexema = source;
}

std::ostream& operator << ( std::ostream& outs, const Token & tok ) {
    if (tok.lexema.empty())
        return outs << Token::token_names[tok.type];
    else
        return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}

class Scanner {
public:
    Scanner(const char* in_s);
    Token* nextToken();  // Se usará solo esta
    ~Scanner();
    
private:
    // Eliminamos nextTokenWithLine() y la tabla de tokens
    // (ya no se usa en main).
    
    string input;
    int first, current;
    int currentLine; // Se mantiene por compatibilidad, aunque no lo usemos mucho
    
    char nextChar();
    void rollBack();
    void startLexema();
    string getLexema();
    
    bool isNumberChar(char c);
    // Métodos auxiliares (scanNumber) para bin/oct/hex/decimal
    Token* scanNumber();
};

Scanner::Scanner(const char* s)
    : input(s), first(0), current(0), currentLine(1) { }

bool Scanner::isNumberChar(char c) {
    return isdigit(c) || c == '.';
}

char Scanner::nextChar() {
    int c = input[current];
    if (c != '\0') current++;
    return c;
}

void Scanner::rollBack() {
    if (current > 0)
        current--;
}

void Scanner::startLexema() {
    first = current;
}

string Scanner::getLexema() {
    return input.substr(first, current-first);
}

// ---------------------------------------------------------------------------
//    nextToken() (¡ÚNICO método que usamos en main!)
//    - Manejo de comentarios con #
//    - Soporte para =, ==, !=, <, >, print, True, False, etc.
//    - Binario, octal, hexadecimal, decimal
//    - Se devuelven los tokens correspondientes
// ---------------------------------------------------------------------------
Token* Scanner::nextToken() {
    char c;
    startLexema();
    
    while (true) {
        c = nextChar();
        
        // Ignorar espacios, tabulaciones, saltos de línea
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            startLexema();
            continue;
        }
        
        // Comentarios con #
        if (c == '#') {
            while (c != '\n' && c != '\0') {
                c = nextChar();
            }
            // Si c es '\n', continuar. Si es fin de archivo, devolvemos END
            if (c == '\0') {
                return new Token(Token::END);
            }
            startLexema();
            continue;
        }
        
        // Fin de archivo
        if (c == '\0') {
            // Si no hemos consumido nada, es END, de lo contrario ERR
            if (first == current) {
                return new Token(Token::END);
            } else {
                return new Token(Token::ERR, "Unexpected end of input");
            }
        }
        
        // 1) Operadores que pueden tener 1 o 2 caracteres: =, ==, !=
        if (c == '=') {
            char look = nextChar();
            if (look == '=') {
                return new Token(Token::EQUAL, "==");
            } else {
                rollBack();
                return new Token(Token::ASSIGN, "=");
            }
        }
        if (c == '!') {
            char look = nextChar();
            if (look == '=') {
                return new Token(Token::NOTEQUAL, "!=");
            } else {
                // Si hay un ! suelto no especificado, lo tratamos como error
                // o se puede devolver ERR
                rollBack();
                return new Token(Token::ERR, "!");
            }
        }
        
        // 2) Operadores <, >
        if (c == '<') {
            return new Token(Token::LT, "<");
        }
        if (c == '>') {
            return new Token(Token::GT, ">");
        }
        
        // 3) Símbolos sencillos
        if (c == '(') return new Token(Token::LPAREN, "(");
        if (c == ')') return new Token(Token::RPAREN, ")");
        if (c == '+') return new Token(Token::PLUS, "+");
        if (c == '-') return new Token(Token::MINUS, "-");
        if (c == '*') {
            // Verificar si hay '**'
            char look = nextChar();
            if (look == '*') {
                return new Token(Token::POW, "**");
            } else {
                rollBack();
                return new Token(Token::MULT, "*");
            }
        }
        if (c == '/') return new Token(Token::DIV, "/");
        if (c == ';') return new Token(Token::SEMICOLON, ";");
        
        // 4) Identificadores / Palabras reservadas
        //    - "print" → PRINT
        //    - "True"  → TRUE
        //    - "False" → FALSE
        //    - Caso contrario → ID
        if (isalpha(c)) {
            string id;
            id.push_back(c);
            // Consumir letras o dígitos (permitimos algo como a3)
            while ((c = nextChar()) && (isalnum(c))) {
                id.push_back(c);
            }
            // Retroceder el último caracter que no formó parte del ID
            rollBack();
            
            // Verificar si es alguna de las palabras clave
            if (id == "print")  return new Token(Token::PRINT,  "print");
            if (id == "True")   return new Token(Token::TTRUE,  "True");
            if (id == "False")  return new Token(Token::TFALSE, "False");
            
            // O alguna de las funciones trig/log conocidas
            if (id == "sin")    return new Token(Token::SIN, "sin");
            if (id == "cos")    return new Token(Token::COS, "cos");
            if (id == "log")    return new Token(Token::LOG, "log");
            if (id == "pi")     return new Token(Token::PI,  "pi");
            if (id == "e")      return new Token(Token::E,   "e");
            
            // Si no coincide con lo anterior, es ID normal
            return new Token(Token::ID, id);
        }
        
        // 5) Números (decimal, bin, oct, hex)
        if (isdigit(c)) {
            // Retroceder para usar la función auxiliar
            rollBack();
            return scanNumber();
        }
        
        // 6) Carácter no reconocido
        return new Token(Token::ERR, string(1, c));
    }
}

// ---------------------------------------------------------------------------
//   scanNumber() - para binario, octal, hexadecimal o decimal
// ---------------------------------------------------------------------------
Token* Scanner::scanNumber() {
    startLexema();
    char c = nextChar();
    
    // Si empieza con 0, ver si es 0b, 0x, 0o
    if (c == '0') {
        char p = nextChar();
        
        // BIN: 0b o 0B
        if (p == 'b' || p == 'B') {
            string binLex = "0";
            binLex.push_back(p);
            bool hasDigits = false;
            while ((c = nextChar()) && (c == '0' || c == '1')) {
                binLex.push_back(c);
                hasDigits = true;
            }
            rollBack();
            if (!hasDigits) {
                return new Token(Token::ERR, binLex);
            }
            return new Token(Token::BIN, binLex);
        }
        
        // HEX: 0x o 0X
        else if (p == 'x' || p == 'X') {
            string hexLex = "0";
            hexLex.push_back(p);
            bool hasHex = false;
            while ((c = nextChar()) && 
                   (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) 
            {
                hexLex.push_back(c);
                hasHex = true;
            }
            rollBack();
            if (!hasHex) {
                return new Token(Token::ERR, hexLex);
            }
            return new Token(Token::HEX, hexLex);
        }
        
        // OCT: 0o o 0O
        else if (p == 'o' || p == 'O') {
            string octLex = "0";
            octLex.push_back(p);
            bool hasOct = false;
            while ((c = nextChar()) && (c >= '0' && c <= '7')) {
                octLex.push_back(c);
                hasOct = true;
            }
            rollBack();
            if (!hasOct) {
                return new Token(Token::ERR, octLex);
            }
            return new Token(Token::OCT, octLex);
        }
        else {
            // Caso 0 algo => decimal normal
            rollBack();
            string decLex = "0";
            bool hasDecimal = false;
            while ((c = nextChar()) && (isdigit(c) || (c == '.' && !hasDecimal))) {
                if (c == '.') hasDecimal = true;
                decLex.push_back(c);
            }
            rollBack();
            return new Token(Token::NUM, decLex);
        }
    } 
    else {
        // No empieza con '0' => decimal normal
        string decLex(1, c);
        bool hasDecimal = false;
        while ((c = nextChar()) && (isdigit(c) || (c == '.' && !hasDecimal))) {
            if (c == '.') hasDecimal = true;
            decLex.push_back(c);
        }
        rollBack();
        return new Token(Token::NUM, decLex);
    }
}

Scanner::~Scanner() {
    // No tenemos vector de tokens interno (lo removimos),
    // así que no borramos nada aquí.
}

// ---------------------------------------------------------------------------
//   main() - Solo usa nextToken() hasta END (como pediste).
//   Eliminamos el uso de nextTokenWithLine() y la tabla de tokens.
// ---------------------------------------------------------------------------
int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <archivo>" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    if (!file) {
        cout << "Error al abrir el archivo " << argv[1] << endl;
        return 1;
    }

    // Leemos todo el archivo a un string
    string input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    // Validar que termine en ';' (opcional, según tu requerimiento anterior)
    if (input.empty() || input.back() != ';') {
        cout << "Error: El archivo debe terminar con un punto y coma (;)" << endl;
        return 1;
    }

    cout << "=== SALIDA SCANNER (nextToken) ===" << endl;
    Scanner scanner(input.c_str());
    Token* tk = scanner.nextToken();
    
    while (tk->type != Token::END) {
        cout << "next token " << tk << endl;
        delete tk;
        tk = scanner.nextToken();
    }
    
    cout << "last token " << tk << endl;
    delete tk;

    return 0;
}
