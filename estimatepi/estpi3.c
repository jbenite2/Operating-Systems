//   P Flynn, last revised 8-Feb-2023
//
//   This program attempts to estimate PI by generating uniformly distributed
//   (x,y) points inside a unit square centered at (0.5, 0.5) and multiplying
//   the proportion of points within the unit-radius quarter-circle
//   centered at the origin  by 4.0.
//   It's embarrassingly parallel - so this program supports threading.
//   On the command line, the user specifies the number of threads
//   desired, and the number of 2D points to generate in each thread.
//   
//   In this version, we use a mutex to protect the critical section - but in a way that
//   serializes the computation - which thereby cancels any benefit from threading.
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

// these globals are updated by all threads
static volatile unsigned long int n=0, nin=0;

// we use a mutex to protect access to the critical section
// (make sure to initialize it before using it!)
pthread_mutex_t lock;

// WORKER
//
// the threads spawned in main() run this function, which takes
// a pointer to the number of points to be generated in this thread
//
// Note: random number generation using the libc srandom routine maintains state and
// is not thread-safe. Use the reentrant versions and maintain thread-local state.
//
void worker(unsigned long int *pnit) {
    unsigned long int nit = *pnit;
    struct drand48_data buf;

    // get a microsecond-level initializer for the random number gen.
    // the seed is a nice big integer, and timing variations *should*
    // ensure that no two threads will initialize at the exact same microsecond.
    // If we're worried about this, perhaps re-place with a hash of the seed
    // plus the thread-ID?
    struct timeval time;
    int rc = gettimeofday(&time,NULL); 
    if (rc<0) { perror("gettimeofday"); exit(-1); };
    unsigned int seed = time.tv_sec * 1000000 + time.tv_usec;

    // initialize random number state in local storage
    rc = srand48_r(seed,&buf);
    if (rc != 0) { perror("srand48_r"); exit(-1); }

    printf("Thread 0x%lx starting its work\n",pthread_self());

    // generate the desired number of points.
    for(unsigned long int i=0; i<nit; i++) {
        double x,y;
        // use drand48_r to generate x and y
        rc = drand48_r(&buf,&x);
        if (rc != 0) { perror("drand48_r"); exit(-1); }
        rc = drand48_r(&buf,&y);
        if (rc != 0) { perror("drand48_r"); exit(-1); }

  	// calculate distance to origin and increment nin if dist <= 1.0
  	double r = x*x + y*y;
  	
    	// protect the critical section!
    	rc = pthread_mutex_lock(&lock);
    	if (rc != 0) { perror("pthread_mutex_lock"); exit(-1); }

  	// CRITICAL SECTION (update two shared variables)
  	if (r<=1.0) nin++;
  	// increment count of points generated
  	n++;
  	// END OF CRITICAL SECTION
	
    	// unprotect the critical section
    	rc = pthread_mutex_unlock(&lock);
    	if (rc != 0) { perror("pthread_mutex_unlock"); exit(-1); }
  	}


    printf("Thread 0x%lx completed its work\n",pthread_self());

    return;
 }
   
int main(int argc,char *argv[]) {
 pthread_t *threads;
 // check args
 if (argc != 3) {
  printf("Usage: %s nthreads nest\n",argv[0]);
  exit(-1);
  }

 // parse args
 int nthr = atoi(argv[1]); // number of threads to create
 unsigned long int nest = atoi(argv[2]); // number of estimates in each thr

 // we are using a mutex and we MUST initialize it.
 // you can use a default initializer, or you can call a function to do it.
 // I do the latter, for no particular reason...
 int rc = pthread_mutex_init(&lock,NULL);
 if (rc != 0) { perror("pthread_mutex_init"); exit(-5); }

 // allocate vector of thread-IDs (we need them because we have to join
 // them)
 threads = (pthread_t *)malloc(nthr*sizeof(pthread_t));
 if (threads == NULL) { perror("malloc"); exit(-2); }

 // fire off the threads - note how worker arg (&nest) is passed.
 for (int i=0; i<nthr; i++) {
  int rc = pthread_create(&threads[i], NULL, (void *)worker, &nest);
  if (rc != 0) { perror("pthread_create"); exit(-3); }
  };

 // wait for the threads to finish.
 for (int i=0; i<nthr; i++) {
  int rc = pthread_join(threads[i],NULL);
  if (rc != 0) { perror("pthread_join"); exit(-4); }
  };

 double pie = 4.0*nin/(1.0*n);

 printf("%ld points, %ld inside: estimate is %f\n",n,nin,pie);
 }
