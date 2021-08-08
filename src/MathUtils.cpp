#include "MathUtils.h"
#include <iostream>
#include <chrono>

#define FLAG_PLUS     0
#define FLAG_MINUS    1
#define FLAG_MUL      2
#define FLAG_DEV      3

namespace MathUtils{

static std::unordered_map<std::string , FUNC_1P> func_map_1p;
static std::unordered_map<std::string , FUNC_2P> func_map_2p;

static std::unordered_map<std::string , ld>      constants;

void addFunction1p(const std::string& name , FUNC_1P func){
    std::pair<std::string , FUNC_1P> p = {name , func};
    func_map_1p.insert(p);
}
void addFunction2p(const std::string& name , FUNC_2P func){
    std::pair<std::string , FUNC_2P> p = {name , func};
    func_map_2p.insert(p);
}

void removeFunction(const std::string& name , bool type_2){
    if (type_2){
        func_map_2p.erase(name);
    }else{
        func_map_1p.erase(name);
    }
}

void createConstant(const std::string& name , ld value){
    std::pair<std::string , ld> p = {name , value};
    constants.insert(p);
}

void removeConstant(const std::string& name){
    constants.erase(name);
}

ld Integrate(Expression& c , ld from , ld to , const std::string& variable_name , ld dx){
    c.setVariable(variable_name , 0.0);
    int sign = 1;
    if (from > to){
        sign = -1;
        to = from + to;
        from = to - from;
        to = to - from; //switch the varaibles value
    }

    if (dx <= 0){ //will never end
        return -1;
    }

    ld result = 0;

    while(from < to){
        c[variable_name] = from;
        result += c.eval() * dx;
        from += dx;
    }

    return result * sign;
}

ld deg(ld v){
    return v * RAD_TO_DEG;
}

ld rad(ld v){
    return v * DEG_TO_RAD;
}

static ld randld(){
    return ((ld) rand()) / RAND_MAX;
}

static ld randld(ld seed){ //seed should be a long type .. but I need it to be ld
    unsigned int s = (unsigned int) seed;
    srand(s);
    return ((ld) rand()) / RAND_MAX;
}

static ld m_log_2(ld v1 , ld v2){
    return logl(v1) / logl(v2);
}

static ld m_int(ld l){
    return (ld) ((int) l);
}

static ld time(){
    auto t = std::chrono::high_resolution_clock::now();
    auto mil = std::chrono::time_point_cast<std::chrono::milliseconds>(t);
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(mil.time_since_epoch());
    return (ld) dur.count();
}

char* Expression::supchar(int start , int len){
    char* out = new char[len + 1];
    for (int i = 0; i < len; ++i) {
        out[i] = data[i + start];
    }

    out[len] = '\0';

    return out;
}

static bool charequl(const char* c1 , const char* c2){
    int i = 0;
    for (;;){
        if (c1[i] != c2[i])
            return false;
        
        if (c1[i] == '\0' && c2[i] == '\0')
            return true;

        i++;
    }
}

class _Constant : public Evalable{
public:
    _Constant(ld v){
        val = v;
    }
    ld val;

    virtual ld eval(){
        return val;
    }
};

class _NegativeFactor : public Evalable{
public:
    Evalable* exp;
    
    virtual ld eval(){
        return -(exp->eval());
    }

    virtual ~_NegativeFactor(){
        delete exp;
    }
};

class _FlippedFactor : public Evalable{
public:
    Evalable* exp;

    _FlippedFactor(Evalable* e) {
        exp = e;
    }
    
    virtual ld eval(){
        return 1.0 /(exp->eval());
    }

    virtual ~_FlippedFactor(){
        delete exp;
    }
};

class _FuncNP : public Evalable{
public:
    FUNC_NP execute;
    virtual ld eval(){
        return execute();
    }
};

class _Func1P : public Evalable{
public:
    FUNC_1P execute;
    Evalable* pass1;
    virtual ld eval(){
        return execute((*pass1).eval());
    }

    virtual ~_Func1P(){
        //std::cout << "func_delete\n";
        delete pass1;
    }
};

class _Func2P : public Evalable{
public:
    FUNC_2P execute;
    Evalable* pass1;
    Evalable* pass2;
    virtual ld eval(){
        return execute((*pass1).eval() , (*pass2).eval());
    }

    virtual ~_Func2P(){
        delete pass1;
        delete pass2;
    }
};

class _VariableFactor : public Evalable{
public:
    _VariableFactor(const char* n , Expression* e){
        name = n;
        exp = e;
    }
    std::string name;
    Expression* exp;
    virtual ld eval(){
        auto i = exp->mVariables.find(name);
        if (i == exp->mVariables.end()){
            std::string s = "Unknown varible '" + name + "'";
            throw ExpressionError(s , -1);
        }
        return exp->getVariable(name);
    }
};

class _PowOperator : public Evalable{
public:
    Evalable *base , *power;

    virtual ld eval(){
        ld b = base->eval();
        ld p = power->eval();

        return powl(b , p);
    }

    virtual ~_PowOperator(){
        delete base;
        delete power;
    }
};

class _SigmaFactor : public Evalable{
public:
    Evalable *exp , *start , *end;
    Expression *caller;

    virtual ld eval(){
        int s = (int) start->eval();
        int e = (int) end->eval();

        ld result = 0;
        if (s > e){
            s = e + s;
            e = s - e;
            s = s - e;
        }

        caller->setVariable("n" , s);

        while(s <= e){
            caller->operator[]("n") = s;
            result += exp->eval();
            s++;
        }

        return result;
    }

    virtual ~_SigmaFactor(){
        delete exp;
        delete start;
        delete end;
    }
};


bool Expression::_eat(char c){
    while (ch == ' ') _nextchar();
    if (c == ch){
        _nextchar();
        return true;
    }
    return false;
}

bool Expression::_can_eat(char c){
    while (ch == ' ') _nextchar();
    if (c == ch){
        return true;
    }
    return false;
}

void Expression::_nextchar(){
    pos++;
    ch = data[pos];
    if (ch == '\0')
        ch = -1;
}

_Expression* Expression::_parseEpxression(){
    _Expression *exp = new _Expression();
    _Term* temp = _parseTerm();
    exp->mTerms.emplace_back(temp);

    //std::cout << "Added a term >> " << temp->eval() << std::endl;
    
    for (;;){
        if (_eat('+')){
            _Term* t = _parseTerm();
            if (t == nullptr){
                std::cout << "t is null\n";
                return exp;
            }
            t->flags = FLAG_PLUS;
            exp->mTerms.emplace_back(t);

            //std::cout << "Added a term >> " << t->eval() << std::endl;
            
            continue;
        }else if (_eat('-')){
            _Term* t = _parseTerm();
            if (t == nullptr){
                std::cout << "t is null\n";
                return exp;
            }
            t->flags = FLAG_MINUS;
            exp->mTerms.emplace_back(t);

            //std::cout << "Added a term >> " << t->eval() << std::endl;
            
            continue;
        }
        return exp;
    }
}

_Term* Expression::_parseTerm(){
    _Term* t = new _Term();
    Evalable* e = _parseFactor();
    t->mFactors.emplace_back(e);

    //std::cout << "Added a factor >> " << e->eval() << std::endl;
    
    for (;;){
        if (_eat('*')){
            Evalable* v = _parseFactor();
            if (v == nullptr){
                std::cout << "v is null\n";
                return t;
            }

            v->flags = FLAG_MUL;
            t->mFactors.emplace_back(v);
            
            //std::cout << "Added a factor >> " << v->eval() << std::endl;

            continue;
        }else if (_eat('/')){
            Evalable* v = _parseFactor();
            if (v == nullptr){
                std::cout << "v is null\n";
                return t;
            }

            v->flags = FLAG_DEV;
            t->mFactors.emplace_back(v);

            //std::cout << "Added a factor >> " << v->eval() << std::endl;

            continue;
        }

        return t;
    }
}

Evalable* Expression::_parseFactor(){
    if (_eat('+')) return _parseFactor();
    
    if (_eat('-')) {
        _NegativeFactor* fact = new _NegativeFactor();
        fact->exp = _parseFactor();
        return fact;
    }

    if (ch == '\0'){ //shouldnt be called anyway
        _Constant* t = new _Constant(0.0);
        return t;
    }

    int startPos = pos;

    Evalable* ret = nullptr;

    if (_eat('(')) { // parentheses
        ret = _parseEpxression();
        _eat(')');
    } else if ((ch >= '0' && ch <= '9') || ch == '.') { // numbers
        while ((ch >= '0' && ch <= '9') || ch == '.' || ch == 'E') _nextchar();
        char* num = supchar(startPos , pos - startPos);
        ld x = strtold(num , nullptr);
        _Constant* t = new _Constant(x);
        delete[] num;

        ret = t;

    } else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_')) { // functions & variables
        while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_') || (ch >= '0' && ch <= '9')) _nextchar();
        char* func = supchar(startPos , pos - startPos);
        
        if (_eat('(')){ //functions
            if (charequl(func , "sqrt")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = sqrtl;
                ret = f;
            }else if (charequl(func , "cbrt")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = cbrtl;
                ret = f;
            }else if (charequl(func , "deg")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = deg;
                ret = f;
            }else if (charequl(func , "rad")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = rad;
                ret = f;
            }
            
            //START Trangmetric Functions
            //sin , cos , tan , csc , sec , cot , and their inverses
            else if (charequl(func , "sin")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = sinl;
                ret = f;
            }else if (charequl(func , "cos")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = cosl;
                ret = f;
            }else if (charequl(func , "tan")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = tanl;
                ret = f;
            }else if (charequl(func , "csc")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = sinl;
                ret = new _FlippedFactor(f);
            }else if (charequl(func , "sec")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = cosl;
                ret = new _FlippedFactor(f);
            }else if (charequl(func , "cot")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = tanl;
                ret = new _FlippedFactor(f);
            }else if (charequl(func , "asin")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = asinl;
                ret = f;
            }else if (charequl(func , "acos")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = acosl;
                ret = f;
            }else if (charequl(func , "atan")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = atanl;
                ret = f;
            }else if (charequl(func , "acsc")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = new _FlippedFactor(v);
                f->execute = asinl;
                ret = f;
            }else if (charequl(func , "asec")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = new _FlippedFactor(v);
                f->execute = acosl;
                ret = f;
            }else if (charequl(func , "acot")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = new _FlippedFactor(v);
                f->execute = atanl;
                ret = f;
            } //DONE Trangmetric Functions
            
            //START Hyperbolic Functions 
            //sinh , cosh , tanh , csch , sech , coth , and their inverses
            else if (charequl(func , "sinh")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = sinhl;
                ret = f;
            }else if (charequl(func , "cosh")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = coshl;
                ret = f;
            }else if (charequl(func , "tanh")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = tanhl;
                ret = f;
            }else if (charequl(func , "csch")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = sinhl;
                ret = new _FlippedFactor(f);
            }else if (charequl(func , "sech")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = coshl;
                ret = new _FlippedFactor(f);
            }else if (charequl(func , "coth")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = tanhl;
                ret = new _FlippedFactor(f);
            }else if (charequl(func , "asinh")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = asinhl;
                ret = f;
            }else if (charequl(func , "acosh")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = acoshl;
                ret = f;
            }else if (charequl(func , "atanh")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = v;
                f->execute = atanhl;
                ret = f;
            }else if (charequl(func , "acsch")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = new _FlippedFactor(v);
                f->execute = asinhl;
                ret = new _FlippedFactor(f);
            }else if (charequl(func , "sech")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = new _FlippedFactor(v);
                f->execute = acoshl;
                ret = new _FlippedFactor(f);
            }else if (charequl(func , "coth")){
                Evalable* v = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->pass1 = new _FlippedFactor(v);
                f->execute = atanhl;
                ret = new _FlippedFactor(f);
            } //DONE Hyperbolic Functions 
            
            else if (charequl(func , "rand")){
                if (_can_eat(')')){
                    _FuncNP* f = new _FuncNP();
                    f->execute = randld;
                    ret = f;
                }else{
                    Evalable* v = _parseEpxression();
                    _Func1P* f = new _Func1P();
                    f->pass1 = v;
                    f->execute = randld;
                    ret = f;
                }
            }else if (charequl(func , "time")){
                _FuncNP* f = new _FuncNP();
                f->execute = time;
                ret = f;
            }else if (charequl(func , "log")){
                Evalable* v0 = _parseEpxression();
                if (_eat(',')){
                    Evalable* v1 = _parseEpxression();
                    _Func2P* f = new _Func2P();
                    f->execute = m_log_2;
                    f->pass1 = v0;
                    f->pass2 = v1;
                    ret = f;
                }else{
                    _Func1P* f = new _Func1P();
                    f->execute = logl;
                    f->pass1 = v0;
                    ret = f;
                }
            }else if (charequl(func , "pow")){
                Evalable* v0 = _parseEpxression();
                if (!_eat(',')){
                    throw ExpressionError("pow(a , b) must have 2 paramiters , sugessted : exp(a) " , startPos);
                }
                Evalable* v1 = _parseEpxression();
                _Func2P* f = new _Func2P();
                f->execute = powl;
                f->pass1 = v0;
                f->pass2 = v1;
                ret = f;
            }else if (charequl(func , "exp")){
                Evalable* v0 = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->execute = expl;
                f->pass1 = v0;
                ret = f;    
            }else if (charequl(func , "int")){
                Evalable* v0 = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->execute = m_int;
                f->pass1 = v0;
                ret = f;
                   
            }else if (charequl(func , "round")){
                Evalable* v0 = _parseEpxression();
                _Func1P* f = new _Func1P();
                f->execute = roundl;
                f->pass1 = v0;
                ret = f;
                   
            }else if (charequl(func , "sigma")){
                Evalable* exp = _parseEpxression();
                if (!_eat(','))
                    throw ExpressionError("sigma(exp , start , end) must have 3 paramiters" , startPos);
                Evalable* start = _parseEpxression();
                if (!_eat(','))
                    throw ExpressionError("sigma(exp , start , end) must have 3 paramiters" , startPos);
                Evalable* end = _parseEpxression();

                _SigmaFactor* s = new _SigmaFactor();
                s->caller = this;
                s->exp = exp;
                s->start = start;
                s->end = end;

                ret = s;
                
            }else{
                Evalable* exp1 = _parseEpxression();
                if (_eat(',')){
                    Evalable* exp2 = _parseEpxression();
                    auto p = func_map_2p.find(func);
                    if (p != func_map_2p.end()){
                        _Func2P* f = new _Func2P();
                        f->execute = p->second;
                        f->pass1 = exp1;
                        f->pass2 = exp2;
                        ret = f;
                    }
                }else{
                    auto p = func_map_1p.find(func);
                    if (p != func_map_1p.end()){
                        _Func1P* f = new _Func1P();
                        f->execute = p->second;
                        f->pass1 = exp1;
                        ret = f;
                    } 
                }
            }
            
            
            if (ret == nullptr){ //function was not found
                std::string str = "function (";
                str += func;
                str += ") was not found";
                throw ExpressionError(str , startPos);
            }
        _eat(')');

        }else{ //constants & variables
            
            //constants
            auto p = constants.find(func);
            if (p != constants.end()){
                ret = new _Constant(p->second);
            }else if (charequl(func , "PI") || charequl(func , "pi")){
                ret = new _Constant(M_PI);
            }else if (charequl(func , "E") || charequl(func , "e")){
                ret = new _Constant(M_E);
            }else{
                //impl variables
                ret = new _VariableFactor(func , this);
            }
        }

        delete[] func;
    }

    if (_eat('^')){
        Evalable* power = _parseFactor();
        _PowOperator* pp = new _PowOperator();
        pp->base = ret;
        pp->power = power;
        return pp;
    }

    return ret;
}
 

Expression::Expression(const char* func){
    data = func;
    pos = -1;
    _nextchar();
    mExp = _parseEpxression();
}

void Expression::setVariable(const std::string& name , ld value){
    std::pair<std::string , ld> p;
    p.first = name;
    p.second = value;

    mVariables.insert(p);
}

ld& Expression::getVariable(const std::string& name){
    return mVariables[name];
}

ld& Expression::operator[](const std::string& name){
    auto p = mVariables.find(name);
    if (p == mVariables.end()){
        std::pair<std::string , ld> pa = {name , 0.0};
        mVariables.insert(pa);
        p = mVariables.find(name);
    }

    return p->second;
}

ld _Expression::eval(){
    ld val;
    for (int i = 0;i < mTerms.size();i++){
        //ld temp = mTerms[i]->eval();
        //std::cout << "term[" << i << "]=" << temp << std::endl;
        if (i == 0){
            val = mTerms[0]->eval();
            continue;
        }

        Evalable* ptr = mTerms[i];
        if (ptr->flags == FLAG_PLUS){
            val += ptr->eval();
        }else{
            val -= ptr->eval();
        }
    }

    return val;
}

ld _Term::eval(){
    ld val;
    for (int i = 0;i < mFactors.size();i++){
        //std::cout << "factor[" << i << "]=" << mFactors[i]->eval() << std::endl;
        if (i == 0){
            val = mFactors[0]->eval();
            continue;
        }

        Evalable* ptr = mFactors[i];
        if (ptr->flags == FLAG_MUL){
            val = val * ptr->eval();
        }else{
            val = val / ptr->eval();
        }
    }

    return val;
}

ld Expression::eval(){
    if (mExp == nullptr)
        return -1;

    return mExp->eval();
}

Expression::~Expression(){
    delete mExp;
    mExp = nullptr;
}

_Expression::~_Expression(){
    //std::cout << "exp_delete\n";
    for (Evalable* t : mTerms){
        delete t;
    }
}

_Term::~_Term(){
    //std::cout << "term_delete\n";
    for (Evalable* v : mFactors){
        delete v;
    }
}


}//Namespace