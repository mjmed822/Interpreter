#include "parseRun.h"
#include <string>

using namespace std;
using namespace Parser;

/*
 * Prog := begin StmtList end
 * StmtList := Stmt; { Stmt;}
 * Stmt := PrintStmt | AssignStmt | IfStmt
 * PrintStmt := print ExprList
 * IfStmt := if (Expr) then Stmt
 * AssignStmt := Var = Expr
 * ExprList := Expr {, Expr}
 * Expr := Term {(+|-) Term}
 * Term := Factor {(*|/) Factor}
 * Var := ident
 * Factor := ident | iconst | rconst | sconst | (Expr)
*/

bool Prog(istream& in, int& line) {
    LexItem tok = GetNextToken(in, line);
    if (tok.GetToken() != BEGIN) {
        ParseError(line, "Missing begin");
        return false;
    }

    if (!StmtList(in, line)) {
        ParseError(line, "No statements in program");
        return false;
    }

    return true;
}

bool StmtList(istream& in, int& line) {

    if (!Stmt(in, line)) {
        ParseError(line, "Invalid statement");
        return false;
    }

    while(true) { 
        LexItem tok = GetNextToken(in, line);
        if (tok.GetToken() == END) {
            return true;
        }

        PushBackToken(tok);
        if (!Stmt(in, line)) {
            return false;
        }
    }
    return true;
}

bool Stmt(istream& in, int& line) {
    if (PrintStmt(in, line)) {
        LexItem tok_scoma = GetNextToken(in, line);
        if (tok_scoma.GetToken() != SCOMA) {
            ParseError(line, "Missing ; in print statement");
            return false;
        }

        //Execute the statement after making sure 
        //the semicolon is seen.
        while (!(*Value::ValQue).empty()) {
            Value nextVal = (*Value::ValQue).front();
            cout << nextVal;
            Value::ValQue->pop();
        }
        cout << endl;

        return true;
    } else if (IfStmt(in, line)) {
        return true;
    } else if (AssignStmt(in, line)) {
        LexItem tok_scoma = GetNextToken(in, line);
        if (tok_scoma.GetToken() != SCOMA) {
            ParseError(line, "Missing ; in assign statement");
            return false;
        }
        return true;
    }

    return false;
}

// PrintStmt := print ExprList
bool PrintStmt(istream& in, int& line) {
    Value::ValQue = new queue<Value>();
    LexItem tok = GetNextToken(in, line);
    if (tok.GetToken() != PRINT) {
        PushBackToken(tok);
        return false;
    }

    if (!ExprList(in, line)) {
        ParseError(line, "Invalid print statement");
        //Empty the queue and delete.
        while (!(*Value::ValQue).empty()) {
            Value::ValQue->pop();
        }
        delete Value::ValQue;
        Value::ValQue = nullptr;
        return false;
    }
    return true;
}

// IfStmt := if (Expr) then Stmt
bool IfStmt(istream& in, int& line) {
    LexItem tok_if = GetNextToken(in, line);
    if (tok_if.GetToken() != IF) {
        PushBackToken(tok_if);
        return false;
    }
    LexItem tok_lparen = GetNextToken(in, line);
    if (tok_lparen.GetToken() != LPAREN) {
        ParseError(line, "Missing left parenthesis");
        return false;
    }
    Value val;
    if (!Expr(in, line, val)) {
        ParseError(line, "Missing If statement expression");
        return false;
    }

    if(!val.IsInt()){
        ParseError(line, "RUNTIME ERROR: Illegal Type for If statement Expression");
        return false;
    }

    LexItem tok_rparen = GetNextToken(in, line);
    if (tok_rparen.GetToken() != RPAREN) {
        ParseError(line, "Missing right parenthesis");
        return false;
    }

    LexItem tok_then = GetNextToken(in, line);
    if (tok_then.GetToken() != THEN) {
        ParseError(line, "Missing THEN");
        return false;
    }

    // semantic of if statement
    if (val.GetInt() == 0) {
        // skip the if statement
        LexItem tok_scoma;
        while(true) {
            tok_scoma = GetNextToken(in, line);
            if (tok_scoma.GetToken() == SCOMA) {
                break;
            }
        }
        return true;
    } else {
        // evaluate the if statement
        bool statement = Stmt(in, line);
        if (!statement) {
            ParseError(line, "Missing statement for if");
            return false;
        }
        return true;
    }

}

bool Var(istream& in, int& line, LexItem& tok_iden) {
    tok_iden = GetNextToken(in, line);
    if (tok_iden.GetToken() != IDENT) {
        PushBackToken(tok_iden);
        return false;
    }

    string iden = tok_iden.GetLexeme();
    defVar[iden] = true;
    if (Value::symbolTable.find(iden) == Value::symbolTable.end()) {
        Value::symbolTable[iden] = Value();
    }

    return true;
}

// AssignStmt := Var = Expr
bool AssignStmt(istream& in, int& line) {
    LexItem tok_iden;
    if (!Var(in, line, tok_iden)) {
        return false;
    }
    LexItem tok_eq = GetNextToken(in, line);
    if (tok_eq.GetToken() != EQ) {
        ParseError(line, "Missing Assignment operator '='");
        return false;
    }
    Value right;
    if (!Expr(in, line, right)) {
        ParseError(line, "Missing Expression after operator '='");
        return false;
    }

    if (right.GetType() == VERR) {
        ParseError(line, "RUNTIME ERROR: Illegal Assignment Operation");
        return false;
    }

    // semantic of assign statement

    auto itLeft = Value::symbolTable.find(tok_iden.GetLexeme());
    Value left = itLeft->second;
    assert(itLeft != Value::symbolTable.end());
    if (left.GetType() == VERR) {
        // define new variable
        itLeft->second = right;
    } else if (left.GetType() == right.GetType()){
        // change value of same type variable
        itLeft->second = right;
    } else if (left.GetType() != right.GetType()){
        // different type assignment
        if (left.GetType() == VREAL && right.GetType() == VINT) {
            // conversion
            itLeft->second = Value((float) right.GetInt());
        } else if (left.GetType() == VINT && right.GetType() == VREAL) {
            // conversion
            itLeft->second = Value((int) right.GetReal());
        } else {
            // string cannot be converted to other type
            ParseError(line, "RUNTIME ERROR: Illegal Assignment Operation");
            return false;
        }
    }
    return true;
}

// ExprList := Expr {, Expr}
bool ExprList(istream& in, int& line) {

    Value val;
    if (!Expr(in, line, val)) {
        ParseError(line, "Invalid ExprList");
        return false;
    }
    Value::ValQue->push(val);

    while(true) {
        LexItem tok = GetNextToken(in, line);
        if (tok.GetToken() != COMA) {
            PushBackToken(tok);
            break;
        }

        Value nextVal;
        if (!Expr(in, line, nextVal)) {
            ParseError(line, "Invalid Expr after ','");
            return false;
        }
        Value::ValQue->push(nextVal);
    }
    return true;
}

// Expr := Term {(+|-) Term}
bool Expr(istream& in, int& line, Value& val) {
    if (!Term(in, line, val)) {
        //ParseError(line, "Invalid Expr");
        return false;
    }

    Value nextVal;
    while(true) {
        LexItem tok = GetNextToken(in, line);
        if (tok.GetToken() != PLUS && tok.GetToken() != MINUS) {
            PushBackToken(tok);
            break;
        }
        if (!Term(in, line, nextVal)) {
            ParseError(line, "Missing term");
            return false;
        }
        // do reduce the value
        try {
            if (tok.GetToken() == PLUS) {
                val = val + nextVal;
            } else if (tok.GetToken() == MINUS) {
                val = val - nextVal;
            } else {
                assert(false);
            }
        } catch (char const* e) {
            ParseError(line, e);
            return false;
        }
    }

    return true;

}

//  Term := Factor {(*|/) Factor}
bool Term(istream& in, int& line, Value& val) {
    if (!Factor(in, line, val)) {
        //ParseError(line, "Invalid Term");
        return false;
    }

    Value nextVal;
    while(true) {
        LexItem tok = GetNextToken(in, line);
        if (tok.GetToken() != MULT && tok.GetToken() != DIV) {
            PushBackToken(tok);
            break;
        } 
        if (!Factor(in, line, nextVal)) {
            ParseError(line, "Missing Factor");
            return false;
        }
        // reduce value 
        try {
            if (tok.GetToken() == MULT) {
                val = val * nextVal;
            } else if (tok.GetToken() == DIV) {
                val = val / nextVal;
            } else {
                assert(false);
            }
        } catch (char const* e) {
            ParseError(line, e);
            return false;
        }
    }

    return true;
}

//* Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, Value& val) {
    LexItem tok = GetNextToken(in, line);
    if (tok.GetToken() == IDENT) {
        string lexeme = tok.GetLexeme();
        auto it = Value::symbolTable.find(lexeme);
        if (it != Value::symbolTable.end() && it->second.GetType() != VERR) {
            val = it->second;
            return true;
        } else {
            ParseError(line, "Undefined varibale " + tok.GetLexeme());
            return false;
        }

    } else if (tok.GetToken() == ICONST) {
        int iconst;
        string lexeme = tok.GetLexeme();
        iconst = stoi(tok.GetLexeme());
        //cout << "integer const: " << iconst<< endl;
        val = Value(iconst);
        return true;
    } else if (tok.GetToken() == RCONST) {
        float rconst;
        string lexeme = tok.GetLexeme();
        rconst = stof(tok.GetLexeme());
        //cout << "float const: " << rconst<< endl;
        val = Value(rconst);
        return true;
    } else if (tok.GetToken() == SCONST) {
        string lexeme = tok.GetLexeme();
        //cout << "string const: " << lexeme << endl;
        val = Value(lexeme);
        return true;
    } else if (tok.GetToken() == LPAREN) {
        if (!Expr(in, line, val)) {
            ParseError(line, "Missing expression after (");
            return false;
        }
        if (GetNextToken(in, line).GetToken() != RPAREN) {
            ParseError(line, "Missing ) after expression");
            return false;
        }
        return true;
    } else {
        ParseError(line, "Unrecognized input");
        return false;
    }
}

