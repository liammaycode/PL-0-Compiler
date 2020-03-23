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

FILE *fpin, *fplex;

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
int parse(char *code, lexeme list[], FILE *fplex, symbol symbol_table[]);
bool isReserved(char *str);
token_type reserved(char *str);
lexeme *createLexeme(token_type t, char *str);
bool isNumber(char *str);
bool isSymbol(char symbol);
void output(lexeme list[], int count, FILE *fplex, bool l, bool a, bool v);
int block(token current);
int statement(token current);
int condition(token current);
void expression(token current);
void term(token current);
int factor(token current);
void print(int tokenRep);

lexeme *createLexeme(token_type t, char *str)
{
	lexeme *l = malloc(1 * sizeof(lexeme));
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
// and returns the number of lexemes that were added to the array or 0 if there
// was an error
int parse(char *code, lexeme list[], FILE *fplex, symbol symbol_table[])
{
  lexeme *lexptr;
  int lp = 0, rp, length, i, listIndex = 0, symIndex = 0;
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
    // printf("%c\n", code[lp]);
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
        lexptr = createLexeme(t, buffer); // segfault??
        list[listIndex++] = *lexptr;
      }
      // must be a identifier at this line
      // printf("must be a identifier at this line\n");
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
        if(code[lp+1] == '=')
        {
          t=12;
        }
        if(code[lp+1] == '>')
        {
          t=10;
        }
      }
      if (code[lp] == '>')
      {
        t = 13;
        if(code[lp+1] == '=')
        {
          t= 13;
        }
      }
      if (code[lp] == ';')
      {
        t = 18;
      }
      if (code[lp] == ':')
      {
        t = 20;
        if(code[lp+1] == '=')
        {
          t=9;
        }
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
void output(lexeme list[], int count, FILE *fplex, bool l, bool a, bool v)
{
  int i = 0;
  char buffer[13] = {'\0'};

  if (l == false && a == false && v == false)
  {
    fprintf(fplex, "in\tout\n");
    return;
  }
  if (l == true)
  {
    fprintf(fplex, "List of lexemes:\n\n");
    for (i = 0; i < count; i++)
    {
      fprintf(fplex, "%s", list[i].lexeme);
      (i % 10 == 0) ? fprintf(fplex, "\n") : fprintf(fplex, "\t");
    }
    fprintf(fplex, "\n\nSymbolic representation:\n\n");
    for (i = 0; i < count; i++)
    {
      // fprintf(fplex, "%s", list[i].type);
      // call print to conver number to string
      print(list[i].type);
      (i % 10 == 0) ? fprintf(fplex, "\n") : fprintf(fplex, "\t");
    }
    fprintf(fplex, "\nNo errors, program is syntactically correct\n\n");
  }
  if (a == true)
  {

  }
  if (v == true)
  {

  }
}

void print(int tokenRep)
{
  switch (tokenRep)
  {
    case 1: fprintf(fplex, "nulsym");
      break;
    case 2: fprintf(fplex, "identsym");
      break;
    case 3: fprintf(fplex, "numbersym");
      break;
    case 4: fprintf(fplex, "plussym");
      break;
    case 5: fprintf(fplex, "minussym");
      break;
    case 6: fprintf(fplex, "multsym");
      break;
    case 7: fprintf(fplex, "slashsym");
      break;
    case 8: fprintf(fplex, "oddsym");
      break;
    case 9: fprintf(fplex, "eqlsym");
      break;
    case 10: fprintf(fplex, "neqsym");
      break;
    case 11: fprintf(fplex, "lessym");
      break;
    case 12: fprintf(fplex, "leqsym");
      break;
    case 13: fprintf(fplex, "gtrsym");
      break;
    case 14: fprintf(fplex, "geqsym");
      break;
    case 15: fprintf(fplex, "lparentsym");
      break;
    case 16: fprintf(fplex, "rparentsym");
      break;
    case 17: fprintf(fplex, "commasym");
      break;
    case 18: fprintf(fplex, "semicolonsym");
      break;
    case 19: fprintf(fplex, "periodsym");
      break;
    case 20: fprintf(fplex, "becomessym");
      break;
    case 21: fprintf(fplex, "beginsym");
      break;
    case 22: fprintf(fplex, "endsym");
      break;
    case 23: fprintf(fplex, "ifsym");
      break;
    case 24: fprintf(fplex, "thensym");
      break;
    case 25: fprintf(fplex, "whilesym");
      break;
    case 26: fprintf(fplex, "dosym");
      break;
    case 27: fprintf(fplex, "callsym");
      break;
    case 28: fprintf(fplex, "constsym");
      break;
    case 29: fprintf(fplex, "varsym");
      break;
    case 30: fprintf(fplex, "procsym");
      break;
    case 31: fprintf(fplex, "writesym");
      break;
    case 32: fprintf(fplex, "readsym");
      break;
    case 33: fprintf(fplex, "elsesym");
      break;
  }
}

// Places the token from the index of the lexeme list and assigns it to current
// token
void getToken(token current)
{
  char buffer[MAX_CODE_LENGTH];
  if (fscanf(fplex, "%s", buffer) != EOF)
  {
    if (strcmp(buffer, "2") == 0 || strcmp(buffer, "3") == 0)
    {
      fscanf(fplex, "%s", buffer);
    }
    else
    {
      current.value[0] = '\0';
    }
    strcpy(current.value, buffer);
  }
  else
  {
    current.type[0] = '\0';
    current.value[0] = '\0';
  }
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

// Stores passed values in symbol table
// For constants (28), you must store kind, name and value.
// For variables (29), you must store kind, name, L and M.
void insertSymbol(int counter, symbol symbol_table[], int kind, char name[], int val, int level, int addr)
{
  // Filling in the symbol tables with current data
  // Note: counter needs to be set to zero in main
  symbol_table[counter].kind = kind;
  // Captures name string and insert it into symbol table as a string
  strcpy(symbol_table[counter].name, name);
  symbol_table[counter].val = val;
  symbol_table[counter].level = level;
  symbol_table[counter].addr = addr;

  counter++;
}

int block(token current)
{
  if (current.type == constsym)
  {
    while (current.type != commasym)
    {
      getToken(current);
      if (current.type != identsym)
      {
        findError(4);
        return 0;
      }
      if (current.type != eqlsym)
      {
        findError(3);
        return 0;
      }
      if (current.type != numbersym)
      {
        findError(2);
        return 0;
      }
    }
    if (current.type != semicolonsym)
    {
      findError(5);
      return 0;
    }
    getToken(current);
  }
  if (current.type = varsym) // ???
  {
    while (current.type != commasym)
    {
      getToken(current);
      if (current.type != identsym)
      {
        findError(4);
        return 0;
      }
      getToken(current);
    }
    if (current.type != semicolonsym)
    {
      findError(5);
      return 0;
    }
    getToken(current);
  }
  while (current.type == procsym)
  {
    getToken(current);
    if (current.type != identsym)
    {
      findError(4); // maybee???????
      return 0;
    }
    getToken(current);
    if (current.type != semicolonsym)
    {
      findError(5);
      return 0;
    }
    getToken(current);
  }
  while (current.type == procsym)
  {
    getToken(current);
    if (current.type != identsym)
    {
      findError(4); // ???
      return 0;
    }
    getToken(current);
    if (current.type != semicolonsym)
    {
      findError(5);
      return 0;
    }
    getToken(current);
    block(current);

    if (current.type != semicolonsym)
    {
      findError(5);
      return 0;
    }
    getToken(current);
  }
  if (statement(current) == 0)
    return 0;

  return 1;
}

int program(token current)
{
  getToken(current);
  if (block(current) == 0)
    return 0;
  if(current.type != periodsym)
  {
    findError(9);
    return 0;
  }
  return 1;
}

int statement(token current)
{
  if(current.type == identsym)
  {
    getToken(current);
    if(current.type != becomessym)
    {
      findError(6); // ??????????
      return 0;
    }
    getToken(current);
    expression(current);
  }
  else if(current.type == callsym )
  {
    getToken(current);
    if(current.type != identsym)
    {
      findError(4);
      return 0;
    }
    getToken(current);
  }
  else if(current.type == beginsym)
  {
    getToken(current);
    if (statement(current) == 0)
    {
      return 0;
    }
    while(current.type == semicolonsym)
    {
      getToken(current);
      if (statement(current) == 0)
      {
        return 0;
      }
    }
    if(current.type != endsym)
    {
      findError(5);
      return 0;
    }
    getToken(current);
  }

  else if(current.type == ifsym)
  {
    getToken(current);
    if (condition(current) == 0)
    {
      return 0;
    }
    if(current.type != thensym)
    {
      findError(16);
      return 0;
    }
    if (statement(current) == 0)
    {
      return 0;
    }
  }
  else if(current.type == whilesym)
  {
    getToken(current);
    if (condition(current) == 0)
    {
      return 0;
    }
    if(current.type != dosym)
    {
      getToken(current);
      if (statement(current) == 0)
      {
        return 0;
      }
    }
  }
  return 1;
}

int condition(token current)
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
    {
      findError(20);
      return 0;
    }
    getToken(current);
    expression(current);
  }
  return 1;
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
  if (factor(current) == 0)
  {
    return;
  }
  while (current.type == multsym || current.type == slashsym)
  {
    getToken(current);
    if (factor(current) == 0)
    {
      return;
    }
  }
}

int factor(token current)
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
      return 0;
    }
    getToken(current);
  }
  else
  {
    findError(23); // ???
    return 0;
  }
  return 1;
}

int main(int argc, char **argv)
{
  fpin = fopen(argv[1], "r");
  fplex = fopen(argv[2], "w+");
  char aSingleLine[MAX_CODE_LENGTH], code[MAX_CODE_LENGTH] = {'\0'},
       trimmed[MAX_CODE_LENGTH] = {'\0'}, commands[3][3];
  lexeme list[MAX_CODE_LENGTH] = {'\0'};
  int count, i, tokens[MAX_SYMBOL_TABLE_SIZE] = {'\0'};
  symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
  token current;
  bool l = false, a = false, v = false;

  // output for user that makes error entering command line arguments
  if (argc < 3 || argc > 6)
  {
    return 0;
  }
  if (argc == 4)
  {
    strcpy(commands[0], argv[3]);
  }
  if (argc == 5)
  {
    strcpy(commands[0], argv[3]);
    strcpy(commands[1], argv[4]);
  }
  if (argc == 6)
  {
    strcpy(commands[0], argv[3]);
    strcpy(commands[1], argv[4]);
    strcpy(commands[2], argv[5]);
  }

  for (i = 0; i < (argc - 3); i++)
  {
    if (strcmp(commands[i], "-l") == 0)
      l = true;
    if (strcmp(commands[i], "-a") == 0)
      a = true;
    if (strcmp(commands[i], "-v") == 0)
      v = true;
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
  // (or 0 if parse found errors)
  count = parse(code, list, fplex);

  if (count == 0)
  {
    fprintf(fplex, "Error(s), program is not syntactically correct\n");
    return 0;
  }
	
  for(i; i < MAX_CODE_LENGTH; i++ ){
    code[i].op = 0;
    code[i].r = 0;
    code[i].l = 0;
    code[i].m = 0;
  }
	
  // Printing output
  output(list, count, fplex, l, a, v); // <- change so that this line only executes if parse is successful
  block(current);
	
  i=0;
  while((code[i].op != 0 && code[i].r != 0 && code[i].l !=0 && code[i].m !=0))
  {
    fprintf(fplex, "%d %d %d %d \n", code[i].op, code[i].r, code[i].l, code[i].m)
    i++
  }

  fclose(fpin);
  fclose(fplex);
  return 0;
}
