/*
 * karan
 * CS280
 * Fall 2019
 */

#include "parse.h"
#include "parsetreeSKEL.h"
#include "lex.h"

namespace Parser {
bool pushed_back = false;
Lex	pushed_token;

static Lex GetNextToken(istream& in, int& line) {
	if( pushed_back ) {
		pushed_back = false;
		return pushed_token;
	}
	return getNextToken(in, line);
}

static void PushBackToken(Lex& t) {
	if( pushed_back ) {
		abort();
	}
	pushed_back = true;
	pushed_token = t;
}

}

static int error_count = 0;

void
ParseError(int line, string msg)
{
	++error_count;
	cout << line+1 << ": " << msg << endl;
}

ParseTree *Prog(istream& in, int& line)
{
	ParseTree *sl = Slist(in, line);

	if( sl == 0 )
		ParseError(line, "No statements in program");

	if( error_count )
		return 0;

	return sl;
}

// Slist is a Statement followed by a Statement List
ParseTree *Slist( istream& in, int& line ) {
	ParseTree *s = Stmt(in, line);
	if( s == 0 )
		return 0;
    Lex t;
    if( (t = Parser::GetNextToken(in, line)) != SC ) {      
		ParseError(line, "Missing statement separator");
		return 0;
	}

	return new StmtList(s, Slist(in,line));
}

ParseTree *Stmt( istream& in, int& line ) {
    ParseTree *s = 0;

	Lex t = Parser::GetNextToken(in, line);
    
	switch( t.GetToken() ) {
	case IF:
		s = IfStmt(in, line);
		break;

	case LET:
		s = LetStmt(in, line);
		break;

	case LOOP:
		s = LoopStmt(in, line);
		break;
            
    case PRINT:
		s = PrintStmt(in, line);
		break;

	case ERR:
		ParseError(line, "Invalid token");
		break;

	case DONE:
		break;

	case SC:
		// an empty statement; get the next one
		s = Stmt(in, line);
		break;

	default:
		Parser::PushBackToken(t);
		break;
	}

	return s;
}

ParseTree *IfStmt( istream& in, int& line ) {
    ParseTree *expression = Expr(in, line);
    if(expression == 0){
        ParseError(line, "Missing expression after if");
        return 0;
    }
    
    Lex t2 = Parser::GetNextToken(in, line);
    if(t2 != BEGIN){
        ParseError(line, "Missing BEGIN after expression");
        return 0;
    }
    
    //only executes if expr !=0
    ParseTree *statement = Slist(in, line); 
    if(statement ==0){
        ParseError(line, "Missing statement after BEGIN");
        return 0;
    }
    
    Lex t3 = Parser::GetNextToken(in, line);
    if(t3 != END){
        ParseError(line, "Missing END after statement");
        return 0;
    }
    
    return new If(line, expression, statement);
}

ParseTree *LetStmt( istream& in, int& line ) {     
    Lex t = Parser::GetNextToken(in, line);  
    
    if(t != ID){
        ParseError(line, "Missing ID after let");
        return 0;
    }
    
    ParseTree *expression = Expr(in, line);   
    if(expression == 0){
        ParseError(line, "Missing expression after id");
        return 0;
    }
    
    return new Let(t, expression);
}

ParseTree *LoopStmt( istream& in, int& line ) {
    ParseTree *expression = Expr(in, line);
    if(expression == 0){
        ParseError(line, "Missing expression after loop");
        return 0;
    }
    
    Lex t2 = Parser::GetNextToken(in, line);
    if( t2 != BEGIN ) {
        ParseError(line, "Missing BEGIN after expression");
        return 0;
    }
    
    //only executes if expr !=0
    ParseTree *statement = Slist(in, line);
    
    if( statement == 0 ) {
        ParseError(line, "Missing statement list after BEGIN");
        return 0;
    }
    
     Lex t3 = Parser::GetNextToken(in, line);
    if( t3 != END ) {
        ParseError(line, "Missing END after statement THIS ONE");
        return 0;
    }
    
    return new Loop(line, expression, statement);
}

ParseTree *PrintStmt( istream& in, int& line ) {
    ParseTree *expression = Expr(in, line);
    if( expression ==0 ) {
        ParseError(line, "Missing expression after print");
        return 0;
    }
    
    Lex t1 = Parser::GetNextToken(in, line);    
    if( t1 == SC ) {
       Parser::PushBackToken(t1);      
    }     
    
    return new Print(line, expression);   
}

ParseTree *Expr( istream& in, int& line ) {
	ParseTree *t1 = Prod(in, line);
	if( t1 == 0 ) {
		return 0;
	}

	while ( true ) {
		Lex t = Parser::GetNextToken(in, line);

		if( t != PLUS && t != MINUS ) {
			Parser::PushBackToken(t);
			return t1;
		}

		ParseTree *t2 = Prod(in, line);
		if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}

		if( t == PLUS )
			t1 = new PlusExpr(t.GetLinenum(), t1, t2);
		else
			t1 = new MinusExpr(t.GetLinenum(), t1, t2);
	}
}

ParseTree *Prod( istream& in, int& line ) {
    ParseTree *t1 = Rev(in, line);
    if( t1 == 0 ) {
        return 0;
    }
    
    while( true ) {
        Lex t = Parser::GetNextToken(in, line);        
        if(t != STAR && t!= SLASH){
           Parser::PushBackToken(t);
            return t1;
        }
        
        ParseTree *t2 = Rev(in, line);    
        if( t2 == 0 ) {
			ParseError(line, "Missing expression after operator");
			return 0;
		}
        if( t == STAR ) {
            t1 = new TimesExpr(t.GetLinenum(), t1, t2);
        }
        else if( t == SLASH ){
            t1 = new DivideExpr(t.GetLinenum(), t1, t2);
        }
    } 
    return t1;
    
}

ParseTree *Rev( istream& in, int& line ) {
        
    Lex r = Parser::GetNextToken(in, line);

	if( r == BANG ) {        
        ParseTree *r1 = Rev(in, line);
        if( r1 == 0 ) {
            ParseError(line, "Missing rev after bang");
            return 0;
        }
		return new BangExpr(r.GetLinenum(), 0, r1);     
	}
    
    Parser::PushBackToken(r);
    ParseTree *r2 = Primary(in, line);
    if( r2 == 0 ) {
        ParseError(line, "Missing Primary");
            return 0;
        }
    return r2;        
}

ParseTree *Primary( istream& in, int& line ) {
	Lex t = Parser::GetNextToken(in, line);

	if( t == ID ) {
		return new Ident(t);
	}
	else if( t == INT ) {
		return new IConst(t);
	}
	else if( t == STR ) {
		return new SConst(t);
	}
	else if( t == LPAREN ) {
		ParseTree *ex = Expr(in, line);
		if( ex == 0 ) {
			ParseError(line, "Missing expression after (");
			return 0;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;

		ParseError(line, "Missing ) after expression");
		return 0;
	}

	ParseError(line, "Primary expected");
	return 0;
}

