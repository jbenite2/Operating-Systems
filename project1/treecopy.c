/*Name: Jose Benitez
Date: January 24, 2023
File Name: treecopy.c */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

// Globals
int bytes_sum = 0;
int files_sum = 0;
int dir_sum = 0;

void filecopy(char *input, char *output);
void recursive(char *read_path, char *write_path);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Wrong number of inputs entered. Try again. \n");
        exit(1);
    }

    struct stat buf;

    // If the target is a path that already exists then finish the program
    if (stat(argv[2], &buf) == 0)
    {
        printf("couldn't open directory %s: File Exists.\n", argv[2]);
        exit(1);
    }

    // If the input file/folder is a directory we perform a recursive copy (cp -r)
    if (stat(argv[1], &buf) == 0 && S_ISDIR(buf.st_mode))
    {
        char *input = argv[1];
        char *output = argv[2];
        int rc = mkdir(output, 511);
        if (rc == -1)
        {
            printf("Unable to create directory.");
            exit(1);
        }
        recursive(input, output);
    }
    // If the input file/folder is a regular file
    else if (stat(argv[1], &buf) == 0 && S_ISREG(buf.st_mode) != 0)
    {
        char *input = argv[1];
        char *output = argv[2];
        filecopy(input, output);
    }
    else
    {
        printf("The file %s is not a file or directory.", argv[1]);
    }

    printf("copied %d directories, %d files, and %d bytes from %s to %s.\n", dir_sum, files_sum, bytes_sum, argv[1], argv[2]);

    return 0;
}

void filecopy(char *input, char *output)
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
            int bytes_written = write(target, &buffer[bytes_written], mod_bytes);
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
    files_sum = files_sum + 1;
}

void recursive(char *read_path, char *write_path)
{
    DIR *dp;
    struct dirent *dirp;

    struct stat buf;
    stat(read_path, &buf);

    if (S_ISREG(buf.st_mode)) // S_ISREG() returns true this means it is a valid file
    {
        filecopy(read_path, write_path);
        return;
    }
    if ((dp = opendir(read_path)) == NULL) // if there is an error reading in a directory
    {
        printf("Error opening directory\n");
        exit(1);
    }

    mkdir(write_path, buf.st_mode);

    dir_sum = dir_sum + 1;

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

        recursive(nsbuf, ndbuf);
    }
    closedir(dp);
}