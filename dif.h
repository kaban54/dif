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
    ARITHM_LOG_OF_NEG    =  4,
    ARITHM_UNKNOWN_OP    =  8,
};

//DSL --------------------------------------------------------------------

#define NUM(x) CreateNum (x)

#define L (elem ->   left)
#define R (elem ->  right)
#define P (elem -> parent)

#define TYPE  (elem -> type)
#define OP    (elem -> value.opval)
#define VAL   (elem -> value.dblval)
#define LVAL  (L    -> value.dblval)
#define RVAL  (R    -> value.dblval)
#define LTYPE (L    -> type)
#define RTYPE (R    -> type)

#define LL (L ->   left)
#define LR (L ->  right)
#define LP (L -> parent)
#define RL (R ->   left)
#define RR (R ->  right)
#define RP (R -> parent)
#define PL (P ->   left)
#define PR (P ->  right)

#define LIS0 (LTYPE == TYPE_NUM && LVAL == 0)
#define RIS0 (RTYPE == TYPE_NUM && RVAL == 0)
#define LIS1 (LTYPE == TYPE_NUM && LVAL == 1)
#define RIS1 (RTYPE == TYPE_NUM && RVAL == 1)

#define dL diff (L, var)
#define dR diff (R, var)

#define cL copy (L)
#define cR copy (R)

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

int RemoveNeutrals (TreeElem_t *elem, int *size);

int Remove_neutrals (TreeElem_t *elem, int *size);

void Replace_with_left (TreeElem_t *elem, int *size);

void Replace_with_right (TreeElem_t *elem, int *size);

void Replace_with_num (TreeElem_t *elem, int *size, double num);



#endif