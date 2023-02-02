/*Name: Jose Benitez
Date: February 1, 2023
File Name: myshell.c */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

void listfiles();

int main(int argc, char *argv[])
{
    // Error Check:
    if (argc != 1) // Addressing the case where the user passes inputs
    {
        printf("No arguments are required just yet.");
        exit(1);
    }

    // Initialize Variables:
    char command[1000];

    // Get User Input:
    // printf("myshell>\n");

    scanf("%s", command);

    if (strcmp(command, "listfiles") == 0)
    {
        listfiles();
    }

    return 0;
}

void listfiles()
{
    DIR *dp;
    struct dirent *subdir;

    if ((dp = opendir(".")) == NULL) // if there is an error reading in a directory
    {
        printf("Error opening directory\n");
        exit(1);
    }

    while ((subdir = readdir(dp)) != NULL)
    {

        if (strcmp(subdir->d_name, ".") == 0 || strcmp(subdir->d_name, "..") == 0)
        {
            continue;
        }

        printf("%s\n", subdir->d_name);
    }
}
