#ifndef READ_EXPRESSION
#define READ_EXPRESSION

#include "bynar_tree.h"

enum ErrorDifferintiator
{
    ERROR_DIFF_NO          = 0,
    ERROR_DIFF_BRACKET     = 1,
    ERROR_DIFF_VARIABLE    = 2,
    ERROR_DIFF_SYNTAX      = 3,
    ERROR_DIFF_BYNAR_TREE  = 4,
    ERROR_DIFF_BAD_GRAPHIS = 5,
    ERROR_DIFF_OPEN_FILE   = 6,
    ERROR_DIFF_READ        = 7,
    ERROR_DIFF_BAD_PDF     = 8,
    ERROR_DIFF_ALLOCATION  = 9
};

enum TypeOp
{
    PLUS   = 0,
    MINUS  = 1,
    MUL    = 2,
    DIV    = 3,
    DEGREE = 4,
    SIN    = 5,
    COS    = 6,
    SQRT   = 7,
    TG     = 8,
    CTG    = 9,
    CH     = 10,
    SH     = 11,
    TH     = 12,
    CTH    = 13,
    LN     = 14,
    LOG    = 15,
    EXP    = 16
};

enum TypeValue
{
    CONST        = 0,
    OP           = 1,
    VARIABLE     = 2,
    BRACKET      = 3,
    BACK_BRACKET = 4,
    FUNC         = 5,
    SEP_FUNC     = 6,
    END          = 7
};

struct Operator
{
    TypeOp number_op;
    int number_operand;
    int prioritet;
};

union ValueEveryType
{
    double cnst;
    char var;
    Operator operator_t;
};

struct ValueTop
{
    TypeValue type;
    bool is_have_specific_var;
    bool is_have_var;
    ValueEveryType value;
};

struct ArrayValueTop
{
    ValueTop *ptr;
    size_t size;
};

struct InfoOp
{
    const char* str;
    size_t size;

    bool is_func;
    Operator operator_t;
};


const InfoOp ArrayOp[] = {  {"+",    sizeof("+")    - 1, false, {PLUS,   2, 4} },
                            {"-",    sizeof("-")    - 1, false, {MINUS,  2, 4} },
                            {"*",    sizeof("*")    - 1, false, {MUL,    2, 3} },
                            {"/",    sizeof("/")    - 1, false, {DIV,    2, 3} },
                            {"^",    sizeof("^")    - 1, false, {DEGREE, 2, 2} },
                            {"sin",  sizeof("sin")  - 1, true,  {SIN,    1, 1} },
                            {"cos",  sizeof("cos")  - 1, true,  {COS,    1, 1} },
                            {"sqrt", sizeof("sqrt") - 1, true,  {SQRT,   1, 1} },
                            {"tg",   sizeof("tg")   - 1, true,  {TG,     1, 1} },
                            {"ctg",  sizeof("ñtg")  - 1, true,  {CTG,    1, 1} },
                            {"ch",   sizeof("ch")   - 1, true,  {CH,     1, 1} },
                            {"sh",   sizeof("sh")   - 1, true,  {SH,     1, 1} },
                            {"th",   sizeof("th")   - 1, true,  {TH,     1, 1} },
                            {"cth",  sizeof("cth")  - 1, true,  {CTH,    1, 1} },
                            {"ln",   sizeof("ln")   - 1, true,  {LN,     1, 1} },
                            {"log",  sizeof("log")  - 1, true,  {LOG,    2, 1} },
                            {"exp",  sizeof("exp")  - 1, true,  {EXP,    1, 1} }
                           };

Top* CreateTop(TypeValue type, TypeOp op, const void *value, Top* left, Top* right);

ErrorDifferintiator converts_expression_tree(BynarTree* tree);

#endif
