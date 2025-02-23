#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dict.h"
#include "view.h"

FILE* run(char* cmd) {
  FILE* fp = popen(cmd, "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }
  return fp;
}

void hashcpy(char* h1, char* h2) {
  strncpy(h1, h2, 40);
  h1[40] = 0;
}

FILE* git_cat(char param, char hash[41]) {
  char cmd[67];
  cmd[0] = 0;
  strcat(cmd, "/usr/bin/git cat-file -? ");
  cmd[23] = param;
  strncat(cmd, hash, 41);
  FILE* fp = run(cmd);
  return fp;
}

blob_t* init_blob(char* hash, char* name) {
  blob_t* blob = (blob_t*) malloc(sizeof(blob_t));
  hashcpy(blob->hash, hash);
  blob->name = name;
  blob->content = NULL;
  return blob;
}

tree_t* init_tree(char* hash, char* name) {
  tree_t* tree = (tree_t*) malloc(sizeof(tree_t));
  hashcpy(tree->hash, hash);

  tree->name = name;

  FILE* fp = git_cat('p', hash);
  char line[1000];
  int nb_trees = 0;
  int nb_blobs = 0;
  while (fgets(line, sizeof(line), fp)) {
    if (strncmp(&line[7], "tree", 4) == 0) nb_trees++;
    else if (strncmp(&line[7], "blob", 4) == 0) nb_blobs++;
  }
  pclose(fp);
  fp = git_cat('p', hash);
  tree->nb_tree = nb_trees;
  tree->nb_blob = nb_blobs;
  tree->trees = (char**) malloc(sizeof(char*)*nb_trees);
  for (int i=0; i<nb_trees; i++) tree->trees[i] = (char*) malloc(sizeof(char)*41);
  tree->blobs = (char**) malloc(sizeof(char*)*nb_blobs);
  for (int i=0; i<nb_blobs; i++) tree->blobs[i] = (char*) malloc(sizeof(char)*41);
  nb_trees = 0;
  nb_blobs = 0;
  while (fgets(line, sizeof(line), fp)) {
    if (strncmp(&line[7], "tree", 4) == 0) {
      hashcpy(tree->trees[nb_trees], &line[12]);
      nb_trees++;
    } else if (strncmp(&line[7], "blob", 4) == 0) {
      hashcpy(tree->blobs[nb_blobs], &line[12]);
      nb_blobs++;
    }
  }
  pclose(fp);

  return tree;
}

commit_t* init_commit(char* hash) {
  commit_t* commit = (commit_t*) malloc(sizeof(commit_t));
  hashcpy(commit->hash, hash);

  FILE* fp = git_cat('p', hash);

  commit->nb_parent = 0;

  char line[1000];
  while(fgets(line, sizeof(line), fp)) {
    if (strncmp(line, "tree", 4) == 0) {
      hashcpy(commit->tree, &line[5]);
    } else if (strncmp(line, "parent", 6) == 0) {
      commit->nb_parent++;
    }
  }
  pclose(fp);

  if (commit->nb_parent != 0) {
    commit->parents = (char**) malloc(sizeof(char*)*commit->nb_parent);
    commit->nb_parent = 0;
    fp = git_cat('p', hash);
    while (fgets(line, sizeof(line), fp)) {
      if (strncmp(line, "parent", 6) == 0) {
        commit->parents[commit->nb_parent] = (char*) malloc(sizeof(char)*41);
        hashcpy(commit->parents[commit->nb_parent], &line[7]);
        commit->nb_parent++;
      }
    }
  }

  return commit;
}

void free_blob(blob_t* b) {
  if (b->content != NULL) free(b->content);
  free(b->name);
  free(b);
}

void free_tree(tree_t* t) {
  for (int i=0; i<t->nb_blob; i++) free(t->blobs[i]);
  free(t->blobs);
  for (int i=0; i<t->nb_tree; i++) free(t->trees[i]);
  free(t->trees);
  free(t->name);
  free(t);
}

void free_commit(commit_t* c) {
  for (int i=0; i<c->nb_parent; i++) free(c->parents[i]);
  if (c->nb_parent != 0) free(c->parents);
  free(c);
}

void free_elem(void* elem_v) {
  elem_t* elem = (elem_t*) elem_v;
  if (elem->type == COMMIT) free_commit((commit_t*) elem->data);
  else if (elem->type == TREE) free_tree((tree_t*) elem->data);
  else if (elem->type == BLOB) free_blob((blob_t*) elem->data);
  free(elem);
}

void print_debut(int prof) {
  for (int i=0; i<prof; i++) printf("| ");
}

void print_blob(blob_t* b, int prof) {
  print_debut(prof);
  printf("%s (%s)\n", b->name, b->hash);
}

void print_tree(dict_t* all_hashes, tree_t* t, int prof) {
  print_debut(prof);
  printf("%s (%s)\n", t->name, t->hash);
  for (int i=0; i<t->nb_blob; i++) {
    blob_t* blob = (blob_t*) get_dict(all_hashes, t->blobs[i]);
    print_blob(blob, prof+1);
  }
  for (int i=0; i<t->nb_tree; i++) {
    tree_t* tree = (tree_t*) get_dict(all_hashes, t->trees[i]);
    print_tree(all_hashes, tree, prof+1);
  }
}

void print_commit(dict_t* all_hashes, commit_t* c, int prof) {
  print_debut(prof);
  printf("%s\n", c->hash);
  tree_t* tree = (tree_t*) get_dict(all_hashes, c->tree);
  print_tree(all_hashes, tree, prof+1);
  printf("\n");
  for (int i=0; i<c->nb_parent; i++) {
    commit_t* co = (commit_t*) get_dict(all_hashes, c->parents[i]);
    print_commit(all_hashes, co, prof);
  }
}

char* get_hashes(dict_t* all_hashes) {
  FILE* fp;
  char* commit_hash = (char*) malloc(sizeof(char)*41);
  char hash[42];

  /* Open the command for reading. */
  fp = run("/usr/bin/git rev-parse HEAD 2>/dev/null");

  /* Read the output a line at a time - output it. */
  fgets(hash, sizeof(hash), fp);
  pclose(fp);
  if (hash[0] == 0) {
    printf("Not a git repo\n");
    exit(1);
  }
  hashcpy(commit_hash, hash);

  fp = run("/usr/bin/git rev-list --objects --all");

  char line[1000];
  while(fgets(line, sizeof(line), fp)) {
    char hash[41];
    strncpy(hash, line, 40);
    hash[40] = 0;
    size_t name_size = strlen(&line[41]);
    char* name = (char*) malloc(sizeof(char)*(name_size+1));
    strcpy(name, &line[41]);
    if (name_size != 0) name[name_size-1] = 0;
    name[name_size] = 0;

    FILE* fp_type = git_cat('t', hash);
    char str_type[10];
    type type = 0;
    fgets(str_type, sizeof(str_type), fp_type);
    if (strncmp(str_type, "commit", 6) == 0) type = COMMIT;
    else if (strncmp(str_type, "tree", 4) == 0) type = TREE;
    else if (strncmp(str_type, "blob", 4) == 0) type = BLOB;
    else printf("type not found\n");

    void* elem = NULL;

    if (type == COMMIT) {
      elem = init_commit(hash);
      free(name);
    } else if (type == TREE) elem = init_tree(hash, name);
    else if (type == BLOB) elem = init_blob(hash, name);

    add_dict(all_hashes, hash, type, elem);
  }
  return commit_hash;
}
