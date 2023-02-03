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
void printdir();

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
        printf("\nmyshell>\n");

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
        else if (strcmp(command, "print-dir") == 0)
        {
            printdir(".");
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

    if ((dp = opendir(curdir)) == NULL)
    {
        printf("Error opening directory\n");
        exit(1);
    }

    while ((subdir = readdir(dp)) != NULL)
    {
        // struct stat *buf;
        // if (stat(subdir, &buf) == 0)
        // {
        //     printf("Error opening file\n");
        //     exit(1);
        // }

        printf("%s\n", subdir->d_name);
    }
}

void changedir(char *path)
{
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

void printdir()
{
    char newdir[1000];
    if (getwd(newdir) == NULL)
    {
        printf("Error getting current directory\n");
        exit(1);
    }
    else
    {
        printf("Current directory is %s\n", newdir);
    }
}