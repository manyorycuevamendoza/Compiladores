#include <iostream>
#include <string>

class Token {
public:
    // 1. Definición de enum Type dentro de la clase
    enum Type {
        PLUS,   // Para representar el símbolo '+'
        MINUS,  // Para representar el símbolo '-'
        NUM,    // Para representar un número entero o decimal
        END,    // Para indicar el fin de la expresión
        ERR     // Para representar un token no reconocido
    };

    // 2. Datos que componen un Token
    Type type;       // Tipo del token (uno de los valores del enum)
    std::string text;// Texto asociado al token (por ejemplo, "123", "+", etc.)

    // 3. Constructores
    // Constructor que recibe solo el tipo
    Token(Type t) : type(t), text("") {}

    // Constructor para un token simple (un solo caracter)
    Token(Type t, char c) : type(t) {
        text.push_back(c);  // Se agrega el char a la cadena
    }

    // Constructor para un token con subcadena (p.ej., un número con varios dígitos)
    Token(Type t, const std::string& source, int start, int end) : type(t) {
        text = source.substr(start, end - start + 1);
    }
};

// 4. Operador de inserción (para poder imprimir el token cómodamente)
std::ostream& operator<<(std::ostream& os, const Token& tok) {
    // Si el texto está vacío, imprimimos solo el tipo (numérico o con otra lógica)
    if (tok.text.empty()) {
        return os << "TYPE=" << tok.type;
    }
    // Si hay texto, imprimimos "TOK(<texto>)"
    else {
        return os << "TOK(" << tok.text << ")";
    }
}

// 5. Ejemplo de uso
int main() {
    // Creamos varios tokens de ejemplo
    Token t1(Token::PLUS, '+');                    // Token de tipo PLUS con texto "+"
    Token t2(Token::NUM, "123+45", 0, 2);          // Extraer "123" de la cadena "123+45"
    Token t3(Token::ERR);                          // Token de tipo ERR
    Token t4(Token::END);                          // Token para fin de expresión

    // Mostramos los tokens en pantalla
    std::cout << t1 << std::endl;  // TOK(+)
    std::cout << t2 << std::endl;  // TOK(123)
    std::cout << t3 << std::endl;  // TYPE=4  (porque ERR podría ser 4 si se definió en ese orden)
    std::cout << t4 << std::endl;  // TYPE=3  (END podría ser 3)

    return 0;
}
