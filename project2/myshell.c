/*Name: Jose Benitez
Date: February 3, 2023
File Name: myshell.c */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>

void listfiles(char *curdir);
void changedir(char *path);
void printdir();
void copyfile(char *read_path, char *write_path);
void helper_cp(char *input, char *output);
void startprocess(char **argv, int i);
void waitforanychild();
void waitforachild(int label);
void runprocess(char **argv, int i);
void killprocess(int pid);

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

        // Get User Input:
        printf("\nmyshell>\n");

        scanf("%s", command);

        if (strcmp(command, "list-files") == 0)
        {
            listfiles(".");
        }
        else if (strcmp(command, "change-dir") == 0)
        {
            char path[1000];
            scanf("%s", path);
            changedir(path);
        }
        else if (strcmp(command, "print-dir") == 0)
        {
            printdir(".");
        }
        else if (strcmp(command, "copy-file") == 0)
        {
            char source[1000];
            char destination[1000];
            scanf("%s", source);
            scanf("%s", destination);
            copyfile(source, destination);
            printf("copy-file: copied %d bytes from %s to %s\n", bytes_sum, source, destination);
            bytes_sum = 0;
        }
        else if (strcmp(command, "start-process") == 0)
        {
            char *argv[10000]; // array of strings
            char *ptr;
            char arguments[100];

            scanf("%[^\n]s", arguments);
            ptr = strtok(arguments, " ");
            int i = 0;
            while (ptr != NULL)
            {
                argv[i] = ptr;
                ptr = strtok(NULL, " ");
                i++;
            }

            startprocess(argv, i);
        }
        else if (strcmp(command, "wait-for-any-child") == 0)
        {
            waitforanychild();
        }
        else if (strcmp(command, "wait-for-a-child") == 0)
        {
            int label;
            scanf("%d", &label);
            waitforachild(label);
        }
        else if (strcmp(command, "run-process") == 0)
        {
            char *argv[10000]; // array of strings
            char *ptr;
            char arguments[100];

            scanf("%[^\n]s", arguments);
            ptr = strtok(arguments, " ");
            int i = 0;
            while (ptr != NULL)
            {
                argv[i] = ptr;
                ptr = strtok(NULL, " ");
                i++;
            }

            runprocess(argv, i);
        }
        else if (strcmp(command, "kill-process") == 0)
        {
            int pid;
            scanf("%d", &pid);
            killprocess(pid);
        }
        else if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0)
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
        // File Type
        if (subdir->d_type == DT_DIR)
        {
            printf("D ");
        }
        else
        {
            printf("F ");
        }

        // File Name
        printf("%-10s", subdir->d_name);

        struct stat buf;
        if (stat(subdir->d_name, &buf) != 0)
        {
            printf("\n Error opening file\n");
            exit(1);
        }

        // Owner Permissions
        char owner[] = "---";
        if (buf.st_mode & S_IRUSR)
        {
            owner[0] = 'r';
        }
        if (buf.st_mode & S_IWUSR)
        {
            owner[1] = 'w';
        }
        if (buf.st_mode & S_IXUSR)
        {
            owner[2] = 'x';
        }

        // Group Permissions
        char group[] = "---";
        if (buf.st_mode & S_IRGRP)
        {
            group[0] = 'r';
        }
        if (buf.st_mode & S_IWGRP)
        {
            group[1] = 'w';
        }
        if (buf.st_mode & S_IXGRP)
        {
            group[2] = 'x';
        }

        // Other Permissions
        char other[] = "---";
        if (buf.st_mode & S_IROTH)
        {
            other[0] = 'r';
        }
        if (buf.st_mode & S_IWOTH)
        {
            other[1] = 'w';
        }
        if (buf.st_mode & S_IXOTH)
        {
            other[2] = 'x';
        }

        printf("user:%s group:%s other:%s", owner, group, other);

        // File Size
        printf("  %d bytes \n", (int)buf.st_size);
    }
}

void changedir(char *path)
{
    if (path[0] == '/')
    {
        path++;
    }

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

void startprocess(char **argv, int i)
{

    char *subcommand = argv[0];
    char *sliced[i - 1];

    for (int j = 1; j < i; j++)
    {
        sliced[j - 1] = argv[j];
    }

    pid_t pid = fork();
    printf("myshell: process %d started\n", pid);
    if (pid == -1)
    {
        printf("Error getting process id\n");
        exit(1);
    }
    else if (pid == 0)
    {
        // this is the child process

        if (execvp(subcommand, argv) == -1)
        {
            printf("Error executing child process\n");
            exit(1);
        }
    }
    else
    {
        // this is the parent process
        printf("I am the parent process: PID %d\n", pid);
    }
}
void waitforanychild()
{
    pid_t pid = fork();
    int status;
    if (pid == -1)
    {
        printf("Error getting process id\n");
        exit(1);
    }
    else if (pid == 0)
    {
        // this is the child process
        exit(5);
    }
    else
    {
        // this is the parent process
        pid_t wpid = wait(&status);

        if (wpid == -1)
        {
            printf("Error waiting for child process\n");
            exit(1);
        }

        if (WIFEXITED(status))
        {
            printf("myshell: process %d exited normally with status %d\n", wpid, WEXITSTATUS(status));
        }
        else
        {
            printf("process %d exited abnormally with signal number %d.\n", wpid, WEXITSTATUS(status));
        }
    }
}
void waitforachild(int label)
{
    pid_t pid = fork();
    int status;
    if (pid == -1)
    {
        printf("Error getting process PID\n");
        exit(1);
    }
    else if (pid == 0)
    {
        // this is the child process
        exit(0);
    }
    else
    {
        // this is the parent process
        pid_t wpid = wait(&status);

        if (wpid == -1)
        {
            printf("Error waiting for child process\n");
            exit(1);
        }

        if (WIFEXITED(status) && wpid == label)
        {
            printf("myshell: process %d exited normally with status %d\n", wpid, WEXITSTATUS(status));
        }
        else
        {
            printf("No such process. \n");
        }
    }
}

void runprocess(char **argv, int i)
{
    char *subcommand = argv[0];
    char *sliced[i - 1];
    int status;

    for (int j = 1; j < i; j++)
    {
        sliced[j - 1] = argv[j];
    }

    pid_t pid = fork();
    printf("myshell: process %d started\n", pid);
    if (pid == -1)
    {
        printf("Error getting process id\n");
        exit(1);
    }
    else if (pid == 0)
    {
        // this is the child process

        if (execvp(subcommand, argv) == -1) // Step 3: Execute the child process
        {
            printf("Error executing child process\n");
            exit(1);
        }
        exit(0); // Step 4: Exit the child process
    }
    else
    {
        // this is the parent process

        int label = pid; // Step 1: Get the process id of the child process

        pid_t wpid = wait(&status); // Step 2: Wait for the child process to finish

        if (wpid == -1)
        {
            printf("Error waiting for child process\n");
            exit(1);
        }

        if (WIFEXITED(status) && wpid == label)
        {
            printf("myshell: process %d exited normally with status %d\n", wpid, WEXITSTATUS(status));
        }
        else
        {
            printf("No such process. \n");
        }
    }
}

void killprocess(int pid)
{
    if (kill(pid, SIGSTOP) == -1)
    {
        printf("Unable to kill process %d\n", pid);
        exit(1);
    }
    else
    {
        printf("Process %d has been killed\n", pid);
    }
}
