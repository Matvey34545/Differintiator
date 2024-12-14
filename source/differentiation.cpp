#include <string.h>
#include <stdio.h>
#include <math.h>

#include "differentiation.h"

const double DIFF_CONST      = 1.0;
const double DIFF_CONST_NO_X = 0.0;
const double DIFF_FOR_DIV    = 2.0;
const double DIFF_MINUS      = -1.0;

const double EPSILON         = 0.000000001;

static Top* differentiation(const Top *src_top, char var);

static bool simplifies_constants(Top **src_top);
static bool simplifies_expressions(Top** src_top);
static double count_tree(const Top* top);

static Top* copy_tree(const Top *src_top);
static int comparison_double_with_zero(double a, double epsilon);

static bool write_specific_var(char var, Top* top);
static bool write_var(Top* top);

static bool enter_answer();

void diff_and_simplifies_expressions(BynarTree *src_tree, BynarTree *diff_tree)
{
    printf("Which variable do you want to differentiate by?\n");

    char var = '\0';
    scanf("%c", &var);
    getchar();

    write_specific_var(var, src_tree->root);

    diff_tree->root = differentiation(src_tree->root, var);

    printf("Do you want to simplify the answer?\n"
           "If you want to simplify, enter YES, if you don't want to enter NO\n");

    if (!enter_answer())
        return;

    bool simplifies = true;

    while (simplifies)
    {
        write_var(diff_tree->root);
        simplifies = simplifies_expressions(&diff_tree->root);

        if (simplifies)
        {
            write_var(diff_tree->root);
            simplifies = simplifies_constants(&diff_tree->root);
        }
    }
}

static bool enter_answer()
{
    const int SIZE_ANSWER    = 100;
    char answer[SIZE_ANSWER] = {};

    if (fgets(answer, SIZE_ANSWER - 1, stdin) == NULL)
        return false;

    answer[strlen(answer) - 1] = '\0';

    while (1)
    {
        if (strcmp(answer, "YES") == 0)
            return true;

        if (strcmp(answer, "NO") == 0)
            return false;

        printf("Please enter \"YES\" or \"NO\"\n");

        if (fgets(answer, SIZE_ANSWER - 1, stdin) == NULL)
            return false;
        answer[strlen(answer) - 1] = '\0';
    }
    return false;
}

static bool write_specific_var(char var, Top* top)
{
    if (top == NULL)
        return false;

    bool is_have_var_left  = write_specific_var(var, top->left);
    bool is_have_var_right = write_specific_var(var, top->right);
    bool is_have_top = false;

    if ( (((ValueTop*)top->element)->value).var == var && ((ValueTop*)top->element)->type == VARIABLE)
        is_have_top = true;

    ((ValueTop*)top->element)->is_have_specific_var = is_have_top || is_have_var_left || is_have_var_right;

    return is_have_top || is_have_var_left || is_have_var_right;
}

static bool write_var(Top* top)
{
    if (top == NULL)
        return false;

    bool is_have_var_left  = write_var(top->left);
    bool is_have_var_right = write_var(top->right);
    bool is_have_top = false;

    if ( ((ValueTop*)top->element)->type == VARIABLE )
        is_have_top = true;

    ((ValueTop*)top->element)->is_have_var = is_have_top || is_have_var_left || is_have_var_right;

    return is_have_top || is_have_var_left || is_have_var_right;
}

static Top* differentiation(const Top *src_top, char var)
{
    if (src_top == NULL)
        return NULL;

    if (((ValueTop*)src_top->element)->type == VARIABLE || ((ValueTop*)src_top->element)->type == CONST)
    {
        if (((ValueTop*)src_top->element)->type == VARIABLE && (((ValueTop*)src_top->element)->value).var == var)
            return _NUM(&DIFF_CONST);

        else
            return _NUM(&DIFF_CONST_NO_X);
    }

    Top* diff_top = NULL;

    switch ( (((ValueTop*)(src_top->element))->value).operator_t.number_op )
    {
        case PLUS:
            diff_top =  _PLUS(dL(src_top, var), dR(src_top, var));
            break;

        case MINUS:
            diff_top =  _MINUS(dL(src_top, var), dR(src_top, var));
            break;

        case MUL:
            diff_top =  _PLUS(_MUL(dL(src_top, var), cR(src_top)), _MUL(cL(src_top), dR(src_top, var)));
            break;

        case DIV:
            diff_top = _DIV(_MINUS(_MUL(dL(src_top, var), cR(src_top)), _MUL(cL(src_top), dR(src_top, var)))
                            , _DEGREE(cR(src_top), _NUM(&DIFF_FOR_DIV)));
            break;

        case SIN:
            diff_top = _MUL(dL(src_top, var), _COS(cL(src_top)));
            break;

        case COS:
            diff_top = _MUL(dL(src_top, var), _MUL(_NUM(&DIFF_MINUS), _SIN(cL(src_top))));
            break;

        case TG:
            diff_top = _DIV(dL(src_top, var), _DEGREE(_COS(cL(src_top)), _NUM(&DIFF_FOR_DIV)));
            break;

        case CTG:
            diff_top = _DIV(_MUL(_NUM(&DIFF_MINUS), dL(src_top, var)), _DEGREE(_SIN(cL(src_top)), _NUM(&DIFF_FOR_DIV)));
            break;

        case SQRT:
            diff_top = _DIV(dL(src_top, var), _MUL(_NUM(&DIFF_FOR_DIV), _SQRT(cL(src_top))));
            break;

        case SH:
            diff_top = _MUL(dL(src_top, var), _CH(cL(src_top)));
            break;

        case CH:
            diff_top = _MUL(dL(src_top, var), _SH(cL(src_top)));
            break;

        case TH:
            diff_top = _DIV(dL(src_top, var), _DEGREE(_CH(cL(src_top)), _NUM(&DIFF_FOR_DIV)));
            break;

        case CTH:
            diff_top = _DIV(_MUL(_NUM(&DIFF_MINUS), dL(src_top, var)), _DEGREE(_SH(cL(src_top)), _NUM(&DIFF_FOR_DIV)));
            break;

        case LN:
            diff_top = _DIV(dL(src_top, var), cL(src_top));
            break;

        case EXP:
            diff_top = _MUL(dL(src_top, var), copy_tree(src_top));
            break;

        case DEGREE:
            if ( ((ValueTop*)src_top->element)->is_have_specific_var)
                diff_top = _NUM(&DIFF_CONST_NO_X);

            if ( ((ValueTop*)src_top->left->element)->is_have_specific_var && !((ValueTop*)src_top->right->element)->is_have_specific_var)
                diff_top = _MUL(dL(src_top, var), _MUL(cR(src_top), _DEGREE(cL(src_top), _MINUS(cR(src_top), _NUM(&DIFF_CONST)))));

            if ( !((ValueTop*)src_top->left->element)->is_have_specific_var && ((ValueTop*)src_top->right->element)->is_have_specific_var)
                diff_top =_MUL(_LN(cL(src_top)), copy_tree(src_top));

            if ( ((ValueTop*)src_top->left->element)->is_have_specific_var && ((ValueTop*)src_top->right->element)->is_have_specific_var)
                diff_top = _MUL(_PLUS(_DIV(_MUL(dL(src_top, var), cR(src_top)), cL(src_top)), _MUL(dR(src_top, var), _LN(cL(src_top)))),
                           _EXP(_MUL(_LN(cL(src_top)), cR(src_top))));
            break;

    }
    return diff_top;
}

static bool simplifies_expressions(Top** src_top)
{
    if (*src_top == NULL)
        return false;

    if ( ((ValueTop*)(*src_top)->element)->type == CONST || ((ValueTop*)(*src_top)->element)->type == VARIABLE)
        return false;

    bool simplifies_left  = simplifies_expressions(&(*src_top)->left);
    bool simplifies_right = simplifies_expressions(&(*src_top)->right);

    TypeOp op            = (((ValueTop*)(*src_top)->element)->value).operator_t.number_op;
    TypeValue type_left  = VARIABLE;
    TypeValue type_right = VARIABLE;
    double const_left    = 0;
    double const_right   = 0;

    if ((*src_top)->left != NULL)
    {
        type_left  = ((ValueTop*)(*src_top)->left->element)->type;
        const_left    = (((ValueTop*)(*src_top)->left->element)->value).cnst;
    }
    if ((*src_top)->right != NULL)
    {
        type_right = ((ValueTop*)(*src_top)->right->element)->type;
        const_right   = (((ValueTop*)(*src_top)->right->element)->value).cnst;
    }

    Top* new_subtree = *src_top;
    bool simplifies  = false;


    switch (op)
    {
        case PLUS:
            if (comparison_double_with_zero(const_left, EPSILON) == 0 && type_left == CONST)
            {
                simplifies = true;
                new_subtree = cR(*src_top);
            }
            if (comparison_double_with_zero(const_right, EPSILON) == 0 && type_right == CONST)
            {
                simplifies = true;
                new_subtree = cL(*src_top);
            }
            break;

        case MINUS:
            if (comparison_double_with_zero(const_right, EPSILON) == 0 && type_right == CONST)
            {
                simplifies = true;
                new_subtree = cL(*src_top);
            }
            if (comparison_double_with_zero(const_left, EPSILON) == 0 && type_left == CONST)
            {
                simplifies = true;
                new_subtree = _MUL(cR(*src_top), _NUM(&DIFF_MINUS));
            }
            break;

        case MUL:
            if ( (comparison_double_with_zero(const_right, EPSILON) == 0 && type_right == CONST) ||
                 (comparison_double_with_zero(const_left, EPSILON)  == 0 && type_left  == CONST)      )
            {
                simplifies = true;
                new_subtree = _NUM(&DIFF_CONST_NO_X);
            }
            if (comparison_double_with_zero(const_right - 1, EPSILON) == 0 && type_right == CONST)
            {
                simplifies = true;
                new_subtree = cL(*src_top);
            }
            if (comparison_double_with_zero(const_left - 1, EPSILON) == 0 && type_left == CONST)
            {
                simplifies = true;
                new_subtree = cR(*src_top);
            }
            break;

        case DIV:
            if (comparison_double_with_zero(const_left, EPSILON) == 0 && type_left == CONST)
            {
                simplifies = true;
                new_subtree = _NUM(&DIFF_CONST_NO_X);
            }
            if (comparison_double_with_zero(const_right - 1, EPSILON) == 0 && type_right == CONST)
            {
                simplifies = true;
                new_subtree = cL(*src_top);
            }
            break;

        case DEGREE:
            if (comparison_double_with_zero(const_left, EPSILON) == 0 && type_left == CONST)
            {
                simplifies = true;
                new_subtree = _NUM(&DIFF_CONST_NO_X);
            }
            if (comparison_double_with_zero(const_left - 1, EPSILON) == 0 && type_left == CONST)
            {
                simplifies = true;
                new_subtree = _NUM(&DIFF_CONST);
            }
            if (comparison_double_with_zero(const_right, EPSILON) == 0 && type_right == CONST)
            {
                simplifies = true;
                new_subtree = _NUM(&DIFF_CONST);
            }
            if (comparison_double_with_zero(const_right - 1, EPSILON) == 0 && type_right == CONST)
            {
                simplifies = true;
                new_subtree = cL(*src_top);
            }
            break;
    }

    if (simplifies)
        dtor_tree(*src_top);

    *src_top = new_subtree;

    return simplifies_left || simplifies_right || simplifies;
}

static bool simplifies_constants(Top **src_top)
{
    if (*src_top == NULL)
        return false;

    bool simplifies_left  = false;
    bool simplifies_right = false;

    if ( ((ValueTop*)(*src_top)->element)->is_have_var)
    {
        simplifies_left  = simplifies_constants(&(*src_top)->left);
        simplifies_right = simplifies_constants(&(*src_top)->right);

        return simplifies_left || simplifies_right;
    }

    double number = count_tree(*src_top);

    Top* new_subtree = _NUM(&number);
    dtor_tree(*src_top);
    *src_top = new_subtree;

    return true;
}

static double count_tree(const Top* top)
{
    if (top == NULL)
        return 0;

    if ( ((ValueTop*)top->element)->type == CONST)
        return (((ValueTop*)top->element)->value).cnst;

    double left  = count_tree(top->left);
    double right = count_tree(top->right);


    switch ( (((ValueTop*)top->element)->value).operator_t.number_op )
    {
        case PLUS:
            return left + right;

        case MINUS:
            return left - right;

        case MUL:
            return left * right;

        case DIV:
            return left / right;

        case DEGREE:
            return pow(left, right);

        case SQRT:
            return pow(left, 0.5);

        case SIN:
            return sin(left);

        case COS:
            return cos(left);

        case TG:
            return tan(left);

        case CTG:
            return cos(left)/sin(left);

        case SH:
            return sinh(left);

        case CH:
            return cosh(left);

        case TH:
            return tanh(left);

        case CTH:
            return cosh(left)/sinh(left);

        case LN:
            return log(left);

        case LOG:
            return log(right)/log(left);

        case EXP:
            return exp(left);
    }
    return -1;
}


static int comparison_double_with_zero(double a, double epsilon)
{
    if (a > epsilon)
        return -1;

    if (a < epsilon && a > -epsilon)
        return 0;

    return -1;
}

static Top* copy_tree(const Top *src_top)
{
    if (src_top == NULL)
        return NULL;


    Top* desrc_top = NULL;
    create_bynar_top(src_top->element, sizeof(ValueTop), &desrc_top);

    if (desrc_top != NULL)
    {
        desrc_top->left  = copy_tree(src_top->left);
        desrc_top->right = copy_tree(src_top->right);
    }
    return desrc_top;
}





