#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print_expression.h"

const char* OpLatex[] = {"+",      "-",     "\\cdot", "\\frac", "^",      "\\sin",  "\\cos",
                         "\\sqrt", "\\tan", "\\cot",  "\\cosh", "\\sinh", "\\tanh", "\\coth",
                         "\\log",  "\\log_", "e^"};


const char* STD_NAME = "dump/diff.tex";

static void write_formula(FILE *fp, const Top* top, const Top* parent_top);
static bool print_bracket(const Top* top, const Top* parent_top);
static ErrorDifferintiator create_pdf_file(const char *filename);

ErrorDifferintiator lateh_expression(BynarTree *tree, const char* filename)
{
    if (filename == NULL)
        filename = STD_NAME;

    FILE *fp = fopen(filename, "w+");
    if (fp == NULL)
        return ERROR_DIFF_OPEN_FILE;

    static bool is_first = true;

    if (is_first)
    {
        fprintf(fp, "\\documentclass{article}\n" "\\usepackage{amsmath}\n");
        is_first = false;
    }

    fprintf(fp, "\\begin{document}\n" "\\begin{equation*}\n");
    write_formula(fp, tree->root, tree->root);
    fprintf(fp, "\n\\end{equation*}\n\\end{document}\n");

    fclose(fp);

    return create_pdf_file(filename);
}

static ErrorDifferintiator create_pdf_file(const char *filename)
{
    const char* COMMAND = "pdflatex ";

    char* buffer = (char*)calloc(strlen(COMMAND) + strlen(filename), sizeof(char));
    if (buffer == NULL)
        return ERROR_DIFF_ALLOCATION;

    char* ptr = buffer;

    strcpy(ptr, COMMAND);
    ptr += strlen(COMMAND);

    strcpy(ptr, filename);

    int res = system(buffer);
    if (res != 0)
    {
        free(buffer);
        return ERROR_DIFF_BAD_PDF;
    }
    free(buffer);

    system("DEL *.log");
    system("DEL *.aux");

    return ERROR_DIFF_NO;
}

static void write_formula(FILE *fp, const Top* top, const Top* parent_top)
{
    if (top == NULL)
        return;

    if ( ((ValueTop*)top->element)->type == CONST)
    {
        fprintf(fp, "%lg ", (((ValueTop*)top->element)->value).cnst);
        return;
    }

    if ( ((ValueTop*)top->element)->type == VARIABLE)
    {
        fprintf(fp, "%c ", (((ValueTop*)top->element)->value).var);
        return;
    }

    bool is_print_bracket = print_bracket(top, parent_top);
    if (is_print_bracket)
        fprintf(fp, "\\left( ");

    int number_op = (((ValueTop*)top->element)->value).operator_t.number_op;

    if ( ((ValueTop*)top->element)->type == OP)
    {
        switch (number_op)
        {
            case PLUS:
            case MINUS:
            case MUL:
                write_formula(fp, top->left, top);
                fprintf(fp, "%s ", OpLatex[number_op]);
                write_formula(fp, top->right, top);
                break;

            case DIV:
                fprintf(fp, "%s{", OpLatex[number_op]);
                write_formula(fp, top->left, top);
                fprintf(fp, "}{");
                write_formula(fp, top->right, top);
                fprintf(fp, "}");
                break;

            case DEGREE:
                write_formula(fp, top->left, top);
                fprintf(fp, "%s{", OpLatex[number_op]);
                write_formula(fp, top->right, top);
                fprintf(fp, "}");
                break;

        }
    }

    if ( ((ValueTop*)top->element)->type == FUNC)
    {
        fprintf(fp, "%s{\\left( ", OpLatex[number_op]);
        write_formula(fp, top->left, top);
        fprintf(fp, "\\right) }");

        if ( (((ValueTop*)top->element)->value).operator_t.number_operand > 1)
        {
            fprintf(fp, "{\\left( ");
            write_formula(fp, top->right, top);
            fprintf(fp, "\\right) }");
        }
    }

    if (is_print_bracket)
        fprintf(fp, "\\right ) ");
}

static bool print_bracket(const Top* top, const Top* parent_top)
{
    if (top == NULL || parent_top == NULL)
        return false;

    if ( ((ValueTop*)top->element)->type != OP )
        return false;

    if ( ((ValueTop*)parent_top->element)->type != OP )
        return false;

    if ( (((ValueTop*)parent_top->element)->value).operator_t.number_op == DIV)
        return false;

    if ( (((ValueTop*)top->element)->value).operator_t.prioritet >
         (((ValueTop*)parent_top->element)->value).operator_t.prioritet)
        return true;

    return false;
}



