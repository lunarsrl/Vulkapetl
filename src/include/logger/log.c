#include "log.h"
#include <stdio.h>
#include <stdlib.h>


#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BRED "\e[1;31m"
#define reset "\e[0m"

void error(char* message) {
    char* tag = "[Error]";
    printf(RED "%s: %s\n" reset, tag, message);
}
void warning(char* message) {
    char* tag = "[Warning]";
    printf(YEL "%s: %s\n" reset, tag, message);
}
void info(char* message) {
    char* tag = "[Info]";
    printf( reset "%s: %s\n" reset, tag, message);
}

void success(char* message) {
    char* tag = "[SUCCESS]";
    printf(GRN "%s: %s\n" reset, tag, message);
}

void panic(char* message) {
    char* tag = "[PANIC!]";
    printf(BRED "%s: %s\n" reset, tag, message);
    exit(1);
}