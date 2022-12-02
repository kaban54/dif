#include "dif.h"


int ReadTree (Tree_t *tree, char *input_str)
{
    if (input_str == nullptr) return TREE_NULLPTR_ARG;
    TreeVerify (tree);

    char **s_ptr = &input_str;
    TreeElem_t *func = GetG (s_ptr);
    if (func == nullptr) return TREE_INCORRECT_FORMAT;

    tree -> data.left = func;
    func -> parent = &(tree -> data);

    tree -> size = Tree_get_size (func);

    TreeVerify (tree);
    return TREE_OK;
}

TreeElem_t *GetG (char **s_ptr)
{
    TreeElem_t *ret = GetExp (s_ptr);
    if (**s_ptr != '\0')
    {
        printf ("Syntax Error: there mustn't be anything after function.\n");
        Tree_free_data (ret, NULL);
        return nullptr;
    }
    return ret;
}

TreeElem_t *GetExp (char **s_ptr)
{
    TreeElem_t *ret = GetTerm (s_ptr);

    if (ret == nullptr) return nullptr;

    TreeElem_t *newelem = nullptr;
    int op = 0;

    while (**s_ptr == '+' || **s_ptr == '-')
    {
        if (**s_ptr == '+') op = OP_ADD;
        else                op = OP_SUB;     
        (*s_ptr) = StrSkipSpaces (*s_ptr);
        newelem = GetTerm (s_ptr);
        if (newelem == nullptr) return nullptr;
        ret = CreateOp (op, ret, newelem);
    }
    return ret;
}

TreeElem_t *GetTerm (char **s_ptr)
{
    TreeElem_t *ret = GetPow (s_ptr);
    if (ret == nullptr) return nullptr;

    TreeElem_t *newelem = nullptr;
    int op = 0;

    while (**s_ptr == '*' || **s_ptr == '/')
    {
        if (**s_ptr == '*') op = OP_MUL;
        else                op = OP_DIV;     
        (*s_ptr) = StrSkipSpaces (*s_ptr);
        newelem = GetPow (s_ptr);
        if (newelem == nullptr) return nullptr;
        ret = CreateOp (op, ret, newelem);
    }
    return ret;
}

TreeElem_t *GetPow (char **s_ptr)
{
    TreeElem_t *left = GetBrack (s_ptr);
    if (left == nullptr) return nullptr;

    if (**s_ptr == '^')
    {
        (*s_ptr) = StrSkipSpaces (*s_ptr);
        TreeElem_t *right = GetBrack (s_ptr);
        if (right == nullptr)
        {
            free (left);
            return nullptr;
        }
        return POW (left, right);
    }
    else return left;
}

TreeElem_t *GetBrack (char **s_ptr)
{
    if (**s_ptr == '(')
    {
        (*s_ptr) = StrSkipSpaces (*s_ptr);
        TreeElem_t *elem = GetExp (s_ptr);
        
        if (**s_ptr == ')')
        {
            (*s_ptr) = StrSkipSpaces (*s_ptr);
            return elem;
        }
        else
        {
            free (elem);
            printf ("Syntax Error: no closer bracket.");
            return nullptr;
        }
    }
    else return GetFunc (s_ptr);
}

TreeElem_t *GetFunc (char **s_ptr)
{
    int op = GetFuncname (s_ptr);

    if (op) return CreateOp (op, NUM (0), GetBrack (s_ptr));

    TreeElem_t *elem = GetVar (s_ptr);
    if (elem) return elem;
    else      return GetNum (s_ptr);
}

int GetFuncname (char **s_ptr)
{
    int op = 0;
    char *s = *s_ptr;

    if (strncmp (s, "sin", 3) == 0)
    {
        s += 2;
        op = OP_SIN;
    }
    else if (strncmp (s, "cos", 3) == 0)
    {
        s += 2;
        op = OP_COS;
    }
    else if (strncmp (s, "tan", 3) == 0)
    {
        s += 2;
        op = OP_TAN;
    }
    else if (strncmp (s, "ln", 2) == 0)
    {
        s += 1;
        op = OP_LN;
    }
    else return 0;

    *s_ptr = StrSkipSpaces (s);
    return op;
}


TreeElem_t *GetNum (char **s_ptr)
{
    char *s = *s_ptr;

    if (!('0' <= *s && *s <= '9'))
    {
        printf ("Syntax Error: cannot get number or variable.\n");
        return nullptr;
    }

    double val = 0;
    int symbs_read = 0;

    sscanf (s, "%lf%n", &val, &symbs_read);
    s += symbs_read;
    
    *s_ptr = StrSkipSpaces (s - 1);
    return NUM (val);
}

TreeElem_t *GetVar (char **s_ptr)
{
    char var = 0;

    char *s = *s_ptr;

    if (isalpha (*s))
    {
        var = *s;
        s = StrSkipSpaces (s);
    }
    else return nullptr;

    TreeElem_t *elem = TreeAllocElem ();
    if (elem == nullptr) return nullptr;

    TYPE = TYPE_VAR;
    elem -> value.varval = var;

    *s_ptr = s;
    return elem;
}

char *StrSkipSpaces (char *str)
{
    if (str == nullptr) return nullptr;
    if (*str == '\0')   return str;

    str++;
    while (isspace (*str)) str++;

    return str;
}
