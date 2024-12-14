#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dump.h"

const char *DOT_FILE = "dump/dump.dot";
const char *PNG_FILE = "img/dot.png";

static void create_dot_top(const Top *top, FILE *fp);
static void create_communication(const Top *top, FILE *fp);
static ErrorDifferintiator create_dot_file(const char *filename, const BynarTree *tree);
static ErrorDifferintiator create_png_file(const char *filename, const char *png_file);

static void print_top(ValueTop *value_top, FILE *fp);
static const char* create_str_enum_op(TypeOp op);

ErrorDifferintiator dump(const BynarTree *tree, const char *filename, const char *png_file)
{
    ErrorDifferintiator error = create_dot_file(filename, tree);
    if (error != ERROR_DIFF_NO)
        return error;

    error = create_png_file(filename, png_file);
    return error;
}

static ErrorDifferintiator create_png_file(const char *filename, const char *png_file)
{
    if (png_file == NULL)
        png_file = PNG_FILE;

    if (filename == NULL)
        filename = DOT_FILE;

    int size_str = strlen("dot -Tpng ") + strlen(" -o ") + strlen(filename) + strlen(png_file) + 1;

    char* str = (char*)calloc(size_str, sizeof(char));
    char* ptr = str;

    strcpy(ptr, "dot -Tpng ");
    ptr += strlen("dot -Tpng ");

    strcpy(ptr, filename);
    ptr += strlen(filename);

    ptr = strcpy(ptr, " -o ");
    ptr += strlen(" -o ");

    ptr = strcpy(ptr, png_file);
    ptr += strlen(png_file);

    int res = system(str);
    if (res != 0)
    {
        free(str);
        return ERROR_DIFF_BAD_GRAPHIS;
    }
    free(str);

    return ERROR_DIFF_NO;
}

static ErrorDifferintiator create_dot_file(const char *filename, const BynarTree *tree)
{
    if (filename == NULL)
        filename = DOT_FILE;

    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
        return ERROR_DIFF_OPEN_FILE;

    fprintf(fp, "digraph list{\nrankdir = HR\n");

    if (tree->root != NULL)
        create_dot_top(tree->root, fp);

    fprintf(fp, "edge[color=black]\n");

    if (tree->root != NULL)
        create_communication(tree->root, fp);

    fprintf(fp, "}");
    fclose(fp);

    return ERROR_DIFF_NO;
}

static void create_dot_top(const Top *top, FILE *fp)
{
    int number_node = 0;
    int last_number_node = 0;

    fprintf(fp, "node%p [shape=Mrecord, style=\"filled\",  label = \"", top);
    print_top((ValueTop*)top->element, fp);
    fprintf(fp, "\"]\n");

    if (top->left != NULL)
        create_dot_top(top->left, fp);

    if (top->right != NULL)
        create_dot_top(top->right, fp);
}

static void create_communication(const Top *top, FILE *fp)
{
    if (top->left != NULL)
    {
        fprintf(fp, "node%p->node%p\n", top, top->left);
        create_communication(top->left, fp);
    }
    if (top->right != NULL)
    {
        fprintf(fp, "node%p->node%p\n", top, top->right);
        create_communication(top->right, fp);
    }
}

static void print_top(ValueTop *value_top, FILE *fp)
{
    fprintf(fp, "specific var: %d | var: %d |", value_top->is_have_specific_var, value_top->is_have_var);
    switch (value_top->type)
    {
        case VARIABLE:
            fprintf(fp, "%c", (value_top->value).var);
            break;

        case CONST:
            fprintf(fp, "%lf", (value_top->value).cnst);
            break;

        case OP:
        case FUNC:
            fprintf(fp, "%s", create_str_enum_op((value_top->value).operator_t.number_op));
            break;
    }
}

static const char* create_str_enum_op(TypeOp op)
{
    #define DESCR_(e) \
        case(e):      \
            return #e;\
            break

    switch (op)
    {
        DESCR_(PLUS);
        DESCR_(MINUS);
        DESCR_(MUL);
        DESCR_(DIV);
        DESCR_(DEGREE);
        DESCR_(SQRT);
        DESCR_(SIN);
        DESCR_(COS);
        DESCR_(TG);
        DESCR_(CTG);
        DESCR_(SH);
        DESCR_(CH);
        DESCR_(TH);
        DESCR_(CTH);
        DESCR_(LN);
        DESCR_(LOG);
        DESCR_(EXP);
    }
    return NULL;
    #undef DESCR_
}



