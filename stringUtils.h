#ifndef STRINGUTILS_H_INCLUDED
#define STRINGUTILS_H_INCLUDED

#include <string.h>
#include <stdbool.h>

bool charIncludedInString(char , char *);
char *xstrtok(char *, char *);
bool xstrcmp(char *, char*);


#endif // STRINGUTILS_H_INCLUDED
