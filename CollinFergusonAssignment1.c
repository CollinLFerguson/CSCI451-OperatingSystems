/*
Collin L. Ferguson
Collin.l.ferguson@und.edu
CSCI 451: Operating Systems
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void LOAD(FILE *data) //Hack way of doing this, but after 3 days I give up on the regex.
{
	char *line = NULL;
	char *theLine = "  <font color=\"#FFCC00\" SIZE=+1>\n";
	size_t junk = 0;

	getline(&line, &junk, data);

	while(!feof(data))
	{
		if(!strcmp(theLine,line))
		{
			getline(&line, &junk, data);
			int index = 2;
			char temp = line[index]; //Skip the 3 blank spaces at the beginning.

			while(!(temp == '\0'))
			{
				printf("%c", temp);
				index++;
				temp = line[index];
			}
		}
		getline(&line, &junk, data);
	}
	free(line);
}


int main(void) {
	system("wget -q http://undcemcs01.und.edu/~ronald.marsh/PROJECTS/PROJECTS.HTML");

	//system("grep -P \"((?<= {3})[ A-Za-z0-9.+-]*(?=  <\/font>))" \"PROJECTS.HTML\"");
	// I know this can be done, I just don't know how.

	FILE *data = fopen("PROJECTS.HTML", "r");

	LOAD(data);

	fclose(data);

	system("unlink PROJECTS.HTML");
	return 0;
}
