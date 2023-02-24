/*
Name: Jose Benitez
Date: February 24, 2023
fractaltask.c - Load Balancing with Tasks
*/

#include "gfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

// Global Variables, Arrays, and Mutexes
pthread_mutex_t lock;
pthread_mutex_t newlock;
int nthreads = 0;
int isVisited[480][640 / 20];
int num;
char str[2];

typedef struct
{
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    int maxiter;
    int start;
    int end;
} thread_arguments;

// Function to compute the points
static int compute_point(double x, double y, int max)
{
    double complex z = 0;
    double complex alpha = x + I * y;

    int iter = 0;

    while (cabs(z) < 4 && iter < max)
    {
        z = cpow(z, 2) + alpha;
        iter++;
    }

    return iter;
}

// Threaded version 'compute_image' that works by writing each point to the given bitmap.
void *t_compute_image(void *arg)
{

    // Reinitializing key variables
    int width = gfx_xsize();
    int height = gfx_ysize();
    int xmin;
    int xmax;
    int ymin;
    int ymax;

    // Casting the pointer 'arg' to make sure the struct is of type 'thread_arguments' and the function can access it
    // (the reason we do this is because the function 'pthread_create' takes a void pointer as an argument and we need to cast it back to the original type)
    thread_arguments *targs = (thread_arguments *)arg;

    // For every pixel i,j, in the image after the division of the sections by the constant 20...
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width / 20; j++)
        {
            if (isVisited[i][j])
            {
                continue;
            }

            pthread_mutex_lock(&newlock);
            isVisited[i][j] = 1;
            xmax = (i + 1) * 20;
            xmin = i * 20;
            ymax = (j + 1) * 20;
            ymin = j * 20;
            pthread_mutex_unlock(&newlock);

            // For every pixel i,j, in the image...

            for (int m = xmin; m < xmax; m++)
            {
                for (int p = ymin; p < ymax; p++)
                {

                    // Scale from pixels i,j to coordinates x,y
                    double x = targs->xmin + p * (targs->xmax - targs->xmin) / width;
                    double y = targs->ymin + m * (targs->ymax - targs->ymin) / height;

                    // Compute the iterations at x,y
                    int iter = compute_point(x, y, targs->maxiter);

                    // Convert a iteration number to an RGB color.
                    // (Change this bit to get more interesting colors.)
                    int gray = 255 * iter / targs->maxiter;

                    pthread_mutex_lock(&lock);
                    gfx_color(gray, gray, gray);

                    // Plot the point on the screen.
                    gfx_point(p, m);
                    pthread_mutex_unlock(&lock);
                }
            }
        }
    }

    return NULL;
}

void compute_image(double xmin, double xmax, double ymin, double ymax, int maxiter)
{
    int i, j;

    int width = gfx_xsize();
    int height = gfx_ysize();

    // For every pixel i,j, in the image...

    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {

            // Scale from pixels i,j to coordinates x,y
            double x = xmin + i * (xmax - xmin) / width;
            double y = ymin + j * (ymax - ymin) / height;

            // Compute the iterations at x,y
            int iter = compute_point(x, y, maxiter);

            // Convert a iteration number to an RGB color.
            // (Change this bit to get more interesting colors.)
            int gray = 255 * iter / maxiter;
            gfx_color(gray, gray, gray);

            // Plot the point on the screen.
            gfx_point(i, j);
        }
    }
}

int main(int argc, char *argv[])
{

    // The initial boundaries of the fractal image in x,y space.
    double xmin = -1.5;
    double xmax = 0.5;
    double ymin = -1.0;
    double ymax = 1.0;

    // Maximum number of iterations to compute.
    // (Higher values take longer but have more detail)
    int maxiter = 100;

    // Open a new window.
    gfx_open(640, 480, "Mandelbrot Fractal");

    // Show the configuration, just in case you want to recreate it.
    printf("coordinates: %lf %lf %lf %lf\n", xmin, xmax, ymin, ymax);

    // Fill it with a dark blue initially.
    gfx_clear_color(0, 0, 255);
    gfx_clear();

    // Display the fractal image
    compute_image(xmin, xmax, ymin, ymax, maxiter);

    while (1)
    {
        // Wait for a key or mouse click.
        int c = gfx_wait();

        switch (c)
        {
        // Quit if q is pressed
        case 'q':
            exit(0);
        // Recenter the image if the mouse is clicked
        case 1:
            gfx_clear();

            int x_center = gfx_xpos();
            int y_center = gfx_ypos();

            double oldx = (xmax + xmin) / 2;
            double oldy = (ymax + ymin) / 2;

            double newx = xmin + x_center * (xmax - xmin) / gfx_xsize();
            double newy = ymin + y_center * (ymax - ymin) / gfx_ysize();

            double xdiff = oldx - newx;
            double ydiff = oldy - newy;

            xmin -= xdiff;
            xmax -= xdiff;
            ymin -= ydiff;
            ymax -= ydiff;

            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        // Zoom in if i is pressed
        case 'i':
            gfx_clear();
            xmin /= 2;
            xmax /= 2;
            ymin /= 2;
            ymax /= 2;
            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        // Zoom out if o is pressed
        case 'o':
            gfx_clear();
            xmin *= 2;
            xmax *= 2;
            ymin *= 2;
            ymax *= 2;
            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        // Move right if r is pressed
        case 'r':
            gfx_clear();
            double shiftright = (xmax - xmin) / 10;
            xmin += shiftright;
            xmax += shiftright;
            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        // Move left if l is pressed
        case 'l':
            gfx_clear();
            double shiftleft = (xmax - xmin) / 10;
            xmin -= shiftleft;
            xmax -= shiftleft;
            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        // Move up if u is pressed
        case 'u':
            gfx_clear();
            double shiftup = (xmax - xmin) / 10;
            ymin -= shiftup;
            ymax -= shiftup;
            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        // Move down if d is pressed
        case 'd':
            gfx_clear();
            double shiftdown = (xmax - xmin) / 10;
            ymin += shiftdown;
            ymax += shiftdown;
            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        // Decrease the number of iterations if - is pressed
        case '-':
            gfx_clear();
            maxiter /= 0.5;
            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        // Increase the number of iterations if + is pressed
        case '+':
            gfx_clear();
            maxiter *= 0.5;
            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        // Specify number of threads
        case '1' ... '8':

            gfx_clear();

            // Changing String -> Integer
            str[0] = c;
            str[1] = '\0';
            sscanf(str, "%d", &num);

            // Initialize variables, arrays, and structs
            nthreads = num;
            pthread_t threads[nthreads];
            int divide = gfx_ysize() / nthreads;
            thread_arguments *arg = (thread_arguments *)malloc(nthreads * sizeof(thread_arguments));

            // Clear the previous screen and reset the image
            gfx_clear();

            // Initialize the isVisited array to keep track of which sections have been colored by threads
            for (int i = 0; i < (480 / 20); i++)
            {
                for (int j = 0; j < (640 / 20); j++)
                {
                    isVisited[i][j] = 0;
                }
            }

            // Initialize the lock
            pthread_mutex_init(&lock, NULL);
            pthread_mutex_init(&newlock, NULL);

            // Create the threads
            for (int i = 0; i < nthreads; i++)
            {
                arg[i].xmin = xmin;
                arg[i].xmax = xmax;
                arg[i].ymin = ymin;
                arg[i].ymax = ymax;
                arg[i].maxiter = maxiter;
                arg[i].start = i * divide;
                arg[i].end = (i + 1) * divide;

                pthread_create(&threads[i], NULL, t_compute_image, (void *)&arg[i]);
            }

            // Join the threads
            for (int i = 0; i < nthreads; i++)
            {
                pthread_join(threads[i], NULL);
            }

            // Destroy locks and free memory
            pthread_mutex_destroy(&lock);
            pthread_mutex_destroy(&newlock);
            break;
        }
    }

    return 0;
}
