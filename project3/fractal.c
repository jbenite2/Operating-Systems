/*
fractal.c - Sample Mandelbrot Fractal Display
Starting code for CSE 30341 Project 3.
*/

#include "gfx.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <complex.h>

/*
Compute the number of iterations at point x, y
in the complex space, up to a maximum of maxiter.
Return the number of iterations at that point.

This example computes the Mandelbrot fractal:
z = z^2 + alpha

Where z is initially zero, and alpha is the location x + iy
in the complex plane.  Note that we are using the "complex"
numeric type in C, which has the special functions cabs()
and cpow() to compute the absolute values and powers of
complex values.
*/

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

/*
Compute an entire image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax).
*/

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
    // Higher values take longer but have more detail.
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
        case '-':
            gfx_clear();

            maxiter /= 0.5;

            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;

        case '+':
            gfx_clear();

            maxiter *= 0.5;

            compute_image(xmin, xmax, ymin, ymax, maxiter);
            break;
        }
    }

    return 0;
}
