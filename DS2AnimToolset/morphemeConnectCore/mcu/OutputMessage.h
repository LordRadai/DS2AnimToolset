#pragma once
#include <stdio.h>

#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GRAY "\033[90m"

#define RESET "\033[0m"

#define PRINT_MESSAGE(color, message) \
	do { \
		printf("%s%s%s\n", color, message, RESET); \
	} while (0)

#define PRINT_WMESSAGE(color, message) \
	do { \
		wprintf("%s%s%s\n", color, message, RESET); \
	} while (0)
