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

    GeneratePdf (&func_tree);

    TreeDtor (&func_tree);
    
    fclose (LOG);
    return 0;
}   