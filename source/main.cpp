#include <stdio.h>
#include <stdlib.h>

#include "differentiation.h"
#include "read_expression.h"
#include "dump.h"

int main()
{
    BynarTree tree = {};
    create_bynar_tree(&tree, sizeof(ValueTop));

    converts_expression_tree(&tree);

    BynarTree tree1= {};
    create_bynar_tree(&tree1, sizeof(ValueTop));

    diff_and_simplifies_expressions(&tree, &tree1);
    dump(&tree1, NULL, NULL);
}
