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
void copyfile(char *read_path, char *write_path);
void helper_cp(char *input, char *output);

int bytes_sum = 0;

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
        else if (strcmp(command, "copy-file") == 0)
        {
            char source[1000];
            char destination[1000];
            scanf("%s", source);
            scanf("%s", destination);
            copyfile(source, destination);
            printf("copied %d bytes from %s to %s\n", bytes_sum, source, destination);
            bytes_sum = 0;
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

void copyfile(char *read_path, char *write_path)
{
    DIR *dp;
    struct dirent *dirp;

    struct stat buf;
    stat(read_path, &buf);

    if (S_ISREG(buf.st_mode)) // S_ISREG() returns true this means it is a valid file
    {
        helper_cp(read_path, write_path);
        return;
    }
    if ((dp = opendir(read_path)) == NULL) // if there is an error reading in a directory
    {
        printf("Error opening directory\n");
        exit(1);
    }

    mkdir(write_path, buf.st_mode);

    while ((dirp = readdir(dp)) != NULL)
    {
        char nsbuf[4096];
        char ndbuf[4096];

        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
        {
            continue;
        }

        sprintf(nsbuf, "%s/%s", read_path, dirp->d_name);
        sprintf(ndbuf, "%s/%s", write_path, dirp->d_name);

        copyfile(nsbuf, ndbuf);
    }
    closedir(dp);
}

void helper_cp(char *input, char *output)
{
    // Variable Assignment
    char buffer[4096];
    int source = open(input, O_RDONLY);
    if (source == -1) // If input file doesn't exists
    {
        printf("couldn't open %s: No such file or directory.\n", input);
        exit(1);
    }

    int target = open(output, O_RDWR | O_EXCL | O_CREAT, 0666);
    if (target == -1) // If output file already exists finish the program
    {
        printf("The output file already exists\n");
        exit(1);
    }

    int bytes = 0;

    while (1)
    {
        // Get the bytes possibly up to the size of the buffer
        int bytes_read = read(source, buffer, sizeof(buffer));

        // If bytes read in this iterations are none then break out of the loop
        if (bytes_read <= 0)
        {
            break;
        }

        // Write to the target file and return the number of bytes written
        int bytes_written = write(target, buffer, bytes_read);

        // Managing the error where the write function falls short of the bytes to be written
        if (bytes_read != bytes_written)
        {
            int mod_bytes = bytes_read - bytes_written;
            bytes_written = write(target, &buffer[bytes_written], mod_bytes);
        }
        bytes = bytes + bytes_read;
    }

    // printf("copied %d bytes from %s to %s\n", bytes, input, output);
    printf("%s -> %s\n", input, output);

    // Finally close used files
    close(source);
    close(target);

    // Modifying global
    bytes_sum = bytes_sum + bytes;
}
