#include "tree.h"

const char *const IMGNUMFILE = "log/imgnum.txt";
const char *const  GRAPHFILE = "log/graph.txt";

const char *const  LEFTCOLOR  = "green";
const char *const RIGHTCOLOR  =   "red";
const char *const PARENTCOLOR = "black";


void TreePrintVal (FILE *stream, TreeElem_t *elem)
{
    if (stream == nullptr || elem == nullptr) return;

    switch (elem -> type)
    {
    case TYPE_OP:
    {
        switch (elem -> value.opval)
        {
        case OP_ADD:
            fprintf (stream, "+");
            break;
            
        case OP_SUB:
            fprintf (stream, "-");
            break;
            
        case OP_MUL:
            fprintf (stream, "*");
            break;
            
        case OP_DIV:
            fprintf (stream, "/");
            break;
        
        case OP_POW:
            fprintf (stream, "^");
            break;
        
        case OP_LN:
            fprintf (stream, "ln");
            break;
        
        case OP_SIN:
            fprintf (stream, "sin");
            break;
        
        case OP_COS:
            fprintf (stream, "cos");
            break;
        
        case OP_TAN:
            fprintf (stream, "tan");
            break;

        default:
            fprintf (stream, "unknown operator");
            break;
        }
        break;
    }
    case TYPE_NUM:
        if (elem -> value.dblval == (int) (elem -> value.dblval)) fprintf (stream, "%d", (int) (elem -> value.dblval));
        else                                                      fprintf (stream, "%lf",       elem -> value.dblval );
        break;

    case TYPE_VAR:
        fprintf (stream, "%c", elem -> value.varval);
        break;

    default:
        fprintf (stream, "unknown type (%d)", elem -> type);
        break;
    }
}

void Tree_txt_dmup (Tree_t *tree, FILE *stream, const char *func_name, const char *file_name, int line)
{
    if (stream == nullptr) stream = stdout;

    if (func_name == nullptr) func_name = "(NULL)";
    if (file_name == nullptr) file_name = "(NULL)";

    fprintf (stream, "\nTree dump from (%s) at (%s) at line (%d):\n", func_name, file_name, line);

    if (tree == nullptr)
    {
        fprintf (stream, "Unknown tree (nullptr).\n");
        return;
    }

    if (tree -> err & TREE_INFO_CORRUPTED) fprintf (stream, "Unknown tree (info corrupted):\n");
    else fprintf (stream, "tree [%p] \"%s\" at (%s) at (%s)(%d):\n", tree, tree -> info.     name, tree -> info.func_name, 
                                                                           tree -> info.file_name, tree -> info.     line);
    fprintf (stream, "{\n");
    fprintf (stream, "\tstatus   = %d\n", tree -> status);
    fprintf (stream, "\terror    = %d\n", tree -> err);
    fprintf (stream, "\tsize     = %d\n", tree -> size);
    fprintf (stream, "\n\t");
    
    if (tree -> err & TREE_DATA_CORRUPTED) fprintf (stream, "data is corrupted");
    else Tree_print_data (stream, tree -> data.left);

    fprintf (stream, "\n}\n");
}

void Tree_print_data (FILE *stream, TreeElem_t *elem)
{
    fprintf (stream, "(");
    if (elem ->  left) Tree_print_data (stream, elem ->  left);
    TreePrintVal (stream, elem);
    if (elem -> right) Tree_print_data (stream, elem -> right);
    fprintf (stream, ")");
}

void Tree_dump (Tree_t *tree, const char *func_name, const char *file_name, int line)
{
    fprintf (LOG, "<pre>\n");
    Tree_txt_dmup (tree, LOG, func_name, file_name, line);

    if (tree != nullptr)
    {
        int imgnum = 0;
        FILE *numfile = fopen (IMGNUMFILE, "r");
        if (numfile != nullptr)
        {
            fscanf (numfile, "%d", &imgnum);
            fclose (numfile);
            numfile = fopen (IMGNUMFILE, "w");
            fprintf (numfile, "%d", imgnum + 1);
            fclose (numfile);
        }
        Tree_generate_img (tree, imgnum);
        fprintf (LOG, "<img src=\"./images/dumpimg%d.png\", width=\"80%%\">", imgnum);
    }

}


void Tree_generate_img (Tree_t *tree, int imgnum)
{
    FILE *graph = fopen (GRAPHFILE, "w");
    if (graph == nullptr) return;

    fprintf (graph, "digraph {\n rankdir = TB;\n"
                    "node [shape = record, fontsize = 12, style = \"rounded, filled\", fillcolor = lightblue];\n"
                    "graph [splines = true];\n");
    
    int size = tree -> size;

    Tree_draw_data (graph, &(tree -> data), 0, &size);

    fprintf (graph, "}");

    fclose (graph);

    char cmd [64] = "";
    sprintf (cmd, "dot -T png -o log/images/dumpimg%d.png %s", imgnum, GRAPHFILE);
    system (cmd);
}

void Tree_draw_data (FILE *graph, TreeElem_t *elem, int rank, int *size)
{
    if (*size < 0 || elem == nullptr) return;

    fprintf (graph, "r%d [style = invis];\n", rank);
    fprintf (graph, "elem%p [label = \"{type = %d|value = ", elem, elem -> type);
    TreePrintVal (graph, elem);

    fprintf (graph, "|{{adress|parent|left|right}|{%p|%p|%p|%p}}}\"", elem, elem -> parent, elem -> left, elem -> right);
    if (rank == 0)
    {
        fprintf (graph, ", color = red, fillcolor = white");
    }
    fprintf (graph, "];\n");
    fprintf (graph, "{rank = same; \"r%d\"; \"elem%p\"}", rank, elem);

    *size -= 1;

    if (elem -> parent)
    {
        fprintf (graph, "elem%p -> elem%p [color = %s, weight = 1];\n", elem, elem -> parent, PARENTCOLOR);
    }

    if (elem -> left)
    {
        fprintf (graph, "elem%p -> elem%p [color = %s, weight = 1];\n", elem, elem -> left, LEFTCOLOR);
        Tree_draw_data (graph, elem -> left, rank + 1, size);
    }
    if (elem -> right)
    {
        fprintf (graph, "elem%p -> elem%p [color = %s, weight = 1];\n", elem, elem -> right, RIGHTCOLOR);
        Tree_draw_data (graph, elem -> right, rank + 1, size);
    }
}
