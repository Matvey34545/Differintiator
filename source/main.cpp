#include <stdio.h>
#include <stdlib.h>

#include "differentiation.h"
#include "read_expression.h"
#include "print_expression.h"


int main()
{
    BynarTree tree = {};
    create_bynar_tree(&tree, sizeof(ValueTop));

    ErrorDifferintiator error = converts_expression_tree(&tree);
    if (error != ERROR_DIFF_NO)
        return error;

    BynarTree tree1= {};
    create_bynar_tree(&tree1, sizeof(ValueTop));

    diff_and_simplifies_expressions(&tree, &tree1);

    return lateh_expression(&tree1, NULL);
}
