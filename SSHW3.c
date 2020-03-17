// Michael Said
// Liam May
// COP 3402
// Spring 2020

// This program is a representation of a PL/0 compiler in c. It contains a compiler
// driver, a parser, and an intermediate code generator. In this representation of
// PL/0 ":=" is represented by "=".

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_IDENT_LENGTH 11
#define MAX_NUM_LENGTH 5
#define MAX_CODE_LENGTH 550
#define MAX_SYMBOL_TABLE_SIZE 500

typedef enum
{
  nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5, multsym = 6,
  slashsym = 7, oddsym = 8, eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
  gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16, commasym = 17,
  semicolonsym = 18, periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22,
  ifsym = 23, thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
  varsym = 29, procsym = 30, writesym = 31, readsym = 32 , elsesym = 33
} token_type;

typedef struct lexemes
{
  token_type type;
  char *lexeme;
}lexeme;

typedef struct
{
	int kind; 		// const = 1, var = 2, proc = 3
	char name[10];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
}symbol;

typedef struct
{
  token_type type;
  char value[12];
}token;

char* trim(char *str, char *trimmed);
int parse(char *code, lexeme list[], FILE *fplex);
bool isReserved(char *str);
token_type reserved(char *str);
lexeme *createLexeme(token_type t, char *str);
bool isNumber(char *str);
bool isSymbol(char symbol);
void output(lexeme list[], int count, FILE *fplex);
void block(token current);
void statement(token current);
void condition(token current);
void expression(token current);
void term(token current);
void factor(token current);

lexeme *createLexeme(token_type t, char *str)
{
	// calloc() initializes the 'left' and 'right' pointers to NULL for us.
	lexeme *l;
	l->type = t;
  l->lexeme = malloc(sizeof(char) * MAX_IDENT_LENGTH);
  strcpy(l->lexeme, str);
	return l;
}

// Removes comments that start with "/*" and end with "*/"
char* trim(char *str, char *trimmed)
{
  int lp = 0, rp, diff, i = 0, len = strlen(str);

  while (str[lp] != '\0')
  {
    if (str[lp] == '/' && str[lp + 1] == '*')
    {
      rp = lp + 2;
      while (str[rp] != '*' && str[rp + 1] != '/')
      {
        rp++;
      }
      //rp += 2; // rp = rp + 2
      lp= rp;
    }
    trimmed[i] = str[lp];
    i++;
    lp++;
  }
  return trimmed;
}

// Parses words from the code to be evaluated, adds them to the lexeme array,
// and returns the number of lexemes that were added to the array
int parse(char *code, lexeme list[], FILE *fplex)
{
  lexeme *lexptr;
  int lp = 0, rp, length, i, listIndex = 0;
  char buffer[MAX_CODE_LENGTH];
  token_type t;

  // looping through string containing input
  while (code[lp] != '\0')
  {
    // ignoring whitespace
    if (isspace(code[lp]))
    {
      lp++;
    }
    if (isalpha(code[lp]))
    {
      rp = lp;

      // capturing length of substring
      while (isalpha(code[rp]) || isdigit(code[rp]))
      {
        rp++;
      }
      length = rp - lp;

      // checking for ident length error
      if (length > MAX_IDENT_LENGTH)
      {
        fprintf(fplex, "Err: ident length too long\n");
        return 0;
      }

      // creating substring
      for (i = 0; i < length; i++)
      {
        buffer[i] = code[lp + i];
      }
      buffer[i] = '\0';
      lp = rp;

      // adds reserved words to lexeme array
      if (isReserved(buffer))
      {
        t = reserved(buffer);
        lexptr = createLexeme(t, buffer);
        list[listIndex++] = *lexptr;
      }
      // must be a identifier at this line
      else
      {
        t = identsym;
        lexptr = createLexeme(t, buffer);
        list[listIndex++] = *lexptr;
      }
    }
    else if (isdigit(code[lp]))
    {
      rp = lp;

      i = 0;
      // capturing length of substring
      while (isdigit(code[lp + i]))
      {
        rp++;
        i++;
      }
      length = rp - lp;

      // checking for ident length error
      if (length > MAX_NUM_LENGTH)
      {
        fprintf(fplex, "Err: number length too long\n");
        return 0;
      }

      // creating substring
      for (i = 0; i < length; i++)
      {
        buffer[i] = code[lp + i];
      }
      buffer[i] = '\0';
      lp = rp;

      t = numbersym;
      lexptr = createLexeme(t, buffer);
      list[listIndex++] = *lexptr;
    }

    // Creating a lexeme for the symbol
    else if (isSymbol(code[lp]))
    {
      if (code[lp] == '+')
      {
        t = 4;
      }
      if (code[lp] == '-')
      {
        t = 5;
      }
      if (code[lp] == '*')
      {
        t = 6;
      }
      if (code[lp] == '/')
      {
        t = 7;
      }
      if (code[lp] == '(')
      {
        t = 15;
      }
      if (code[lp] == ')')
      {
        t = 16;
      }
      if (code[lp] == '=')
      {
        t = 9;
      }
      if (code[lp] == ',')
      {
        t = 17;
      }
      if (code[lp] == '.')
      {
        t = 19;
      }
      if (code[lp] == '<')
      {
        t = 11;
      }
      if (code[lp] == '>')
      {
        t = 13;
      }
      if (code[lp] == ';')
      {
        t = 18;
      }
      if (code[lp] == ':')
      {
        t = 20;
      }

      buffer[0] = code[lp];
      buffer[1] = '\0';
      lexptr = createLexeme(t, buffer);
      list[listIndex++] = *lexptr;

      lp++;
    }
  }
  return listIndex;
}

// Returns true if the char is a member of the set of valid symbols and false
// otherwise
bool isSymbol(char symbol)
{
  char validsymbols[13] = {'+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>',  ';', ':'};

  for(int i=0; i<13; i++)
  {
    if(symbol == validsymbols[i])
    {
      return 1;
    }
  }
  return 0;
}

// Returns true if string is a valid number and false otherwise
bool isNumber(char *str)
{
  int i, len = strlen(str);

  if (len > MAX_NUM_LENGTH)
  {
    return false;
  }
  for (i = 0; i < len; i++)
  {
    if (!isdigit(str[i]))
    {
      return false;
    }
  }
  return true;
}

// return true if the string is a reserved keyword and false otherwise
bool isReserved(char *str)
{
  // Table of reserved word names
  char reserved[14][9] = { "const", "var", "procedure", "call", "begin", "end",
                           "if", "then", "else", "while", "do", "read", "write",
                           "odd" };

  if (str[0] == 'b')
  {
    if (strcmp(reserved[4], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 'c')
  {
    if (strcmp(reserved[0], str) == 0)
    {
      return true;
    }
    else if (strcmp(reserved[3], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 'd')
  {
    if (strcmp(reserved[10], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 'e')
  {
    if (strcmp(reserved[5], str) == 0)
    {
      return true;
    }
    else if (strcmp(reserved[8], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 'i')
  {
    if (strcmp(reserved[6], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 'o')
  {
    if (strcmp(reserved[13], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 'p')
  {
    if (strcmp(reserved[2], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 'r')
  {
    if (strcmp(reserved[11], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 't')
  {
    if (strcmp(reserved[7], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 'v')
  {
    if (strcmp(reserved[1], str) == 0)
    {
      return true;
    }
  }
  if (str[0] == 'w')
  {
    if (strcmp(reserved[9], str) == 0)
    {
      return true;
    }
    else if (strcmp(reserved[12], str) == 0)
    {
      return true;
    }
  }
  return false;
}

// Returns the value assigned to the reserved keyword sent to this function
token_type reserved(char *str)
{
  // Table of reserved word names
  char reserved[14][9] = { "const", "var", "procedure", "call", "begin", "end",
                           "if", "then", "else", "while", "do", "read", "write",
                           "odd" };

  if (str[0] == 'b')
  {
    if (strcmp(reserved[4], str) == 0)
    {
      return 21;
    }
  }
  if (str[0] == 'c')
  {
    if (strcmp(reserved[0], str) == 0)
    {
      return 28;
    }
    else if (strcmp(reserved[3], str) == 0)
    {
      return 27;
    }
  }
  if (str[0] == 'd')
  {
    if (strcmp(reserved[10], str) == 0)
    {
      return 26;
    }
  }
  if (str[0] == 'e')
  {
    if (strcmp(reserved[5], str) == 0)
    {
      return 22;
    }
    else if (strcmp(reserved[8], str) == 0)
    {
      return 33;
    }
  }
  if (str[0] == 'i')
  {
    if (strcmp(reserved[6], str) == 0)
    {
      return 23;
    }
  }
  if (str[0] == 'o')
  {
    if (strcmp(reserved[13], str) == 0)
    {
      return 8;
    }
  }
  if (str[0] == 'p')
  {
    if (strcmp(reserved[2], str) == 0)
    {
      return 30;
    }
  }
  if (str[0] == 'r')
  {
    if (strcmp(reserved[11], str) == 0)
    {
      return 32;
    }
  }
  if (str[0] == 't')
  {
    if (strcmp(reserved[7], str) == 0)
    {
      return 24;
    }
  }
  if (str[0] == 'v')
  {
    if (strcmp(reserved[1], str) == 0)
    {
      return 29;
    }
  }
  if (str[0] == 'w')
  {
    if (strcmp(reserved[9], str) == 0)
    {
      return 25;
    }
    else if (strcmp(reserved[12], str) == 0)
    {
      return 31;
    }
  }
  return 0;
}

// Prints leveme list to output file
void output(lexeme list[], int count, FILE *fplex)
{
  int i = 0;
  // fprintf(fplex, "Lexeme Table:\nLexeme\t\tToken Type\n");
  // for (i = 0; i < count; i++)
  // {
  //   fprintf(fplex, "%s\t\t%d\n", list[i].lexeme, list[i].type);
  // }

  // fprintf(fplex, "Lexeme List:\n");
  for(i = 0; i < count; i++)
  {
    fprintf(fplex, "%d ", list[i].type);
    if(list[i].type == 2 || list[i].type == 3)
    {
      fprintf(fplex, "%s ", list[i].lexeme);
    }
  }
  fprintf(fplex, "\n");
}

// Places the token from the index of the lexeme list and assigns it to current
// token
void getToken(token current)
{
  // int i = 0;
  //
  // // Assigning lexeme token at index to current
  // current.type = list[index].type;
  //
  // // If token type is identsym or numbersym, assigning value to next index of array
  // if (current.type == 2 || current.type == 3)
  // {
  //   strcpy(current.value, list[index + 1].lexeme);
  // }
	if( fscanf( fpin, "%s", currrent.type ) != EOF ){

    //If token is identsym or numbersym, get token value.
    if( !strcmp(current.type, "2") || !strcmp(current.type, "3") )
      fscanf( fpin, "%s", current.value );

    //All other tokens.
    else current.value[0] = '\0';

    //Debugging Statement:
    //printf("Token: Type: %s, Value: %s .\n", curr_token.type, curr_token.value);

    //Successful token fetch.
    return 1;
	

}

void findError(int errorNum)
{
    switch( errorNum )
    {

    case 1:
      printf("Use = instead of := \n");
      break;

    case 2:
      printf("= must be followed by a number \n");
      break;

    case 3:
      printf("Identifier must be followed by = \n");
      break;

    case 4:
      printf("const, int, procedure must be followed by identifier\n");
      break;

    case 5:
      printf("Semicolon or comma missing\n");
      break;

    case 6:
      printf("Incorrect symbol after procedure declaration\n");
      break;

    case 7:
      printf("Statement expected\n");
      break;

    case 8:
      printf("Incorrect symbol after statement part in block\n");
      break;

    case 9:
      printf("Period expected\n");
      break;

    case 10:
      printf("Semicolon between statements missing\n");
      break;

    case 11:
      printf("Undeclared identifier \n");
      break;

    case 12:
      printf("Assignment to constant or procedure is not allowed\n");
      break;

    case 13:
      printf("Assignment operator expected\n");
      break;

    case 14:
      printf("Call must be followed by an identifier\n");
      break;

    case 15:
      printf("Call of a constant or variable is meaningless\n");
      break;

    case 16:
      printf("Then expected\n");
      break;

    case 17:
      printf("Semicolon or } expected \n");
      break;

    case 18:
      printf("Do expected\n");
      break;

    case 19:
      printf("Incorrect symbol following statement\n");
      break;

    case 20:
      printf("Relational operator expected\n");
      break;

    case 21:
      printf("Expression must not contain a procedure identifier\n");
      break;

    case 22:
      printf("Right parenthesis missing\n");
      break;

    case 23:
      printf("The preceding factor cannot begin with this symbol\n");
      break;

    case 24:
      printf("An expression cannot begin with this symbol\n");
      break;

    case 25:
      printf("This number is too large\n");
      break;

      default:
      printf("Invalid instruction");
    }
}

void insertSymbols(int counter, symbol symbol_table[], int kind, char name[], int val, int level, int addr)
{
  //filling in the symbol tables with current data
  //note: counter needs to be set to zero in main
  symbol_table[counter].kind = kind;
  //capture name string and insert it into symbol table as a string
  strcpy(symbol_table[counter].name, name);
  symbol_table[counter].val = val;
  symbol_table[counter].level = level;
  symbol_table[counter].addr = addr;

  counter++;
}

void block(token current)
{
  if (current.type == constsym)
  {
    while (current.type != commasym)
    {
      getToken(current);
      if (current.type != identsym)
        findError(4);
      if (current.type != eqlsym)
        findError(3);
      if (current.type != numbersym)
        findError(2);
    }
    if (current.type != semicolonsym)
      findError(5);
    getToken(current);
  }
  if (current.type = varsym) // ???
  {
    while (current.type != commasym)
    {
      getToken(current);
      if (current.type != identsym)
        findError(4);
      getToken(current);
    }
    if (current.type != semicolonsym)
      findError(5);
    getToken(current);
  }
  while (current.type == procsym)
  {
    getToken(current);
    if (current.type != identsym)
      findError(4); // maybee???????
    getToken(current);
    if (current.type != semicolonsym)
      findError(5);
    getToken(current);
  }
  while (current.type == procsym)
  {
    getToken(current);
    if (current.type != identsym)
      findError(4); // ???
    getToken(current);
    if (current.type != semicolonsym)
      findError(5);
    getToken(current);

    block(current);

    if (current.type != semicolonsym)
      findError(5);
    getToken(current);
  }
  statement(current);
}

void program(lexeme list[], int length, token current)
{
  getToken();
  block();
  if(current.type != periodsym)
  {
  findError(9);
  }
}

void statement(lexeme list[], int length, token current)
{
  if(current.type == identsym)
  {
    getToken();
    if(current.type != becomessym)
      findError(6); // ??????????
    getToken();
    expression();
  }
  else if(current.type == callsym )
  {
    getToken();
    if(current.type != identsym)
    findError(4);
    getToken();
  }
  else if(current.type == beginsym)
  {
    getToken();
    statement();
    while(current.type == semicolonsym)
    {
      getToken();
      statement();
    }
    if(current.type != endsym)
    {
      findError(5);
    }
    getToken();
  }
  
  else if(current.type == ifsym)
  {
    getToken();
    condition();
    if(current.type != thensym)
    findError(16);
    statement();
  }
  else if(current.type ==whilesym)
  {
    getToken();
    condition();
    if(current.type != dosym)
    {
    getToken();
    statement();
    }
  }
  
}

void condition(token current)
{
  if (current.type == oddsym)
  {
    getToken(current);
    expression(current);
  }
  else
  {
    expression(current);
    if (current.type != eqlsym && current.type != neqsym && current.type != lessym
        && current.type != leqsym && current.type != gtrsym && current.type != geqsym)
        findError(20);
    getToken(current);
    expression(current);
  }
}

void expression(token current)
{
  if (current.type == plussym || current.type == minussym)
  {
    getToken(current);
    term(current);
  }
}

void term(token current)
{
  factor(current);
  while (current.type == multsym || current.type == slashsym)
  {
    getToken(current);
    factor(current);
  }
}

void factor(token current)
{
  if (current.type == identsym)
  {
    getToken(current);
  }
  else if (current.type == numbersym)
  {
    getToken(current);
  }
  else if (current.type == lparentsym)
  {
    getToken(current);
    expression(current);
    if (current.type != rparentsym)
    {
      findError(22);
      getToken(current);
    }
  }
  else
  {
    findError(23); // ???
  }
}

int main(int argc, char **argv)
{
  FILE *fpin, *fplex;
  fpin = fopen(argv[1], "r");
  fplex = fopen(argv[2], "r+");
  char aSingleLine[MAX_CODE_LENGTH], code[MAX_CODE_LENGTH] = {'\0'},
       trimmed[MAX_CODE_LENGTH] = {'\0'};
  lexeme list[MAX_CODE_LENGTH] = {'\0'};
  int count, i, tokens[MAX_SYMBOL_TABLE_SIZE] = {'\0'};
  symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
  token current;

  // output for user that makes error entering command line arguments
  if (argc < 3)
  {
    printf("Err: incorrect program call\nSyntax: ./a.out <inputfilename> <outputfilename>\n");
    return 0;
  }
  // Preventing file errors by checking for failures to open files
  if (fpin == NULL)
  {
    printf("File not found\n");
    return 0;
  }
  if (fplex == NULL)
  {
    printf("File not found\n");
    return 0;
  }

  // Scanning file into code array
  while(!feof(fpin))
  {
    fgets(aSingleLine, MAX_CODE_LENGTH, fpin);
    strcat(code, aSingleLine);
  }

  // Removing all comments from code
  strcpy(code, trim(code, trimmed));
  // Filling lexeme array and capturing number of elements of lexeme array
  count = parse(code, list, fplex);
  // Printing output that represents the lexeme array
  output(list, count, fplex);
  block(current);

  fclose(fpin);
  fclose(fplex);
  return 0;
}
