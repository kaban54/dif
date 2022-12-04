#ifndef _DIF_H_
#define _DIF_H_

#include "tree/tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

const char *const  TEXFILENAME = "tex/texfile.tex";
const char *const PLOTFILENAME = "tex/plotfile";

const int PLOTIMGW = 1600;
const int PLOTIMGH = 1200;

const size_t BUFSIZE = 256;

const int MAX_TAYLOR_POW = 12;

//DSL --------------------------------------------------------------------

#define NUM(x) CreateNum (x)
#define VAR(x) CreateVar (x)

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

#define LLP (LL -> parent)
#define LRP (LR -> parent)
#define RLP (RL -> parent)
#define RRP (RR -> parent)

#define LIS0 (LTYPE == TYPE_NUM && LVAL == 0)
#define RIS0 (RTYPE == TYPE_NUM && RVAL == 0)
#define LIS1 (LTYPE == TYPE_NUM && LVAL == 1)
#define RIS1 (RTYPE == TYPE_NUM && RVAL == 1)

#define dL diff (L, var, texfile)
#define dR diff (R, var, texfile)

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

#define EQUAL(node1, node2) CompareTrees (node1, node2)

#define OPRANK  (GetOpRank (OP))
#define LOPRANK (GetOpRank (L -> value.opval))
#define ROPRANK (GetOpRank (R -> value.opval))
// -----------------------------------------------------------------------

int LoadTree (Tree_t *tree, double *x0, int *taylor_pow, const char *filename);

int ReadTree (Tree_t *tree, char *input_str);

TreeElem_t *GetG (char **s_ptr);

TreeElem_t *GetExp (char **s_ptr);

TreeElem_t *GetTerm (char **s_ptr);

TreeElem_t *GetPow (char **s_ptr);

TreeElem_t *GetBrack (char **s_ptr);

TreeElem_t *GetFunc (char **s_ptr);

int GetFuncname (char **s_ptr);

TreeElem_t *GetNum (char **s_ptr);

TreeElem_t *GetVar (char **s_ptr);

char *StrSkipSpaces (char *str);


int SaveTree (Tree_t *tree, const char *filename);

void Print_tree (FILE *file, TreeElem_t *elem);

int Tree_get_size (TreeElem_t *elem);

int GeneratePdf (Tree_t *func_tree, double x0, int taylor_pow);


int GetDerivative (Tree_t *der_tree, Tree_t *func_tree, char var, FILE *texfile);

TreeElem_t *diff (TreeElem_t *elem, char var, FILE *texfile);

TreeElem_t *diff_op (TreeElem_t *elem, char var, FILE *texfile);

TreeElem_t *diff_mul (TreeElem_t *elem, char var, int var_l, int var_r, FILE *texfile);

TreeElem_t *diff_div (TreeElem_t *elem, char var, int var_l, int var_r, FILE *texfile);

TreeElem_t *diff_pow (TreeElem_t *elem, char var, int var_l, int var_r, FILE *texfile);

TreeElem_t *copy (TreeElem_t *elem);

TreeElem_t *CreateNum (double num);

TreeElem_t *CreateOp (int op, TreeElem_t *left, TreeElem_t *right);

TreeElem_t *CreateVar (char var);

int FindVar (TreeElem_t *node, char var);


TreeElem_t *Simplify (TreeElem_t *elem, FILE *texfile);

TreeElem_t *CalculateConsts (TreeElem_t *elem, int *size);

TreeElem_t *Calculate (TreeElem_t *elem);

TreeElem_t *RemoveNeutrals (TreeElem_t *elem, int *size);

TreeElem_t *Remove_neutrals (TreeElem_t *elem, int *size);

TreeElem_t *Replace_with_left (TreeElem_t *elem, int *size);

TreeElem_t *Replace_with_right (TreeElem_t *elem, int *size);

TreeElem_t *Replace_with_num (TreeElem_t *elem, int *size, double num);


int GetSlope (Tree_t *slope_tree, Tree_t *func_tree, Tree_t *der_tree, double x0, FILE *texfile);

double GetFuncVal (TreeElem_t *elem, double x0);

TreeElem_t *Replace_var_with_num (TreeElem_t *elem, char var, double num);


int GetTaylor (Tree_t *taylor_tree, Tree_t *func_tree, Tree_t *der_tree, int max_pow, FILE *texfile);

long long Fact (int x);


int CompareTrees (TreeElem_t *elem1, TreeElem_t *elem2);

int OpCommutative (int op);


void Print_tex_top (FILE *texfile);

void Print_tex_bottom (FILE *texfile);

void Print_before_diff (FILE *texfile, TreeElem_t *elem);

void Print_after_diff (FILE *texfile, TreeElem_t *elem);

void Print_before_simplify (FILE *texfile, TreeElem_t *elem);

void Print_after_simplify (FILE *texfile, TreeElem_t *elem);

void PrintTreeTex (FILE *texfile, TreeElem_t *elem);

void Print_tree_tex (FILE *texfile, TreeElem_t *elem);

void Print_frac_tex (FILE *texfile, TreeElem_t *elem);

void Print_pow_tex (FILE *texfile, TreeElem_t *elem);

void PrintOpTex (FILE *texfile, int op);

int OneArgOp (int op);

int GetOpRank (int op);


void Print_img_tex (FILE *texfile, int number_of_funcs, TreeElem_t **funcs, const char *caption);

void GeneratePlotImg (int number_of_funcs, TreeElem_t **funcs, int imgnum);

void Print_gnuplot_exp (FILE *plotfile, TreeElem_t *elem);

void Print_gnuplot_op  (FILE *plotfile, int op);

#endif