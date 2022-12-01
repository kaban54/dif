#include "dif.h"

FILE *LOG = NULL;

int main()
{
    LOG = fopen (LOGFILENAME, "w");
    if (LOG == nullptr) return 0;
    fprintf (LOG, "<pre>\n");

    Tree_t func_tree = {};
    TreeCtor (&func_tree);

    double x0 = 0;

    LoadTree (&func_tree, &x0, "testtree.txt");
    TreeDump (&func_tree);
    fflush (LOG);
    
    GeneratePdf (&func_tree, x0);

    TreeDtor (&func_tree);
    
    fclose (LOG);
    return 0;
}   