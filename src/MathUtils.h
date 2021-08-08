#pragma once
#include<cmath>
#include<vector>
#include<string>
#include<unordered_map>

typedef long double ld;
typedef ld(* FUNC_NP ) ();
typedef ld(* FUNC_1P ) (ld);
typedef ld(* FUNC_2P ) (ld , ld);

#define DEG_TO_RAD            M_PI / 180.0
#define RAD_TO_DEG            180.0 / M_PI

namespace MathUtils{

class Evalable{
public:
    int flags;
    virtual ld eval(){ //should never be called btw
        return 0.0;
    }

    virtual ~Evalable(){}
};

class _Term;
class _Expression;
class _VariableFactor;

//
//Evaluates a string expression
//i.e : 2 + 2 , 4/(1 + 2) , sin(x) / cos(y)
//supported functions :
//  sin , cos , tan , csc , sec , cot , and their inverses
//  sinh , cosh , tanh , csch , sech , coth , and their inverses
//  log , pow , exp , rand , time , sqrt , cbrt , reg , rad ,
//  round , int , sigma(n)
//
//supported operators :
// + - * / ^
//
//you can add custom fucntions by calling addFunction(name , func)
//this affects all the Expression created after adding the function
//
//supported constants are : PI , E
//you can add constants by calling createConstant(name , value)
//
//can create variables by calling 
//      Expression::setVariable(name , value)
//you can modefy them later by the same function or by using the [] operator
//i.e:
//      Expression exp("sin(x)");
//      exp.setVariable("x" , 0);
//      exp.eval() //returns 0.0
//      exp["x"] = PI/2.0;
//      exp.eval() //returns 1.0
//
// 
class Expression : public Evalable{
    
    friend _Term;
    friend _VariableFactor;

private:
    _Expression* mExp;
    std::unordered_map<std::string , ld> mVariables;

    int pos;
    const char* data;
    char ch;
    
    char* supchar(int start , int end);
    _Expression* _parseEpxression();
    _Term* _parseTerm();
    Evalable* _parseFactor();

    void _nextchar();
    bool _eat(char c);
    bool _can_eat(char c);

    Expression();
    
    
public:
    Expression(const char* exp);
    virtual ~Expression();

    //Evalutates the expression and resturns the value
    virtual ld eval();

    //add a value to a variable (if it doesnot exist , the variables gets created)
    void setVariable(const std::string& , ld);

    //returns refrunce to the variable or creates it if it does not exist
    ld& getVariable(const std::string& );
    
    //returns refrunce to the variable or creates it if it does not exist
    ld& operator[](const std::string& name);

};

class _Expression : public Evalable{
public:
    std::vector<Evalable*> mTerms;
    virtual ld eval();

    virtual ~_Expression();
};

class _Term : public Evalable{
public:
    std::vector<Evalable*> mFactors;
    virtual ld eval();
    virtual ~_Term();
};

//converts the passed value from rad to deg
ld deg(ld rads);
//converts the passed value from deg to rad
ld rad(ld degs);

//a class that dircribes an Expression error :)
class ExpressionError{
public:
    std::string error;
    int where;

    ExpressionError(std::string e , int w) : error(e) , where(w){
    }
};

//adds a function to the functions that can be executed from the expression
void addFunction1p(const std::string& name , FUNC_1P func);

//adds a function to the functions that can be executed from the expression
void addFunction2p(const std::string& name , FUNC_2P func);

//void addFunction(const std::string& name , FUNC_3P func);

//removes a function from the executable functions list
void removeFunction(const std::string& name , bool type_2 = false);

//creates a global constant just like PI and E
void createConstant(const std::string& name , ld value);

void removeConstant(const std::string& name);

//computes the limited integral of an expression
ld Integrate(Expression& c , ld from , ld to , const std::string& variable_name = "x" , ld dx = 1E-4);

}