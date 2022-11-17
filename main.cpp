#include "dif.h"

FILE *LOG = NULL;

int main()
{
    LOG = fopen (LOGFILENAME, "w");
    if (LOG == nullptr) return 0;
    fprintf (LOG, "<pre>\n");

    Tree_t tree = {};
    TreeCtor (&tree);

    LoadTree (&tree, "testtree.txt");
    TreeDump (&tree);
    SaveTree (&tree, "testtree.txt");

    TreeDtor (&tree);

    fclose (LOG);
    return 0;
}   