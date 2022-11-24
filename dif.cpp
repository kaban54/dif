#include "dif.h"


int LoadTree (Tree_t *tree, const char *filename)
{
    TreeVerify (tree);
    if (filename == nullptr) TreeErr (tree, TREE_NULLPTR_ARG);

    if (tree -> data.left || tree -> data.right) TreeErr (tree, TREE_INCORRECT_POSITION);

    TreeAddLeft (tree, &(tree -> data), TreeAllocElem());
    if (tree -> data.left == nullptr) TreeErr (tree, TREE_ALLOC_ERROR);

    FILE *file = fopen (filename, "r");
    if (file == nullptr) TreeErr (tree, TREE_NULLPTR_ARG);

    tree -> size = 0;
    Read_tree (file, tree -> data.left, &(tree -> size));

    fclose (file);
    TreeVerify (tree);
    return TREE_OK;
}

int Read_tree (FILE *file, TreeElem_t *elem, int *size)
{
    int ch = SkipSpaces (file);
    if (ch != '(') return TREE_INCORRECT_FORMAT;

    *size += 1;

    ch = SkipSpaces (file);
    ungetc (ch, file);

    int err = TREE_OK;

    if (ch == '(')
    {
        L = TreeAllocElem ();
        R = TreeAllocElem ();
        if (L == nullptr) return TREE_ALLOC_ERROR;
        if (R == nullptr) return TREE_ALLOC_ERROR;
        
        L -> parent = elem;
        R -> parent = elem;

        err |= Read_tree (file, L, size);
        if (err) return err;

        err |= Read_op (file, elem);
        if (err) return err;

        err |= Read_tree (file, R, size);
        if (err) return err;
    }
    else
    {
        err |= Read_value (file, elem);
        if (err) return err;
    }

    ch = SkipSpaces (file);
    if (ch != ')') return TREE_INCORRECT_FORMAT;

    return TREE_OK;
}

int SkipSpaces (FILE *file)
{
    if (file == nullptr) return EOF;

    int ch = fgetc (file);

    while (isspace (ch)) ch = fgetc (file);

    return ch;
}

int Read_op (FILE *file, TreeElem_t *elem)
{
    int ch = SkipSpaces (file);

    switch (ch)
    {
    case '+':
        OP = OP_ADD;
        break;

    case '-':
        OP = OP_SUB;
        break;

    case '*':
        OP = OP_MUL;
        break;

    case '/':
        OP = OP_DIV;
        break;
    
    case '^':
        OP = OP_POW;
        break;
    
    case 'l':
        OP = OP_LN;
        break;
    
    case 's':
        OP = OP_SIN;
        break;

    case 'c':
        OP = OP_COS;
        break;

    default:
        return TREE_INCORRECT_FORMAT;
    }
    TYPE = TYPE_OP;
    return TREE_OK;
}

int Read_value (FILE *file, TreeElem_t *elem)
{
    int ch = SkipSpaces (file);

    if (isdigit (ch))
    {
        ungetc (ch, file);
        fscanf (file, "%lf", &(VAL));
        TYPE = TYPE_NUM;
    }
    else if (isalpha (ch))
    {
        elem -> value.varval = (char) ch;
        TYPE = TYPE_VAR;
    }
    else return TREE_INCORRECT_FORMAT;

    return TREE_OK;
}


int SaveTree (Tree_t *tree, const char *filename)
{
    TreeVerify (tree);
    if (filename == nullptr) TreeErr (tree, TREE_NULLPTR_ARG);

    FILE *file = fopen (filename, "w");
    if (file == nullptr) TreeErr (tree, TREE_NULLPTR_ARG);

    Print_tree (file, tree -> data.left);

    fclose (file);
    return TREE_OK;
}

void Print_tree (FILE *file, TreeElem_t *elem)
{
    fputc ('(', file);

    if (L) Print_tree (file, L);

    TreePrintVal (file, elem);

    if (R) Print_tree (file, R);

    fputc (')', file);
}

int Tree_get_size (TreeElem_t *elem)
{
    if (elem == nullptr) return 0;

    int size = 1;

    if (L) size += Tree_get_size (L);
    if (R) size += Tree_get_size (R);

    return size;
}


void GeneratePdf (Tree_t *func_tree)
{
    FILE *texfile = fopen (TEXFILENAME, "w");
    if (texfile == nullptr) return;
    Print_tex_top (texfile);

    Tree_t der_tree = {};
    TreeCtor (&der_tree);

    func_tree -> data.left = Simplify (func_tree -> data.left, texfile);
    func_tree -> size = Tree_get_size (func_tree -> data.left);

    GetDerivative (&der_tree, func_tree, 'x', texfile);
    TreeDump (&der_tree);

    SaveTree (&der_tree, "savetree.txt");

    Print_tex_bottom (texfile);
    fclose (texfile);

    TreeDtor (& der_tree);

    char cmd [BUFSIZE] = "";
    sprintf (cmd, "pdflatex -output-directory ./tex %s", TEXFILENAME);
    system (cmd);
}


int GetDerivative (Tree_t *der_tree, Tree_t *func_tree, char var, FILE *texfile)
{
    if (texfile == nullptr) return TREE_NULLPTR_ARG;
    TreeVerify ( der_tree);
    TreeVerify (func_tree);

    TreeElem_t *der = diff (func_tree -> data.left, var, texfile);
    der_tree -> size = Tree_get_size (der);

    der -> parent = &(der_tree -> data);
    der_tree -> data.left = der;

    TreeVerify (der_tree);
    return TREE_OK;
}

TreeElem_t *diff (TreeElem_t *elem, char var, FILE *texfile)
{
    TreeElem_t *der = nullptr;

    Print_before_diff (texfile, elem);

    switch (TYPE)
    {
    case TYPE_NUM:
        der = NUM (0);
        break;

    case TYPE_VAR:
        if (elem -> value.varval == var) der = NUM (1);
        else                             der = NUM (0);
        break;

    case TYPE_OP:
        der = diff_op (elem, var, texfile);
        break;

    default:
        return nullptr;
    }

    Print_after_diff (texfile, der);
    der = Simplify (der, texfile);

    return der;
}

TreeElem_t *diff_op (TreeElem_t *elem, char var, FILE *texfile)
{
    int var_l = FindVar (L, var);
    int var_r = FindVar (R, var);

    if (!var_l && !var_r) return NUM (0);

    switch (OP)
    {
    case OP_ADD:
        return ADD (dL, dR);

    case OP_SUB:
        return SUB (dL, dR);

    case OP_MUL:
        return diff_mul (elem, var, var_l, var_r, texfile);

    case OP_DIV:
        return diff_div (elem, var, var_l, var_r, texfile);

    case OP_POW:
        return diff_pow (elem, var, var_l, var_r, texfile);
        
    case OP_SIN:
        return MUL (COS (cR), dR);
        
    case OP_COS:
        return MUL (NUM (-1), MUL (SIN (cR), dR));

    case OP_TAN:
        return DIV (dR, POW (COS (cR), NUM (2)));

    default:
        return nullptr;
    }
}


TreeElem_t *diff_mul (TreeElem_t *elem, char var, int var_l, int var_r, FILE *texfile)
{
    if (var_l && var_r) return ADD (MUL (dL, cR), MUL (cL, dR));
    else if (var_l)     return MUL (cR, dL);
    else                return MUL (cL, dR);
}

TreeElem_t *diff_div (TreeElem_t *elem, char var, int var_l, int var_r, FILE *texfile)
{
    if (var_l && var_r) return DIV (SUB (MUL (dL, cR), MUL (cL, dR)), POW (cR, NUM (2)));
    // (f / g)' = (f'g - fg') / g^2
    else if (var_l)     return DIV (dL, cR);
    // (f / c)' = f' / c
    else                return MUL (cL, MUL (NUM (-1), DIV (dR, POW (cR, NUM (2)))));
    // (c / f)' = c * (- f' / f^2)
}

TreeElem_t *diff_pow (TreeElem_t *elem, char var, int var_l, int var_r, FILE *texfile)
{
    if (var_l && var_r) return MUL (POW (cL, cR), ADD (MUL (cR, DIV (dL, cL)), MUL (LN (cL), dR))); 
    // (f^g)' = f^g * (g * f' / f + ln(f) * g')
    else if (var_l)     return MUL (cR, MUL (POW (cL, SUB (cR, NUM (1))), dL));
    // (f^c)' = c * f^(c - 1) * f'
    else return MUL (LN (cL), MUL (POW (cL, cR), dR));
    // (c^f)' = ln(c) * c^f * f'
}


TreeElem_t *copy (TreeElem_t *elem)
{
    TreeElem_t *elem_cpy = TreeAllocElem ();
    if (elem_cpy == nullptr) return nullptr;

    elem_cpy -> type  = TYPE;
    elem_cpy -> value = elem -> value;

    if (L)
    {
        elem_cpy -> left = copy (L);
        elem_cpy -> left -> parent = elem_cpy;
    }
    if (R)
    {
        elem_cpy -> right = copy (R);
        elem_cpy -> right -> parent = elem_cpy;
    }
    return elem_cpy;
}

TreeElem_t *CreateNum (double num)
{
    TreeElem_t *node = TreeAllocElem ();
    if (node == nullptr) return nullptr;

    node -> type = TYPE_NUM;
    node -> value.dblval = num;
    return node;
}

TreeElem_t *CreateOp (int op, TreeElem_t *left, TreeElem_t *right)
{
    TreeElem_t *elem = TreeAllocElem ();
    if (elem == nullptr) return nullptr;

    TYPE = TYPE_OP;
    OP = op;
    L  = left;
    R = right;

    LP = elem;
    RP = elem;

    return elem;
}


int FindVar (TreeElem_t *node, char var)
{
    if (node == nullptr) return 0;

    if (node -> type == TYPE_VAR && (node -> value.varval == var || var == 0)) return 1;
    if (node -> type == TYPE_OP) return FindVar (node -> left, var) | FindVar (node -> right, var);

    return 0;
}


TreeElem_t *Simplify (TreeElem_t *elem, FILE *texfile)
{
    Print_before_simplify (texfile, elem);

    int size = Tree_get_size (elem);
    int old_size = size;

    elem = CalculateConsts (elem, &size);
    elem = RemoveNeutrals  (elem, &size);

    while (size < old_size)
    {
        old_size = size;
        elem = CalculateConsts (elem, &size);
        elem = RemoveNeutrals  (elem, &size);
    }

    Print_after_simplify (texfile, elem);

    return elem;
}

TreeElem_t *CalculateConsts (TreeElem_t *elem, int *size)
{
    if (TYPE == TYPE_NUM || TYPE == TYPE_VAR) return elem;

    L = CalculateConsts (L, size);
    R = CalculateConsts (R, size);

    if (LTYPE == TYPE_NUM && RTYPE == TYPE_NUM)
    {
        elem = Calculate (elem);
        (*size) -= 2;
    }
    // Print to tex ?
    return elem;
}

TreeElem_t *Calculate (TreeElem_t *elem)
{
    switch (OP)
    {
    case OP_ADD:
        VAL = LVAL + RVAL;
        break;
    
    case OP_SUB:
        VAL = LVAL - RVAL;
        break;
    
    case OP_MUL:
        VAL = LVAL * RVAL;
        break;
    
    case OP_DIV:
        if (RVAL == 0)
        {
            printf ("Division by zero in (%s) at line (%d)", __PRETTY_FUNCTION__, __LINE__);
            return elem;
        }
        VAL = LVAL / RVAL;
        break;

    case OP_POW:
        if (LVAL < 0 && RVAL != (int) RVAL) 
        {
            printf ("Root of negative in (%s) at line (%d)", __PRETTY_FUNCTION__, __LINE__);
            return elem;
        }
        if (LVAL == 0 && RVAL < 0)
        {
            printf ("Negative power of zero in (%s) at line (%d)", __PRETTY_FUNCTION__, __LINE__);
            return elem;
        }
        VAL = pow (LVAL, RVAL);
        break;

    case OP_LN:
        if (RVAL <= 0)
        {
            printf ("Log of negative in (%s) at line (%d)", __PRETTY_FUNCTION__, __LINE__);
            return elem;
        }
        VAL = log (RVAL);
        break;
    
    case OP_SIN:
        VAL = sin (RVAL);
        break;
    
    case OP_COS:
        VAL = cos (RVAL);
        break;

    case OP_TAN:
        VAL = tan (RVAL);
        break;

    default:
        return elem;
    }

    TYPE = TYPE_NUM;
    free (L);
    free (R);
    L = nullptr;
    R = nullptr;

    return elem;
}


TreeElem_t *RemoveNeutrals (TreeElem_t *elem, int *size)
{
    if (TYPE == TYPE_NUM || TYPE == TYPE_VAR) return elem;

    L = RemoveNeutrals (L, size);
    R = RemoveNeutrals (R, size);

    if (LTYPE != TYPE_NUM && RTYPE != TYPE_NUM) return elem;

    elem = Remove_neutrals (elem, size);

    // Print to tex ?
    return elem;
}

TreeElem_t *Remove_neutrals (TreeElem_t *elem, int *size)
{
    switch (OP)
    {
    case OP_ADD:
        if      (LIS0) elem = Replace_with_right (elem, size);
        else if (RIS0) elem = Replace_with_left  (elem, size);
        break;
    
    case OP_SUB:
        if      (RIS0) elem = Replace_with_left (elem, size);
        else if (LIS0)
        {
            LVAL = -1;
            OP = OP_MUL;
        }
        break;
    
    case OP_MUL:
        if (LIS0 || RIS0) elem = Replace_with_num   (elem, size, 0);
        else if (LIS1)    elem = Replace_with_right (elem, size);
        else if (RIS1)    elem = Replace_with_left  (elem, size);
        break;

    case OP_DIV:
        if      (RIS0) 
        {
            printf ("Division by zero in (%s) at line (%d)", __PRETTY_FUNCTION__, __LINE__);
            return elem;
        }
        if      (RIS1) elem = Replace_with_left (elem, size);
        else if (LIS0) elem = Replace_with_num  (elem, size, 0);
        break;

    case OP_POW:
        if      (RIS0) elem = Replace_with_num  (elem, size, 1);
        else if (LIS1) elem = Replace_with_num  (elem, size, 1);
        else if (RIS1) elem = Replace_with_left (elem, size);
        else if (LIS0)
        {
            if (RTYPE == TYPE_NUM && RVAL < 0)
            {
                printf ("Negative power of zero in (%s) at line (%d)", __PRETTY_FUNCTION__, __LINE__);
                return elem;
            }
            else elem = Replace_with_num (elem, size, 0);
        }
        break;

    default:
        break;
    }
    return elem;
}
/*
TreeElem_t *Remove_neutrals_add (TreeElem_t *elem, int *size)
{

}

TreeElem_t *Remove_neutrals_sub (TreeElem_t *elem, int *size)
{

}

TreeElem_t *Remove_neutrals_mul (TreeElem_t *elem, int *size)
{

}

TreeElem_t *Remove_neutrals_div (TreeElem_t *elem, int *size)
{

}

TreeElem_t *Remove_neutrals_pow (TreeElem_t *elem, int *size)
{

}
*/
TreeElem_t *Replace_with_left (TreeElem_t *elem, int *size)
{
    free (R);
    LP = P;

    if (P)
    {
        if (PL == elem) PL = L;
        else            PR = L;
    }

    TreeElem_t *ret = L;
    free (elem);
    *size -= 2;
    return ret;
}

TreeElem_t *Replace_with_right (TreeElem_t *elem, int *size)
{
    free (L);
    RP = P;

    if (P)
    {
        if (PL == elem) PL = R;
        else            PR = R;
    }

    TreeElem_t *ret = L;
    free (elem);
    *size -= 2;
    return ret;
}

TreeElem_t *Replace_with_num (TreeElem_t *elem, int *size, double num)
{
    free (L);
    free (R);

    L = nullptr;
    R = nullptr;

    TYPE = TYPE_NUM;
    VAL = num;

    *size -= 2;
    return elem;
}



int CompareTrees (TreeElem_t *elem1, TreeElem_t *elem2)
{
    if (elem1 == nullptr || elem2 == nullptr) return 0;
    if (elem1 -> type != elem2 -> type) return 0;

    if (elem1 -> type == TYPE_OP)
    {
        if (elem1 -> value.opval != elem2 -> value.opval) return 0;
        if (CompareTrees (elem1 -> left, elem2 ->  left) && CompareTrees (elem1 -> right, elem2 -> right)) return 1;
        if (!OpCommutative (elem1 -> value.opval)) return 0;
        if (CompareTrees (elem1 -> left, elem2 -> right) && CompareTrees (elem1 -> right, elem2 ->  left)) return 1;
    }
    else if (elem1 -> type == TYPE_NUM) return (elem1 -> value.dblval == elem2 -> value.dblval);
    else if (elem1 -> type == TYPE_VAR) return (elem1 -> value.varval == elem2 -> value.varval);
    return 0;
}

int OpCommutative (int op)
{
    return (op == OP_ADD || op == OP_MUL);
}


void Print_tex_top (FILE *texfile)
{
    fprintf (texfile, "\\documentclass[12pt,a4paper,fleqn]{article}\n"
                      "\\usepackage[utf8]{inputenc}\n"
                      "\\usepackage[russian]{babel}\n");
    fprintf (texfile, "\\begin{document}\nHEADER\\\\\\\\");
}

void Print_tex_bottom (FILE *texfile)
{
    fprintf (texfile, "\\end{document}");
}



void Print_before_diff (FILE *texfile, TreeElem_t *elem)
{
    fprintf (texfile, "Я РУССКИЙ!!! Before diff:\\\\\n");
    PrintTreeTex (texfile, elem);
    fprintf (texfile, "\\\\\n");
}

void Print_after_diff (FILE *texfile, TreeElem_t *elem)
{
    fprintf (texfile, "After diff:\\\\\n");
    PrintTreeTex (texfile, elem);
    fprintf (texfile, "\\\\\n");
}

void Print_before_simplify (FILE *texfile, TreeElem_t *elem)
{
    fprintf (texfile, "Before simplify:\\\\\n");
    PrintTreeTex (texfile, elem);
    fprintf (texfile, "\\\\\n");
}

void Print_after_simplify (FILE *texfile, TreeElem_t *elem)
{
    fprintf (texfile, "After simplify:\\\\\n");
    PrintTreeTex (texfile, elem);
    fprintf (texfile, "\\\\\n");
}



void PrintTreeTex (FILE *texfile, TreeElem_t *elem)
{
    if (texfile == nullptr || elem == nullptr) return;

    fprintf (texfile, "$");
    
    if (TYPE == TYPE_OP) Print_tree_tex (texfile, elem);
    else                 TreePrintVal   (texfile, elem);

    fprintf (texfile, "$");
}

void Print_tree_tex (FILE *texfile, TreeElem_t *elem)
{
    if      (OP == OP_DIV) Print_frac_tex (texfile, elem);
    else if (OP == OP_POW) Print_pow_tex  (texfile, elem);
    else
    {
        if (!OneArgOp (OP))
        {
            if (LTYPE == TYPE_OP)
                {
                    if (OPRANK > LOPRANK) fprintf (texfile, " ( ");
                    Print_tree_tex (texfile, L);
                    if (OPRANK > LOPRANK) fprintf (texfile, " ) ");
                }
            else TreePrintVal (texfile, L);
        }

        PrintOpTex (texfile, OP);

        if (RTYPE == TYPE_OP)
        {
            if (OPRANK > ROPRANK) fprintf (texfile, " ( ");
            Print_tree_tex (texfile, R);
            if (OPRANK > ROPRANK) fprintf (texfile, " ) ");
        }
        else TreePrintVal (texfile, R);
    }
}

void Print_frac_tex (FILE *texfile, TreeElem_t *elem)
{
    fprintf (texfile, " \\frac{");

    if (LTYPE == TYPE_OP) Print_tree_tex (texfile, L);
    else                  TreePrintVal   (texfile, L);

    fprintf (texfile, "}{");

    if (RTYPE == TYPE_OP) Print_tree_tex (texfile, R);
    else                  TreePrintVal   (texfile, R);

    fprintf (texfile, "} ");
}

void Print_pow_tex (FILE *texfile, TreeElem_t *elem)
{
    if (LTYPE == TYPE_OP)
    {
        fprintf (texfile, " ( ");
        Print_tree_tex (texfile, L);
        fprintf (texfile, " ) ");
    }
    else TreePrintVal (texfile, L);

    fprintf (texfile, "^{");

    if (RTYPE == TYPE_OP) Print_tree_tex (texfile, R);
    else                  TreePrintVal   (texfile, R);

    fprintf (texfile, "} ");
}

void PrintOpTex (FILE *texfile, int op)
{
    switch (op)
    {
    case OP_ADD:
        fprintf (texfile, " + ");
        break;
    case OP_SUB:
        fprintf (texfile, " - ");
        break;
    case OP_MUL:
        fprintf (texfile, " \\times ");
        break;
    case OP_LN:
        fprintf (texfile, " \\ln ");
        break;
    case OP_SIN:
        fprintf (texfile, " \\sin ");
        break;
    case OP_COS:
        fprintf (texfile, " \\cos ");
        break;
    case OP_TAN:
        fprintf (texfile, " \\tan ");
        break;
    default:
        break;
    }
}

int OneArgOp (int op)
{
    return op >= 6;
}

int GetOpRank (int op)
{
    if (op == OP_ADD || op == OP_SUB) return 1;
    if (op == OP_MUL || op == OP_DIV) return 2;
    if (op == OP_POW)                 return 3;
    else                              return 4;
}
