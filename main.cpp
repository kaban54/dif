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
    char *settings = LoadGnuplotSettings (PLOTSETTINGS);

    TreeDump (&func_tree);
    fflush (LOG);

    GeneratePdf (&func_tree, x0, taylor_pow, settings);

    TreeDtor (&func_tree);
    free (settings);

    fclose (LOG);
    return 0;
}   