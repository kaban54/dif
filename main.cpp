#include "dif.h"

FILE *LOG = NULL;

int main()
{
    LOG = fopen (LOGFILENAME, "w");
    if (LOG == nullptr) return 0;
    fprintf (LOG, "<pre>\n");

    Tree_t func_tree = {};
    TreeCtor (&func_tree);

    LoadTree (&func_tree, "testtree.txt");
    TreeDump (&func_tree);

    Tree_t der_tree = {};
    TreeCtor (&der_tree);
    
    GetDerivative (&der_tree, &func_tree, 'x');
    TreeDump (&der_tree);

    SaveTree (&der_tree, "savetree.txt");

    TreeDtor (&func_tree);
    TreeDtor (& der_tree);

    fclose (LOG);
    return 0;
}   