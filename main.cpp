#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <string>
#include "tao/pegtl.hpp"

using namespace std;

namespace pegtl = tao::pegtl;

// Reglas básicas mas basicas que la concha de la lora
// Ahora si enserio, estas son las reglas basicas de la gramatica libre de contexto
// Se usa una libreria llamada pegtl la cual nos dice que debemos usar estructuras las cuales sirven como reglas

struct identifier : pegtl::plus<pegtl::alnum> {}; // Identificadores
struct number : pegtl::plus<pegtl::digit> {}; // Números

// Operaciones
struct plus_op : pegtl::one<'+'> {};
struct minus_op : pegtl::one<'-'> {};
struct mul_op : pegtl::one<'*'> {};
struct div_op : pegtl::one<'/'> {};
struct open_paren : pegtl::one<'('> {};
struct close_paren : pegtl::one<')'> {};

//definiendo el E-> y los demas
struct e_rule;// template de una E
struct t_rule;// template de una T
struct f_rule;// template de una F

//aqui ya definiendolos porque necesitaba usar el T y F antes de ser definidos
struct e_rule : pegtl::seq<t_rule, pegtl::star<pegtl::seq<pegtl::sor<plus_op, minus_op>, t_rule>>> {};
//una T con o sin mas expresiones de tipo + -, no hecho de derivacion a izquierda por miedo a weonadas de bucles tonotos
struct t_rule : pegtl::seq<f_rule, pegtl::star<pegtl::seq<pegtl::sor<mul_op, div_op>, f_rule>>> {};
//misma situacion aqui pero con F
struct f_rule : pegtl::sor<pegtl::seq<open_paren, e_rule, close_paren>, identifier, number> {};

//funcion simple para borrar todos los caracteres c de un string
string remove_char(const string& str, char c) {
    string result = str;
    result.erase(remove(result.begin(), result.end(), c), result.end());
    return result;
}

template<typename Rule>
struct action
      : pegtl::nothing<Rule> {}; // accion sin reglas que no hace nada para casos de errores o demas


//accion que deberia de realizar el programa cuando compruebe que una parte de lo comprobado si cumple con la regla
//como estaba probando primero decidi hacerlo con couts y luego pasarlo a una estructura mas compleja como nodos o un vector
//para graficar comodamente en QT
template<>
struct action<e_rule> {
    static void apply(const string & s) {
        if(s.size() > 1) {
            string curDer = "E -> E ";
            string r = remove_char(s,' ');

            for(int i = 1; i < s.size(); i+=2) {
                curDer += s.at(i) + " T ";
            }

            cout << curDer << endl;
            return;
        }
        cout << "E -> T" << endl;
    }
};

template<>
struct action<t_rule> {
    static void apply(const string & s) {
        if(s.size() > 1) {
            string curDer = "T -> T ";
            string r = remove_char(s,' ');

            for(int i = 1; i < s.size(); i+=2) {
                curDer += s.at(i) + " F ";
            }

            cout << curDer << endl;
            return;
        }
        cout << "T -> F" << endl;
    }
};

template<>
struct action<f_rule> {
    static void apply(const string & s) {
        if(s.at(0) == '(') {
            cout << "F -> (E)" << endl;
            return;
        }
        cout << "F -> "<< s << endl;
    }
};

//una prueba que hice para ver si funcionaba pero no cambio nada
struct grammar
         : pegtl::must<f_rule, pegtl::eof > {};

int main()
{
    //cout<<decode("A")<<endl;
    string s = "4+2";
    pegtl::string_input input(s, "input");
    pegtl::parse<grammar, action>(input);
    return 0;
}
