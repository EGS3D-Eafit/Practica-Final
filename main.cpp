#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <string>

#define TAO_PEGTL_NAMESPACE mylib::pegtl

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>

using namespace std;

namespace pegtl = TAO_PEGTL_NAMESPACE;

struct Node {
    Node* parent;
    vector<Node*> children;
    string value;

    Node(string s) {
        value = s;
    }
};

Node* root;

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
//una T con o sin mas expresiones de tipo + -, heco asi para evitar bucles tonotos
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
      : pegtl::nothing<Rule> {};

template<>
struct action<e_rule> {
    template< typename input >
    static void apply(const input & s) {
        cout<< s.string() << "E" << endl;
    }
};

template<>
struct action<t_rule> {

    template< typename input >
    static void apply(const input & s) {
        cout<< s.string() << "T" << endl;
    }
};

template<>
struct action<f_rule> {
    template< typename input >
    static void apply(const input & s) {
        cout<< s.string() << "F" << endl;
    }
};

struct grammar
         : pegtl::must<pegtl::plus<e_rule>, pegtl::eof> {};

int main()
{
    //cout<<decode("A")<<endl;
    string s = "(3+4)+(y/3)+10";
    pegtl::string_input input(s, "input")   ;
    pegtl::parse<grammar, action>(input);
    return 0;
}
