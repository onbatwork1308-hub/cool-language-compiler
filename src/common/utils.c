#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

bool isDigit(char c) {
    int i = c - '0';
    return (i >= 0 && i < 10);
}

bool isAlpha(char c) {
    return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'));
}

bool isAlphaNum(char c) {
    return (isDigit(c) || isAlpha(c));
}

bool isUpper(char c) {
    if (!isAlpha(c)) {
        printf("Invalid argument! passed %c, expected:[a-zA-Z].\n", c);
        exit(1);
    }

    return ('A' <= c && c <= 'Z');
}

bool isLower(char c) {
    if (!isAlpha(c)) {
        printf("Invalid argument! passed %c, expected:[a-zA-Z].\n", c);
        exit(1);
    }

    return ('a' <= c && c <= 'z');
}