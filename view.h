
#ifndef VIEW_H
#define VIEW_H

#include "dict.h"

struct blob_s {
  char* name;
  char hash[41];
  char* content;
};
typedef struct blob_s blob_t;

struct tree_s {
  char* name;
  char hash[41];
  int nb_blob;
  char** blobs;
  int nb_tree;
  char** trees;
};
typedef struct tree_s tree_t;

struct commit_s {
  char hash[41];
  char tree[41];
  int nb_parent;
  char** parents;
};
typedef struct commit_s commit_t;


FILE* run(char* cmd);
void hashcpy(char* h1, char* h2);
FILE* git_cat(char param, char hash[41]);
blob_t* init_blob(char* hash, char* name);
tree_t* init_tree(char* hash, char* name);
commit_t* init_commit(char* hash);
void free_blob(blob_t* b);
void free_tree(tree_t* t);
void free_commit(commit_t* c);
void print_blob(blob_t* b, int prof);
void print_tree(tree_t* t, int prof);
void print_commit(commit_t* c, int prof);


#endif // VIEW_H
