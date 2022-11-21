#include "tree.h"

int Tree_ctor (Tree_t *tree, const char *name, const char *func_name, const char *file_name, int line)
{
    if (tree == nullptr) return TREE_NULLPTR_ARG;

    if (tree -> status != TREE_CREATED) TreeErr (tree, TREE_STATUS_ERROR);

    Tree_set_info (tree, name, func_name, file_name, line);

    Tree_set_psn (&(tree -> data));

    tree -> size = 0;

    tree -> status = TREE_CONSTRUCTED;

    TreeVerify (tree);

    return TREE_OK;
}

void Tree_set_psn (TreeElem_t *elem)
{
    elem -> parent       = nullptr;
    elem -> left         = nullptr;
    elem -> right        = nullptr;
    elem -> type         = TYPE_PSN;
    elem -> value.dblval = POISON_DBL_VAL;
}

void Tree_set_info (Tree_t *tree, const char *name, const char *func_name, const char *file_name, int line)
{
    tree -> info.     name =      name;
    tree -> info.func_name = func_name;
    tree -> info.file_name = file_name;
    tree -> info.     line =      line;
}


int TreeDtor (Tree_t *tree)
{
    TreeVerify (tree);

    if (tree -> data.left) Tree_free_data (tree -> data.left, &(tree -> size));

    Tree_set_psn (&(tree -> data));

    tree -> size = 0;
    
    tree -> status = TREE_DECONSTRUCTED;

    return TREE_OK;
}

int TreeFreeData (Tree_t *tree, TreeElem_t *elem)
{
    TreeVerify (tree);

    if (elem == nullptr) return TREE_OK;

    return Tree_free_data (elem, &(tree -> size));
}

int Tree_free_data (TreeElem_t *elem, int *size)
{
    if (elem ->  left) Tree_free_data (elem ->  left, size);
    if (elem -> right) Tree_free_data (elem -> right, size);

    *size -= 1;

    return TREE_OK;
}


TreeElem_t *TreeAllocElem (void)
{
    return (TreeElem_t *) (calloc (1, sizeof (TreeElem_t)));
}

int TreeAddElem (Tree_t *tree, TreeElem_t *parent, int position, TreeElem_t *newelem)
{
    TreeVerify (tree);

    if (parent == nullptr || newelem == nullptr) TreeErr (tree, TREE_NULLPTR_ARG);

    if      (position ==  LEFT && !(parent ->  left)) parent ->  left = newelem;
    else if (position == RIGHT && !(parent -> right)) parent -> right = newelem;
    else TreeErr (tree, TREE_INCORRECT_POSITION);

    newelem -> parent = parent;
    tree -> size ++;

    return TREE_OK;
}

int TreeAddLeft (Tree_t *tree, TreeElem_t *parent, TreeElem_t *newelem)
{
    return TreeAddElem (tree, parent, LEFT, newelem);
}

int TreeAddRight (Tree_t *tree, TreeElem_t *parent, TreeElem_t *newelem)
{
    return TreeAddElem (tree, parent, RIGHT, newelem);
}

int Tree_verify (Tree_t *tree)
{
    #ifdef NDEBUG
    return 0;
    #else

    if (tree == nullptr) return TREE_NULLPTR_ARG;

    if (tree -> status != TREE_CONSTRUCTED) tree -> err |= TREE_STATUS_ERROR;
    if (tree -> size < 0)                   tree -> err |= TREE_INCORRECT_SIZE;

    if (tree -> info.     name == nullptr ||
        tree -> info.file_name == nullptr ||
        tree -> info.func_name == nullptr   ) tree -> err |= TREE_INFO_CORRUPTED;

    int size = tree -> size;

    if (tree -> data.right || tree -> data.parent) tree -> err |= TREE_DATA_CORRUPTED;

    if (tree -> data.left)
    {
        if (tree -> data.left -> parent != &(tree -> data)) tree -> err |= TREE_DATA_CORRUPTED;
        tree -> err |= Tree_verify_data (tree -> data.left, &size);
    }
    if (size != 0) tree -> err |= TREE_INCORRECT_SIZE;

    return tree -> err;
    #endif
}

int Tree_verify_data (TreeElem_t *elem, int *size)
{
    if (*size < 0) return TREE_DATA_CORRUPTED;

    int err = TREE_OK;

    if (elem -> value.dblval == POISON_DBL_VAL) err |= TREE_DATA_CORRUPTED;

    if (elem -> left)
    {   
        if (elem -> left -> parent != elem) err |= TREE_DATA_CORRUPTED;
        err |= Tree_verify_data (elem ->  left, size);
    }
    if (elem -> right)
    {
        if (elem -> right -> parent != elem) err |= TREE_DATA_CORRUPTED;
        err |= Tree_verify_data (elem -> right, size);
    }

    *size -= 1;

    return err;
}

void Tree_print_error (Tree_t *tree, const char *func_name, const char *file_name, int line)
{
    int err = tree == nullptr ? TREE_NULLPTR_ARG : tree -> err;
    
    printf ("\nERROR (%d) in (%s) at (%s) at line (%d).\n", err, func_name, file_name, line);

    Tree_dump (tree, func_name, file_name, line);
}
