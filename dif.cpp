#include "dif.h"


int LoadTree (Tree_t *tree, const char *filename)
{
    TreeVerify (tree);
    if (filename == nullptr) TreeErr (tree, TREE_NULLPTR_ARG);

    if (tree -> data.left || tree -> data.right) TreeErr (tree, TREE_INCORRECT_POSITION);

    FILE *file = fopen (filename, "r");
    if (file == nullptr) TreeErr (tree, TREE_NULLPTR_ARG);

    tree -> size = 0;
    Read_tree (file, &(tree -> data), &(tree -> size));

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
        elem ->  left = TreeAllocElem ();
        elem -> right = TreeAllocElem ();
        if (elem ->  left == nullptr) return TREE_ALLOC_ERROR;
        if (elem -> right == nullptr) return TREE_ALLOC_ERROR;
        elem -> left  -> varval = 'e';
        elem -> right -> varval = 'e';
        
        err |= Read_tree (file, elem -> left, size);

        if (err) return err;

        err |= Read_op (file, elem);
        if (err) return err;

        err |= Read_tree (file, elem -> right, size);
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
        elem -> opval = OP_ADD;
        break;

    case '-':
        elem -> opval = OP_SUB;
        break;

    case '*':
        elem -> opval = OP_MUL;
        break;

    case '/':
        elem -> opval = OP_DIV;
        break;

    default:
        return TREE_INCORRECT_FORMAT;
    }
    elem -> type = TYPE_OP;
    return TREE_OK;
}

int Read_value (FILE *file, TreeElem_t *elem)
{
    int ch = SkipSpaces (file);

    if (isdigit (ch))
    {
        ungetc (ch, file);
        fscanf (file, "%lf", &(elem -> dblval));
        elem -> type = TYPE_DBL;
    }
    else if (isalpha (ch))
    {
        elem -> varval = (char) ch;
        elem -> type = TYPE_VAR;
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

    Print_tree (file, &(tree -> data));

    fclose (file);
    return TREE_OK;
}

void Print_tree (FILE *file, TreeElem_t *elem)
{
    fputc ('(', file);

    if (elem ->  left) Print_tree (file, elem ->  left);

    TreePrintVal (file, elem);

    if (elem -> right) Print_tree (file, elem -> right);

    fputc (')', file);
}
