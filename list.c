//
// Created by lempek on 30.03.2025.
//

#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void add_new_value(stos_t** stos) {
    stos_t* new_node = malloc(sizeof(stos_t));
    new_node->value = NULL;
    new_node->next = *stos;
    *stos = new_node;
}

void insert_value(stos_t** stos, value_t* value) {
    stos_t* new_node = malloc(sizeof(stos_t));
    new_node->value = value;
    new_node->next = *stos;
    *stos = new_node;
}

void remove_value(stos_t** stos) {
    stos_t* temp = *stos;
    *stos = (*stos)->next;
    free_values(temp->value);
    free(temp);
}

void replace_value(stos_t** stos, value_t* value) {
    free_values((*stos)->value);
    (*stos)->value = value;
}

value_t* pop_value(stos_t** stos) {
    if (stos == NULL || *stos == NULL) return NULL;
    stos_t* temp = *stos;
    *stos = (*stos)->next;
    value_t* value = temp->value;
    free(temp);
    return value;
}

void clone_value_at_wrap(stos_t** stos, stos_t* pass, int index) {
    if (index <= 0) {
        add_new_value(stos);
        (*stos)->value = clone_value(pass->value);
    } else {
        clone_value_at_wrap(stos, pass->next, index - 1);
    }
}

void clone_value_at(stos_t** stos, int index) {
    clone_value_at_wrap(stos, *stos, index);
}

void swap_values(stos_t** stos) {
    stos_t* temp = *stos;
    *stos = (*stos)->next;
    temp->next = (*stos)->next;
    (*stos)->next = temp;
}

void free_stos(stos_t** stos) {
    if (stos == NULL || *stos == NULL) return;
    free_stos(&(*stos)->next);
    remove_value(stos);
    free_stos(stos);
}

int pop_parse_value(stos_t** stos) {
    value_t* v = pop_value(stos);
    int num = value_to_int(v);
    free_values(v);
    return num;
}

int eq_values_wrap(value_t* v1, value_t* v2, int neg, int zero) {
    if (!v1 && !v2) return neg == 0 || zero;
    if (v1 && v1->value == '-') return eq_values_wrap(v1->next, v2, neg + 1, zero);
    if (v2 && v2->value == '-') return eq_values_wrap(v1, v2->next, neg - 1, zero);
    if (v1 && v2) {
        if (v1->value != v2->value) return 0;
        return eq_values_wrap(v1->next, v2->next, neg, v1->value == '0' && zero);
    }
    if (!v1) return v2->value == '0' ? eq_values_wrap(NULL, v2->next, neg, zero) : 0;
    return v1->value == '0' ? eq_values_wrap(v1->next, NULL, neg, zero) : 0;
}

int eq_values(value_t* v1, value_t* v2) {
    return eq_values_wrap(v1, v2, 0, 1);
}

int lt_values_wrap(value_t* v1, value_t* v2, int smaller) {
    if (!v1 && !v2) return smaller;
    if (v1 && v2 && v1->value != '-' && v2->value != '-')
        return lt_values_wrap(v1->next, v2->next, v1->value == v2->value ? smaller : v1->value < v2->value);
    if (v1 && v1->value == '-') return lt_values_wrap(NULL, v2, smaller);
    if (v2 && v2->value == '-') return lt_values_wrap(v1, NULL, smaller);
    if (!v1 && v2->value == '0') return lt_values_wrap(NULL, v2->next, smaller);
    if (!v2 && v1->value == '0') return lt_values_wrap(v1->next, NULL, smaller);
    return v1 == NULL || !(v2 == NULL && v1->value > '0');
}

int lt_values(value_t* v1, value_t* v2) {
    if (eq_values(v1, v2)) return 0;
    char lastV1 = get_last_value(v1)->value;
    char lastV2 = get_last_value(v2)->value;
    if (lastV1 == '-' && lastV2 != '-') return 1;
    if (lastV1 != '-' && lastV2 == '-') return 0;
    int result = lt_values_wrap(v1, v2, 0);
    return lastV1 == '-' && lastV2 == '-' ? !result : result;
}

int addValuesTogetherWrap(value_t** out, value_t* v1, value_t* v2, int carry) {
    if (!v1 && !v2 && carry == 0) return 0;
    int v = carry;
    v += v1 ? v1->value - '0' : 0;
    v += v2 ? v2->value - '0' : 0;
    int NONzero = addValuesTogetherWrap(out, v1 ? v1->next : NULL, v2 ? v2->next : NULL, v / 10);
    if (NONzero || v != 0) {
        insert_char(out, v % 10 + '0');
        return 1;
    }
    return NONzero;
}

int subValuesTogetherWrap(value_t** out, value_t* v1, value_t* v2, int borrow) {
    if (!v1 && !v2 && borrow == 0) return 0;
    int dv1 = v1 ? v1->value - '0' : 0;
    int dv2 = v2 ? v2->value - '0' : 0;
    int v = dv1 - dv2 - borrow;
    if (v < 0) {
        v += 10;
        borrow = 1;
    } else {
        borrow = 0;
    }
    int NONzero = subValuesTogetherWrap(out, v1 ? v1->next : NULL, v2 ? v2->next : NULL, v1 || v2 ? borrow : 0);
    if (NONzero || v != 0) {
        insert_char(out, v % 10 + '0');
        return 1;
    }
    return NONzero;
}

value_t* add_values(value_t* v1, value_t* v2) {
    value_t* out = NULL;
    value_t* secondLastV1 = get_second_last_value(v1);
    int negV1 = 0, negV2 = 0;
    if (secondLastV1 && secondLastV1->next->value == '-') {
        negV1 = 1;
        free(secondLastV1->next);
        secondLastV1->next = NULL;
    }
    value_t* secondLastV2 = get_second_last_value(v2);
    if (secondLastV2 && secondLastV2->next->value == '-') {
        negV2 = 1;
        free(secondLastV2->next);
        secondLastV2->next = NULL;
    }

    if (negV1 == negV2) {
        if (negV1) insert_char(&out, '-');
        int NONzero = addValuesTogetherWrap(&out, v1, v2, 0);
        if (!NONzero) insert_char(&out, '0');
        return out;
    }
    if (eq_values(v1, v2)) {
        insert_char(&out, '0');
        return out;
    }

    int v1Smaller = lt_values(v1, v2);
    if ((negV1 && !v1Smaller) || (negV2 && v1Smaller)) {
        insert_char(&out, '-');
    }

    if (v1Smaller) {
        subValuesTogetherWrap(&out, v2, v1, 0);
    } else {
        subValuesTogetherWrap(&out, v1, v2, 0);
    }
    return out;
}

void add_new_char(stos_t** stos, char value) {
    insert_char(&(*stos)->value, value);
}

void insert_char(value_t** list, char value) {
    value_t* newValue = malloc(sizeof(value_t));
    newValue->value = value;
    newValue->next = *list;
    *list = newValue;
}

void valueFromIntWrap(value_t** list, int number) {
    if (number < 10) {
        insert_char(list, number % 10 + '0');
        return;
    }
    valueFromIntWrap(list, number / 10);
    insert_char(list, number % 10 + '0');
}

value_t* get_last_value(value_t* list) {
    if (list == NULL) return NULL;
    if (list->next == NULL) {
        return list;
    }
    return get_last_value(list->next);
}

value_t* get_second_last_value(value_t* list) {
    if (list == NULL || list->next == NULL) return NULL;
    if (list->next->next == NULL) {
        return list;
    }
    return get_second_last_value(list->next);
}

value_t* value_from_int(int number) {
    value_t* newValue = NULL;
    if (number < 0) {
        insert_char(&newValue, '-');
    }
    valueFromIntWrap(&newValue, abs(number));
    return newValue;
}

void parseValueWrap(value_t* list, int* number, int level) {
    if (list->value == '-') {
        *number *= -1;
    } else {
        *number += (list->value - '0') * level;
        if (list->next != NULL) {
            parseValueWrap(list->next, number, level * 10);
        }
    }
}

int value_to_int(value_t* list) {
    int number = 0;
    parseValueWrap(list, &number, 1);
    return number;
}

void add_last_char(value_t** list, char value) {
    if ((*list)->next == NULL) {
        value_t* newValue = malloc(sizeof(value_t));
        newValue->value = value;
        newValue->next = NULL;
        (*list)->next = newValue;
        return;
    }
    add_last_char(&(*list)->next, value);
}

void add_last_value(value_t** list, value_t* value) {
    if ((*list)->next == NULL) {
        (*list)->next = value;
        return;
    }
    add_last_value(&(*list)->next, value);
}

void removeLastValueWrap(value_t** list, value_t** prev) {
    if ((*list)->next == NULL) {
        free(*list);
        (*prev)->next = NULL;
        return;
    }
    removeLastValueWrap(&(*list)->next, list);
}

void remove_last_value(value_t** list) {
    if (*list == NULL) return;
    if ((*list)->next == NULL) {
        free(*list);
        *list = NULL;
        return;
    }
    removeLastValueWrap(&(*list)->next, list);
}

value_t* clone_value(value_t* list) {
    if (list == NULL) return NULL;
    value_t* newValue = malloc(sizeof(value_t));
    newValue->value = list->value;
    newValue->next = NULL;
    if (list->next == NULL) {
        return newValue;
    }
    newValue->next = clone_value(list->next);
    return newValue;
}

void free_values(value_t* list) {
    if (list == NULL) return;
    free_values(list->next);
    free(list->next);
}

void print_values(value_t* list) {
    if (list == NULL) return;
    printf("%c", list->value);
    print_values(list->next);
}

void printList(stos_t* list, int level) {
    if (list == NULL) return;
    printList(list->next, level + 1);
    printf("%d: ", level);
    if (list->value != NULL) {
        print_values(list->value);
    }
    printf("\n");
}

void print_stos(stos_t* stos) {
    if (stos == NULL) return;
    printList(stos, 0);
}
