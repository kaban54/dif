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


int GetDerivative (Tree_t *der_tree, Tree_t *func_tree, char var)
{
    TreeVerify ( der_tree);
    TreeVerify (func_tree);
    TreeElem_t *der = diff (func_tree -> data.left, var);
    der_tree -> size = Tree_get_size (der);

    der -> parent = &(der_tree -> data);
    der_tree -> data.left = der;

    TreeVerify (der_tree);
    return TREE_OK;
}

TreeElem_t *diff (TreeElem_t *elem, char var)
{
    TreeElem_t *der = nullptr;

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
        der = diff_op (elem, var);
        break;

    default:
        return nullptr;
    }

    // Print to tex

    Simplify (der);

    return der;
}

TreeElem_t *diff_op (TreeElem_t *elem, char var)
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
        return diff_mul (elem, var, var_l, var_r);

    case OP_DIV:
        return diff_div (elem, var, var_l, var_r);

    case OP_POW:
        return diff_pow (elem, var, var_l, var_r);
        
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


TreeElem_t *diff_mul (TreeElem_t *elem, char var, int var_l, int var_r)
{
    if (var_l && var_r) return ADD (MUL (dL, cR), MUL (cL, dR));
    else if (var_l)     return MUL (cR, dL);
    else                return MUL (cL, dR);
}

TreeElem_t *diff_div (TreeElem_t *elem, char var, int var_l, int var_r)
{
    if (var_l && var_r) return DIV (SUB (MUL (dL, cR), MUL (cL, dR)), POW (cR, NUM (2)));
    // (f / g)' = (f'g - fg') / g^2
    else if (var_l)     return DIV (dL, cR);
    // (f / c)' = f' / c
    else                return MUL (cL, MUL (NUM (-1), DIV (dR, POW (cR, NUM (2)))));
    // (c / f)' = c * (- f' / f^2)
}

TreeElem_t *diff_pow (TreeElem_t *elem, char var, int var_l, int var_r)
{
    if (var_l && var_r) return MUL (POW (cL, cR), ADD (MUL (cR, DIV (dL, cL)), MUL (LN (cL), dR))); 
    // (f^g)' = f^g * (g * f' / f + ln(f) * g')
    else if (var_l)     return MUL (cR, MUL (POW (cL, SUB (cR, NUM (1))), dL));
    // (f^c)' = c * f^(c - 1) * f'
    else                return MUL(LN (cL), MUL (POW (cL, cR), dR));
    // (c^f)' = ln(c) * c^f * f'
}


TreeElem_t *copy (TreeElem_t *elem)
{
    TreeElem_t *elem_cpy = TreeAllocElem ();
    if (elem_cpy == nullptr) return nullptr;

    elem_cpy -> type  = TYPE;
    elem_cpy -> value = elem -> value;

    if (L) elem_cpy ->  left = copy (L);
    if (R) elem_cpy -> right = copy (R);

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
    TreeElem_t *node = TreeAllocElem ();
    if (node == nullptr) return nullptr;

    node -> type = TYPE_OP;
    node -> value.opval = op;
    node -> left  = left;
    node -> right = right;

    left  -> parent = node;
    right -> parent = node;

    return node;
}


int FindVar (TreeElem_t *node, char var)
{
    if (node == nullptr) return 0;

    if (node -> type == TYPE_VAR && (node -> value.varval == var || var == 0)) return 1;
    if (node -> type == TYPE_OP) return FindVar (node -> left, var) | FindVar (node -> right, var);

    return 0;
}


TreeElem_t *Simplify (TreeElem_t *elem)
{
    int size = Tree_get_size (elem);
    int old_size = size;

    CalculateConsts (elem, &size);
    RemoveNeutrals  (elem, &size);

    while (size < old_size)
    {
        old_size = size;
        CalculateConsts (elem, &size);
        RemoveNeutrals  (elem, &size);
    }

    // Print to tex

    return elem;
}

int CalculateConsts (TreeElem_t *elem, int *size)
{
    if (TYPE == TYPE_NUM || TYPE == TYPE_VAR) return 0;

    int err = ARITHM_OK;

    err |= CalculateConsts (L, size);
    err |= CalculateConsts (R, size);
    if (err) return err;

    if (L -> type == TYPE_NUM && R -> type == TYPE_NUM)
    {
        err |= Calculate (elem);
        if (err) return err;
        (*size) -= 2;
    }
    // Print to tex
    return err;
}

int Calculate (TreeElem_t *elem)
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
        if (RVAL == 0) return ARITHM_DIV_BY_ZERO;
        VAL = LVAL / RVAL;
        break;

    case OP_POW:
        if (LVAL <= 0 && RVAL != (int) RVAL) return ARITHM_ROOT_OF_NEG;
        VAL = pow (LVAL, RVAL);
        break;

    case OP_LN:
        if (RVAL <= 0) return ARITHM_LOG_OF_NEG;
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
        return ARITHM_UNKNOWN_OP;
    }

    TYPE = TYPE_NUM;
    free (L);
    free (R);
    L = nullptr;
    R = nullptr;

    return ARITHM_OK;
}


int RemoveNeutrals (TreeElem_t *elem, int *size)
{
    if (TYPE == TYPE_NUM || TYPE == TYPE_VAR) return ARITHM_OK;

    int err = ARITHM_OK;

    err |= RemoveNeutrals (L, size);
    err |= RemoveNeutrals (R, size);
    if (err) return err;

    if (LTYPE != TYPE_NUM && RTYPE != TYPE_NUM) return ARITHM_OK;

    Remove_neutrals (elem, size);
    // Print to tex
    return err;
}

int Remove_neutrals (TreeElem_t *elem, int *size)
{
    switch (OP)
    {
    case OP_ADD:
        if      (LIS0) Replace_with_right (elem, size);
        else if (RIS0) Replace_with_left  (elem, size);
        break;
    
    case OP_SUB:
        if      (RIS0) Replace_with_left (elem, size);
        else if (LIS0)
        {
            TreeElem_t *newelem = MUL (NUM (-1), R);
            if (PL == elem) PL = newelem;
            else            PR = newelem;
            newelem -> parent = P;
            free (L);
            free (elem);
        }
        break;
    
    case OP_MUL:
        if (LIS0 || RIS0) Replace_with_num   (elem, size, 0);
        else if (LIS1)    Replace_with_right (elem, size);
        else if (RIS1)    Replace_with_left  (elem, size);
        break;
    
    case OP_DIV:
        if      (RIS0) return ARITHM_DIV_BY_ZERO;
        if      (RIS1) Replace_with_left (elem, size);
        else if (LIS0) Replace_with_num  (elem, size, 0);
        break;

    case OP_POW:
        if      (RIS0) Replace_with_num (elem, size, 1);
        else if (LIS1) Replace_with_num (elem, size, 1);
        else if (RIS1) Replace_with_left (elem, size);
        else if (LIS0)
        {
            if (RTYPE == TYPE_NUM && RVAL < 0) return ARITHM_ROOT_OF_NEG;
            else Replace_with_num (elem, size, 0);
        }
        break;

    default:
        break;
    }
    return 0;
}

void Replace_with_left (TreeElem_t *elem, int *size)
{
    free (R);
    LP = P;
    if (PL == elem) PL = L;
    else            PR = L;
    free (elem);
    *size -= 2;
}

void Replace_with_right (TreeElem_t *elem, int *size)
{
    free (L);
    RP = P;
    if (PL == elem) PL = R;
    else            PR = R;
    free (elem);
    *size -= 2;
}

void Replace_with_num (TreeElem_t *elem, int *size, double num)
{
    free (L);
    free (R);

    TreeElem_t *newelem = NUM (num);
    newelem -> parent = P;
    if (PL == elem) PL = newelem;
    else            PR = newelem;

    free (elem);
    *size -= 2;
}

/*
void PrintTreeTex (TreeElem_t *elem)
{
    fprintf (TEX, "$");

    Print_tree_tex (elem);

    fprintf (TEX, "$");
}

void Print_tree_tex (TreeElem_t *elem)
{

}


int GetOpRank (int op)
{
    if (op == )
}
*/