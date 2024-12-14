#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "read_expression.h"

const char DIRECT_BRACKET = '(';
const char REVERS_BRACKET = ')';
const char SEP            = ',';

static ErrorDifferintiator GetG(BynarTree *tree, ValueTop* value_top);
static ErrorDifferintiator create_array_struct(char *str, ArrayValueTop *array_struct);

static bool is_op(char** str, ValueTop *value_top);
static bool is_standart_op(char** str, InfoOp *info_op);
static bool is_cnst(char** str, ValueTop *value_top);
static char read_variable(char** str);

void skip_spaces(char **str);

const int MAX_PRIORITET = 4;

static ErrorDifferintiator GetOp(Top **top, ValueTop **value_top, int prioritet);
static ErrorDifferintiator GetFunc(Top** top, ValueTop **value_top);
static ErrorDifferintiator GetN(Top** top, ValueTop **value_top);

ErrorDifferintiator converts_expression_tree(BynarTree* tree)
{
    printf("Please, enter the expression.\n");

    const int SIZE_EXPRESSION = 100;
    char expression[SIZE_EXPRESSION] = {};

    if (fgets(expression, SIZE_EXPRESSION, stdin) == NULL)
        return ERROR_DIFF_READ;

    ArrayValueTop array_t = {};
    array_t.ptr = (ValueTop*)calloc(SIZE_EXPRESSION, sizeof(ValueTop));

    ErrorDifferintiator error = create_array_struct(expression, &array_t);
    if (error != ERROR_DIFF_NO)
        return error;

    return GetG(tree, array_t.ptr);
}

Top* CreateTop(TypeValue type, TypeOp op, const void *value, Top* left, Top* right)
{
    ValueTop value_top = {};

    value_top.type = type;

    switch (type)
    {
        case CONST:
            value_top.value.cnst = *(double*)value;
            break;

        case VARIABLE:
            value_top.value.var = *(char*)value;
            break;

        case OP:
        case FUNC:
            memcpy(&value_top.value.operator_t, &(ArrayOp + (int)op)->operator_t, sizeof(Operator));
            break;
    }

    Top* top = NULL;
    create_bynar_top(&value_top, sizeof(ValueTop), &top);

    if (top != NULL)
    {
        top->left  = left;
        top->right = right;
    }
    return top;
}

static ErrorDifferintiator GetG(BynarTree *tree, ValueTop* value_top)
{
    ErrorDifferintiator error = GetOp(&tree->root, &value_top, MAX_PRIORITET);
    if (error != ERROR_DIFF_NO)
        return error;

    if (value_top->type != END)
        return ERROR_DIFF_SYNTAX;

    return ERROR_DIFF_NO;
}

static ErrorDifferintiator GetOp(Top** top, ValueTop **value_top, int prioritet)
{
    Top *left_subtree = NULL;

    if (prioritet < 2)
        return GetFunc(top, value_top);

    ErrorDifferintiator error = GetOp(&left_subtree, value_top, prioritet - 1);
    if (error != ERROR_DIFF_NO)
        return error;

    if ( (*value_top)->type == OP && ((*value_top)->value).operator_t.prioritet == prioritet)
    {
        Top *right_subtree = NULL;

        ErrorBynarTree error_byn = create_bynar_top(*value_top, sizeof(ValueTop), top);
        if (error_byn != BYNAR_OK)
            return ERROR_DIFF_BYNAR_TREE;
        (*value_top)++;

        error = GetOp(&right_subtree, value_top, prioritet);
        if (error != ERROR_DIFF_NO)
            return error;

        (*top)->left  = left_subtree;
        (*top)->right = right_subtree;
    }
    else
        *top = left_subtree;

    return ERROR_DIFF_NO;
}

static ErrorDifferintiator GetFunc(Top** top, ValueTop **value_top)
{
    if ( (*value_top)->type != FUNC )
        return GetN(top, value_top);

    ErrorBynarTree error_byn = create_bynar_top(*value_top, sizeof(ValueTop), top);
    if (error_byn != BYNAR_OK)
        return ERROR_DIFF_BYNAR_TREE;

    int number_parametrs = ((*value_top)->value).operator_t.number_operand;
    (*value_top)++;

    if ( (*value_top)->type != BRACKET )
        return ERROR_DIFF_SYNTAX;

    (*value_top)++;

    ErrorDifferintiator error = GetOp(&(*top)->left, value_top, MAX_PRIORITET);
    if (error != ERROR_DIFF_NO)
        return error;


    if (number_parametrs > 1)
    {
        if ( (*value_top)->type != SEP_FUNC )
            return ERROR_DIFF_SYNTAX;

        (*value_top)++;

        error = GetOp(&(*top)->right, value_top, MAX_PRIORITET);
        if (error != ERROR_DIFF_NO)
            return error;
    }

    if ( (*value_top)->type != BACK_BRACKET )
        return ERROR_DIFF_SYNTAX;

    (*value_top)++;

    return ERROR_DIFF_NO;
}

static ErrorDifferintiator GetN(Top** top, ValueTop **value_top)
{
    if ((*value_top)->type == CONST || (*value_top)->type == VARIABLE)
    {
        ErrorBynarTree error_byn = create_bynar_top(*value_top, sizeof(ValueTop), top);
        if (error_byn != BYNAR_OK)
            return ERROR_DIFF_BYNAR_TREE;

        (*value_top)++;
        return ERROR_DIFF_NO;
    }

    if ((*value_top)->type == BRACKET)
    {
        (*value_top)++;

        ErrorDifferintiator error = GetOp(top, value_top, MAX_PRIORITET);
        if (error != ERROR_DIFF_NO)
            return error;

        if ((*value_top)->type != BACK_BRACKET)
            return ERROR_DIFF_SYNTAX;

        (*value_top)++;
        return ERROR_DIFF_NO;
    }
    return ERROR_DIFF_SYNTAX;
}

static ErrorDifferintiator create_array_struct(char *str, ArrayValueTop *array_struct)
{
    while (*str != '\0')
    {
        if (!is_cnst(&str, array_struct->ptr + array_struct->size))
        {
            if (!is_op(&str, array_struct->ptr + array_struct->size))
            {
                char variable = read_variable(&str);

                if (!isalpha(variable))
                {
                    switch (variable)
                    {
                        case DIRECT_BRACKET:
                            (array_struct->ptr + array_struct->size)->type = BRACKET;
                            break;

                        case REVERS_BRACKET:
                            (array_struct->ptr + array_struct->size)->type = BACK_BRACKET;
                            break;

                        case SEP:
                            (array_struct->ptr + array_struct->size)->type = SEP_FUNC;
                            break;

                        default:
                            return ERROR_DIFF_VARIABLE;
                    }
                    array_struct->size++;
                    skip_spaces(&str);
                    continue;
                }

                (array_struct->ptr + array_struct->size)->type            = VARIABLE;
                ((array_struct->ptr + array_struct->size)->value).var     = variable;
            }
        }
        array_struct->size++;

        skip_spaces(&str);
    }

    (array_struct->ptr + array_struct->size)->type = END;
    return ERROR_DIFF_NO;
}

static bool is_cnst(char** str, ValueTop *value_top)
{
    double temp_const = 0;
    int read_symbol   = 0;

    skip_spaces(str);
    if (!isdigit(**str))
        return false;

    sscanf(*str, "%lf%n", &temp_const, &read_symbol);

    value_top->type         = CONST;
    (value_top->value).cnst = temp_const;

    *str += read_symbol;
    return true;
}

static bool is_op(char** str, ValueTop *value_top)
{
    InfoOp info_op = {};

    if (!is_standart_op(str, &info_op))
        return false;

    if (info_op.is_func)
        value_top->type = FUNC;

    else
        value_top->type = OP;

    memcpy(&((value_top->value).operator_t), &info_op.operator_t, sizeof(Operator));

    return true;
}

static bool is_standart_op(char** str, InfoOp *info_op)
{
    skip_spaces(str);

    for (int i = 0; i < sizeof(ArrayOp) / sizeof(InfoOp); i++)
    {
        if (strncmp(*str, ArrayOp[i].str, ArrayOp[i].size) == 0)
        {
            memcpy(info_op, &ArrayOp[i], sizeof(InfoOp));
            *str += ArrayOp[i].size;
            return true;
        }
    }
    return false;
}

static char read_variable(char** str)
{
    skip_spaces(str);
    (*str)++;

    return *(*str - 1);
}

void skip_spaces(char **str)
{
    while (isspace(**str))
        (*str)++;
}








