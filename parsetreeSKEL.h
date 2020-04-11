/*
 * karan
 * CS280
 * Fall 2019
 */

#ifndef PARSETREE_H_
#define PARSETREE_H_

#include <vector>
#include <map>
using std::vector;
using std::map;

// NodeType represents all possible types
enum NodeType { ERRTYPE, INTTYPE, STRTYPE };

// a "forward declaration" for a class to hold values
class Value;
static map<string,bool> iden;

class ParseTree {
protected:
	int			linenum;
	ParseTree	*left;
	ParseTree	*right;

public:
	ParseTree(int linenum, ParseTree *l = 0, ParseTree *r = 0)
		: linenum(linenum), left(l), right(r) {}

	virtual ~ParseTree() {
		delete left;
		delete right;
	}

	int GetLineNumber() const { return linenum; }
    ParseTree *GetLeft() const { return left; }
	ParseTree *GetRight() const { return right; }

	int MaxDepth() const {
        int depthCountL = 0;
        int depthCountR = 0;
		if( left ) {            
            depthCountL += left->MaxDepth();
        }
        if( right ) {
            depthCountR += right->MaxDepth();
        }      
        if(depthCountL >  depthCountR){
             return depthCountL + 1;
        }
        else{
            return depthCountR + 1;
        }
	}
    
    int BangCount() const {
        int bangC = 0;
		
		if( left ) bangC += left->BangCount();
		if( right ) bangC += right->BangCount();
        
        if(IsBang()){
            bangC++;
        }
        
		return bangC;
	}
    

	virtual bool IsIdent() const { return false; }
	virtual bool IsVar() const { return false; }
	virtual string GetId() const { return ""; }
    virtual int IsBang() const { return 0; }
    virtual bool IsLet() const { return false; }	
	virtual void CheckLetBeforeUse(map<string,bool>& var) {
	}
};


class StmtList : public ParseTree {
public:
	StmtList(ParseTree *l, ParseTree *r) : ParseTree(0, l, r) {}
};


class Let : public ParseTree {
	string id;
public:
	Let(Lex& t, ParseTree *e) : ParseTree(t.GetLinenum(), e), id(t.GetLexeme()) {
        iden.insert( {id,IsLet()} );
    }

	string GetId() const { return id; }
	bool IsLet() const { return true; }
};


class Print : public ParseTree {
public:
	Print(int line, ParseTree *e) : ParseTree(line, e) {}
};


class Loop : public ParseTree {
public:
	Loop(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}
};


class If : public ParseTree {
public:
	If(int line, ParseTree *ex, ParseTree *stmt) : ParseTree(line, ex, stmt) {}
};


class PlusExpr : public ParseTree {
public:
	PlusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};


class MinusExpr : public ParseTree {
public:
	MinusExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};


class TimesExpr : public ParseTree {
public:    
   TimesExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};


class DivideExpr : public ParseTree {
public:
	DivideExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
};


class BangExpr : public ParseTree {
public:
	BangExpr(int line, ParseTree *l, ParseTree *r) : ParseTree(line,l,r) {}
    int IsBang() const { return 1;}    
};


class IConst : public ParseTree {
	int val;

public:
	IConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = stoi(t.GetLexeme());
	}
};

class SConst : public ParseTree {
	string val;

public:
	SConst(Lex& t) : ParseTree(t.GetLinenum()) {
		val = t.GetLexeme();
	}
};

class Ident : public ParseTree {
	string id;

public:
	Ident(Lex& t) : ParseTree(t.GetLinenum()), id(t.GetLexeme()) {
        CheckLetBeforeUse(iden);
    }

	bool IsIdent() const { return true; }
	string GetId() const { return id; }
    void CheckLetBeforeUse(map<string,bool>& var) {
        if(var.find(id) == var.end()){
            cout << "UNDECLARED VARIABLE " << id << endl;
        }
    }
};

#endif /* PARSETREE_H_ */
