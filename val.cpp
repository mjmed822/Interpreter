#include "val.h"

map<string, Value> Value::symbolTable;
queue<Value>* Value::ValQue;

Value Value::operator+(const Value& op) const {
    if( GetType() == op.GetType() ){
        if( IsInt() ) return Value( I + op.GetInt() );
        if( IsReal() ) return Value( R + op.GetReal() );
        if( IsStr()) throw "RUNTIME ERROR: Illegal String Type Operation";
        throw "RUNTIME ERROR: Illegal Mixed Type Operation";
    }
    else if(IsInt() && op.IsReal()) {
        return Value( (float) GetInt() + op.GetReal());
    }
    else if(IsReal() && op.IsInt()){
        return Value(GetReal() + (float) op.GetInt());
    } 
    throw "RUNTIME ERROR: Illegal Mixed Type Operation";
    return Value(VERR);
}

Value Value::operator-(const Value& op) const {
    if( GetType() == op.GetType() ){
        if( IsInt() ) return Value( I - op.GetInt() );
        if( IsReal() ) return Value( R - op.GetReal() );
        if( IsStr()) throw "RUNTIME ERROR: Illegal String Type Operation";
        throw "RUNTIME ERROR: Illegal Mixed Type Operation";
    }
    else if(IsInt() && op.IsReal()) {
        return Value( (float) GetInt() - op.GetReal());
    }
    else if(IsReal() && op.IsInt()){
        return Value(GetReal() - (float) op.GetInt());
    } 
    throw "RUNTIME ERROR: Illegal Mixed Type Operation";
    return Value(VERR);
}

Value Value::operator*(const Value& op) const {
    if( GetType() == op.GetType() ){
        if( IsInt() ) return Value( I * op.GetInt() );
        if( IsReal() ) return Value( R * op.GetReal() );
        if( IsStr()) throw "RUNTIME ERROR: Illegal String Type Operation";
        throw "RUNTIME ERROR: Illegal Mixed Type Operation";
    }
    else if(IsInt() && op.IsReal()) {
        return Value( (float) GetInt() * op.GetReal());
    }
    else if(IsReal() && op.IsInt()){
        return Value(GetReal() * (float) op.GetInt());
    } 
    throw "RUNTIME ERROR: Illegal Mixed Type Operation";
    return Value(VERR);
}

Value Value::operator/(const Value& op) const {
    if( GetType() == op.GetType() ){
        if( IsInt() ) return Value( I / op.GetInt() );
        if( IsReal() ) return Value( R / op.GetReal() );
        if( IsStr()) throw "RUNTIME ERROR: Illegal String Type Operation";
        throw "RUNTIME ERROR: Illegal Mixed Type Operation";
    }
    else if(IsInt() && op.IsReal()) {
        return Value( (float) GetInt() / op.GetReal());
    }
    else if(IsReal() && op.IsInt()){
        return Value(GetReal() / (float) op.GetInt());
    } 
    throw "RUNTIME ERROR: Illegal Mixed Type Operation";
    return Value(VERR);
}

