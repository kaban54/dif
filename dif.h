#ifndef _DIF_H_
#define _DIF_H_

#include "tree/tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>


const char *const TEXFILENAME = "tex/texfile.tex";
extern FILE *TEX;

enum ARITHMERRORS
{
    ARITHM_OK            =  0,
    ARITHM_DIV_BY_ZERO   =  1,
    ARITHM_ROOT_OF_NEG   =  2,
    ARITHM_ZERO_POW_ZERO =  4,
    ARITHM_LOG_OF_NEG    =  8,
    ARITHM_UNKNOWN_OP    = 16,
};

//DSL --------------------------------------------------------------------

#define NUM(x) CreateNum (x)

#define L (elem ->  left)
#define R (elem -> right)

#define   OP (elem -> value.opval)
#define  VAL (elem -> value.dblval)
#define LVAL (elem ->  left -> value.dblval)
#define RVAL (elem -> right -> value.dblval)

#define LL (elem ->  left ->  left)
#define LR (elem ->  left -> right)
#define RL (elem -> right ->  left)
#define RR (elem -> right -> right)

#define dL diff (elem ->  left, var)
#define dR diff (elem -> right, var)

#define cL copy (elem ->  left)
#define cR copy (elem -> right)

#define ADD(left, right) CreateOp (OP_ADD, left, right)
#define SUB(left, right) CreateOp (OP_SUB, left, right)
#define MUL(left, right) CreateOp (OP_MUL, left, right)
#define DIV(left, right) CreateOp (OP_DIV, left, right)
#define POW(left, right) CreateOp (OP_POW, left, right)
#define  LN(node)        CreateOp (OP_LN , CreateNum (0), node)
#define SIN(node)        CreateOp (OP_SIN, CreateNum (0), node)
#define COS(node)        CreateOp (OP_COS, CreateNum (0), node)
#define TAN(node)        CreateOp (OP_TAN ,CreateNum (0), node)

// -----------------------------------------------------------------------

int LoadTree (Tree_t *tree, const char *filename);

int Read_tree (FILE *file, TreeElem_t *elem, int *size);

int SkipSpaces (FILE *file);

int Read_op (FILE *file, TreeElem_t *elem);

int Read_value (FILE *file, TreeElem_t *elem);

int SaveTree (Tree_t *tree, const char *filename);

void Print_tree (FILE *file, TreeElem_t *elem);


int Tree_get_size (TreeElem_t *elem);

int GetDerivative (Tree_t *der_tree, Tree_t *func_tree, char var);

TreeElem_t *diff (TreeElem_t *elem, char var);

TreeElem_t *diff_op (TreeElem_t *elem, char var);

TreeElem_t *diff_mul (TreeElem_t *elem, char var, int var_l, int var_r);

TreeElem_t *diff_div (TreeElem_t *elem, char var, int var_l, int var_r);

TreeElem_t *diff_pow (TreeElem_t *elem, char var, int var_l, int var_r);

TreeElem_t *copy (TreeElem_t *elem);

TreeElem_t *CreateNum (double num);

TreeElem_t *CreateOp (int op, TreeElem_t *left, TreeElem_t *right);

int FindVar (TreeElem_t *node, char var);

TreeElem_t *Simplify (TreeElem_t *elem);

int CalculateConsts (TreeElem_t *elem, int *size);

int Calculate (TreeElem_t *elem);

#endif