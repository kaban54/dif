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
    int taylor_pow = 0;

    LoadTree (&func_tree, &x0, &taylor_pow, "testtree.txt");
    TreeDump (&func_tree);
    fflush (LOG);

    GeneratePdf (&func_tree, x0, taylor_pow);

    TreeDtor (&func_tree);
    
    fclose (LOG);
    return 0;
}   