#include <iostream>

enum Day {sunday = 0, monday = 1, tuesday = 2, wednesday = 3,
                    thursday = 4, friday = 5, saturday = 6};

int main () {

// enumeraciones = un tipo de dato definido por el usuario que consta 
// de constantes enteras con nombre emparejadas.
// Excelente si tienes un conjunto de opciones potenciales.

    Day today = friday;

    switch(today){
        case sunday:    std::cout << "It is Sunday!\n";
                        break;
        case monday:    std::cout << "It is Monday!\n";
                        break;
        case tuesday:   std::cout << "It is Tuesday!\n";
                        break;
        case wednesday: std::cout << "It is Wednesday!\n";
                        break;
        case thursday:  std::cout << "It is Thursday!\n";
                        break;
        case friday:    std::cout << "It is Friday!\n";
                        break;
        case saturday:  std::cout << "It is Saturday!\n";
                        break;
    }

    return 0;
}