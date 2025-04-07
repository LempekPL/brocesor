//
// Created by lempek on 30.03.2025.
//

#ifndef LIST_H
#define LIST_H

typedef struct value_t {
    char value;
    struct value_t* next;
} value_t;

typedef struct stos_t {
    value_t* value;
    struct stos_t* next;
} stos_t;

void add_new_value(stos_t** stos);
void insert_value(stos_t** stos, value_t* value);
void remove_value(stos_t** stos);
void replace_value(stos_t** stos, value_t* value);
value_t* pop_value(stos_t** stos);
void clone_value_at(stos_t** stos, int index);
void swap_values(stos_t** stos);
void free_stos(stos_t** stos);

int pop_parse_value(stos_t** stos);
int eq_values(value_t* v1, value_t* v2);
int lt_values(value_t* v1, value_t* v2);
value_t* add_values(value_t* v1, value_t* v2);

void add_new_char(stos_t** stos, char value);
void insert_char(value_t** list, char value);
void add_last_char(value_t** list, char value);
void add_last_value(value_t** list, value_t* value);
void remove_last_value(value_t** list);
value_t* clone_value(value_t* list);
value_t* get_last_value(value_t* list);
value_t* get_second_last_value(value_t* list);
value_t* value_from_int(int number);
int value_to_int(value_t* list);
void free_values(value_t* list);

void print_values(value_t* list);
void print_stos(stos_t* stos);

#endif //LIST_H
