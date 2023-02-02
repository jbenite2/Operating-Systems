/*Name: Jose Benitez
Date: January 24, 2023
File Name: filecopy.c */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // Error Handling
    if (argc != 3)
    {
        printf("Please enter a valid number of input arguments.");
        return 1;
    }

    // Variable Assignment
    char buffer[4096];
    int source = open(argv[1], O_RDONLY);
    if (source == -1) // If input file doesn't exists
    {
        printf("couldn't open %s: No such file or directory.\n", argv[1]);
        exit(1);
    }

    int target = open(argv[2], O_RDWR | O_EXCL | O_CREAT, 0666);
    if (target == -1) // If output file already exists finish the program
    {
        printf("The output file already exists\n");
        exit(1);
    }

    int bytes = 0;

    // Procedure
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

    printf("copied %d bytes from %s to %s\n", bytes, argv[1], argv[2]);

    // Finally close used files
    close(source);
    close(target);

    return 0;
}
