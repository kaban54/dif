#ifndef _DIF_H_
#define _DIF_H_

#include "tree/tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


int LoadTree (Tree_t *tree, const char *filename);

int Read_tree (FILE *file, TreeElem_t *elem, int *size);

int SkipSpaces (FILE *file);

int Read_op (FILE *file, TreeElem_t *elem);

int Read_value (FILE *file, TreeElem_t *elem);

int SaveTree (Tree_t *tree, const char *filename);

void Print_tree (FILE *file, TreeElem_t *elem);


#endif