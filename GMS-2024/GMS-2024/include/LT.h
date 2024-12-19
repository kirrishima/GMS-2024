#pragma once

#define LEXEMA_FIXSIZE 1             
#define LT_MAXSIZE 4096               
#define LT_TI_NULLIDX 0XFFFFFFFF        

#define LEX_TYPE		't'
#define LEX_INTEGER		LEX_TYPE             
#define LEX_STRING		LEX_TYPE            
#define LEX_CHAR		LEX_TYPE            

#define LEX_ID		'i'                 
#define LEX_LITERAL 'l'				    

#define LEX_FUNCTION	'f'             
#define LEX_DECLARE		'd'              
#define LEX_RETURN		'r'             
#define LEX_PRINT		'p'             
#define LEX_WRITE		'w'             
#define LEX_STRCMP		's'            
#define LEX_MAIN		'm'              

#define LEX_SEMICOLON ';'             
#define LEX_COMMA ','               
#define LEX_LEFTBRACE '{'             
#define LEX_BRACELET '}'              
#define LEX_LEFTTHESIS '('            
#define LEX_RIGHTTHESIS ')'           

#define LEX_MATH 'v'
#define LEX_PLUS		LEX_MATH            
#define LEX_MINUS		LEX_MATH            
#define LEX_STAR		LEX_MATH            
#define LEX_DIRSLASH	LEX_MATH            

#define LEX_EQUAL '='               

#define LEX_COMPARE 'c'

#define LEX_COMPARE_EQUAL			LEX_COMPARE		  
#define LEX_COMPARE_GREATER			LEX_COMPARE       
#define LEX_COMPARE_LESS			LEX_COMPARE		  
#define LEX_COMPARE_NOT_EQUAL		LEX_COMPARE        
#define LEX_COMPARE_LESS_EQUAL		LEX_COMPARE         
#define LEX_COMPARE_GREATER_EQUAL	LEX_COMPARE		    

#define LEX_IF '?'                  
#define LEX_ELSE ':'                


namespace LT
{
	struct Entry
	{
		char lexema[LEXEMA_FIXSIZE];
		int sn;
		int idxTI;
		char v;
		std::string c;
	};

	struct LexTable
	{
		int maxsize;
		int size;
		Entry* table;
	};

	LexTable Create(
		int size
	);

	void Add(
		LexTable& lextable,
		Entry entry
	);

	Entry GetEntry(
		const LexTable& lextable,
		int n
	);

	bool hasMainLexem(const LexTable& table);
	void Delete(LexTable& lextable);
}

