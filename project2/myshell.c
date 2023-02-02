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

void listfiles(char *curdir);
void changedir(char *path);

int main(int argc, char *argv[])
{
    // Error Check:
    if (argc != 1) // Addressing the case where the user passes inputs
    {
        printf("No arguments are required just yet.");
        exit(1);
    }

    while (1)
    {
        // Initialize Variables:
        char command[1000];
        char path[1000];

        // Get User Input:
        printf("myshell>\n");

        scanf("%s", command);

        if (strcmp(command, "list-files") == 0)
        {
            listfiles(".");
        }
        else if (strcmp(command, "change-dir") == 0)
        {
            scanf("%s", path);
            changedir(path);
        }
        else if (strcmp(command, "exit") == 0)
        {
            exit(0);
        }
        else
        {
            printf("\nInvalid Command\n");
        }
    }

    return 0;
}

void listfiles(char *curdir)
{
    DIR *dp;
    struct dirent *subdir;

    if ((dp = opendir(curdir)) == NULL) // if there is an error reading in a directory
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

void changedir(char *path)
{
    printf("%s", path);
    if (chdir(path) == -1)
    {
        printf("Error changing directory\n");
        exit(1);
    }
    else
    {
        printf("Directory changed to %s\n", path);
    }
}