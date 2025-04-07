#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main(void) {
    stos_t* stos = NULL;
    char program[20002];
    fgets(program, 20002, stdin);
    int current = 0;
    int running = 1;

    while (running) {
        char val = program[current];
        switch (val) {
            case '\0':
            case '\n':
                running = 0;
                break;
            case '\'':
                add_new_value(&stos);
                break;
            case ',':
                remove_value(&stos);
                break;
            case ':':
                clone_value_at(&stos, 0);
                break;
            case ';':
                swap_values(&stos);
                break;
            case '@':
                int copyFromIndex = pop_parse_value(&stos);
                clone_value_at(&stos, copyFromIndex);
                break;
            case '.':
                char c;
                scanf("%c", &c);
                add_new_char(&stos, c);
                break;
            case '>':
                value_t* lastValue = pop_value(&stos);
                printf("%c", lastValue->value);
                free_values(lastValue);
                break;
            case '!':
                if (stos->value == NULL) {
                    add_new_char(&stos, '1');
                } else if (stos->value->value == '0' && stos->value->next == NULL) {
                    replace_value(&stos, value_from_int(1));
                } else {
                    replace_value(&stos, value_from_int(0));
                }
                break;
            case '<':
                value_t* ltA = pop_value(&stos);
                value_t* ltB = pop_value(&stos);
                if (lt_values(ltB, ltA)) insert_value(&stos, value_from_int(1));
                else insert_value(&stos, value_from_int(0));
                free_values(ltA);
                free_values(ltB);
                break;
            case '=':
                value_t* eqA = pop_value(&stos);
                value_t* eqB = pop_value(&stos);
                if (eq_values(eqA, eqB)) insert_value(&stos, value_from_int(1));
                else insert_value(&stos, value_from_int(0));
                free_values(eqA);
                free_values(eqB);
                break;
            case '~':
                insert_value(&stos, value_from_int(current));
                break;
            case '?':
                int newCurrent = pop_parse_value(&stos) - 1;
                value_t* cmpValue = pop_value(&stos);
                if (cmpValue != NULL && (cmpValue->next != NULL || cmpValue->value != '0')) {
                    current = newCurrent;
                }
                free_values(cmpValue);
                break;
            case '-':
                value_t* lastValueNeg = get_last_value(stos->value);
                if (lastValueNeg == NULL) add_new_char(&stos, val);
                else if (lastValueNeg->value == '-') remove_last_value(&stos->value);
                else add_last_char(&stos->value, '-');
                break;
            case '^':
                value_t* lastValueAbs = get_last_value(stos->value);
                if (lastValueAbs != NULL && lastValueAbs->value == '-') remove_last_value(&stos->value);
                break;
            case '$':
                value_t* firstChar = stos->value;
                stos->value = stos->value->next;
                firstChar->next = NULL;
                insert_value(&stos, firstChar);
                break;
            case '#':
                value_t* lastValueConnect = pop_value(&stos);
                add_last_value(&stos->value, lastValueConnect);
                break;
            case '+':
                value_t* addA = pop_value(&stos);
                value_t* addB = pop_value(&stos);
                insert_value(&stos, add_values(addA, addB));
                free_values(addA);
                free_values(addB);
                break;
            case '&':
                print_stos(stos);
                break;
            case ']':
                int newChar = pop_parse_value(&stos);
                value_t* newValueChar = malloc(sizeof(value_t));
                newValueChar->value = (char) newChar;
                newValueChar->next = NULL;
                insert_value(&stos, newValueChar);
                break;
            case '[':
                value_t* newNum = pop_value(&stos);
                char newInt = newNum->value;
                insert_value(&stos, value_from_int(newInt));
                break;
            default:
                add_new_char(&stos, val);
                break;
        }
        current++;
    }

    free_stos(&stos);
    return 0;
}