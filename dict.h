
#ifndef DICT_H
#define DICT_H

#define DICT_POW 3
#define DICT_SIZE 4096  // 16^DICT_POW

enum type_e {COMMIT, TREE, BLOB};
typedef enum type_e type;

struct elem_s {
  char hash[41];
  type type;
  void* data;
};
typedef struct elem_s elem_t;

struct array_s {
  int capa;
  int size;
  elem_t** data;
};
typedef struct array_s array_t;

struct dict_s {
  array_t* tab[DICT_SIZE];
};
typedef struct dict_s dict_t;


dict_t* init_dict();
void add_dict(dict_t* d, char hash[41], type t, void* data);
elem_t* get_dict(dict_t* d, char hash[41]);


#endif // DICT_H
