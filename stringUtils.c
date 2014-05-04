#include "stringUtils.h"


bool charIncludedInString(char c, char * string)
{
	for(int i=0; i<strlen(string); i++)
	{
		if(c == string[i])
			return true;
	}
	return false;
}


char *xstrtok(char *line, char *delims)
{
	//Custom strtok function. Improvements:
	//1. Identifies consecutive delimiters
	//2. Does not modify the original string

	static char *saveline = NULL;
	int n;

	if(line != NULL)
	{
		saveline = malloc(sizeof(char*));
		strcpy(saveline, line);
		int i = 0;
		while(charIncludedInString(saveline[i++], delims));
		saveline += i-1;
	}
	/*
	 *see if we have reached the end of the line
	 */
	if(saveline == NULL || *saveline == '\0')
		return(NULL);

	/*
	 *return the number of characters that aren't delims
	 */
	n = strcspn(saveline, delims);
	//p = saveline; /*save start of this token*/
	char p[n+1];
	strncpy(p, saveline, n);
	p[n] = '\0';
	saveline += n; /*bump past the delim*/

	if(*saveline != '\0') /*trash the delim if necessary*/
		*saveline++ = '\0';

	return(p);
}

bool xstrcmp(char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL || strlen(str1) != strlen(str2))
        return false;
    int i = 0;
    while(str1[i] != '\0')
    {
        if(str1[i] != str2[i])
            return false;
        i++;
    }
    return true;
}

