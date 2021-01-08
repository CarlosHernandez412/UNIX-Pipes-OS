/* dotprod.c
 *     $ gcc -o dotprod dotprod.c -lpthread
 *     $ ./dotprod
 *     80200         sum(i=1 to 400) i = 80200
 *
 *  This threaded program computes the algebraic dot product of two vectors.
 *
 *  For example:
 *        a = <2,3,4>  and  b = <-1,3,5>
 *        _   _ 
 *        a . b = (2*-1)+(3*3)+(4*5) = -2+9+20 = 27.
 *
 *  A mutex enforces mutual exclusion on the shared sum: lock before 
 *  updating and unlock after updating.
 *
 *  The main program creates threads which do all the work and then print out 
 *  result upon completion. Before creating the threads, the input data is 
 *  created. The main thread needs to wait for all threads to complete, it 
 *  waits for each one of the threads. We specify a thread attribute value that 
 *  allow the main thread to join with the threads it creates. Note also that 
 *  we free up handles when they are no longer needed.
 *	 
 *  Each thread works on a different set of data. The offset is specified by 
 *  the loop variable 'i'. The size of the data for each thread is indicated by 
 *  VECLEN.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
   int      *a;
   int      *b;
   long     sum; 
   int      veclen; 
} Dotdata;

#define NUMTHRDS    6
#define VECLEN    100

Dotdata dotstr;               /* global so all threads can see and use it */

pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;     /* use a mutex to protect the dot product */

/* thread function */
void *dotprod(void *arg)
{
   int i, start, end, len;
   unsigned long offset;
   int mysum, *x, *y;
   offset = (unsigned long)arg;

   printf("Thread offset: %lu\n", offset);
 
   len = dotstr.veclen;
   start = offset*len;
   end   = start + len;
   x = dotstr.a;
   y = dotstr.b;

   mysum = 0;
   for (i=start; i < end ; i++) 
   {
      mysum += (x[i] * y[i]);
   }
   pthread_mutex_lock(&mutexsum);
   dotstr.sum += mysum;
   pthread_mutex_unlock(&mutexsum);
   pthread_exit((void*) 0);
}


int main (int argc, char *argv[])
{
   unsigned long i;
   int *a, *b;
   void *status;
   pthread_attr_t attr;

   printf("sizeof(size_t) is %d\n", sizeof(size_t));

   /* Assign storage and initialize values in the vectors */
   /* Security Note from Melissa: 
    *
    * Don't use an allocation like this (passing a math expression to malloc())
    * with user-provided parameters without first verifying that the product 
    * fits in size_t space. 
    *
    * A user could provide a number that pushes the product above the size_t 
    * max, at which point the calculation to malloc() would overflow and wrap 
    * back around to the start of the range.
    *
    * For example, if the size_t max is 2^64 - 1 and the product is 2^64 + 8, 
    * malloc() would only allocate 8 bytes due to the overflow on multiply. 
    * But the initialization loop here in main and the thread function would 
    * assume there are 2^64 + 8 bytes and would go around clobbering a LARGE 
    * portion of virtual memory that isn't actually allocated to the vectors.*/

   a = (int*) malloc (NUMTHRDS*VECLEN*sizeof(int));
   b = (int*) malloc (NUMTHRDS*VECLEN*sizeof(int));

   for (i=0; i < VECLEN*NUMTHRDS; i++)
   {
      a[i] = 1;
      b[i] = (i+1); /* compute the sum of integers from 1 to VECLEN*NUMTHRDS*/  
   }

   dotstr.veclen = VECLEN; 
   dotstr.a = a; 
   dotstr.b = b; 
   dotstr.sum=0;

   pthread_mutex_init(&mutexsum, NULL);
         
   /* create NUMTHRDS threads */
   for(i=0; i < NUMTHRDS; i++)
   {
      pthread_create(&callThd[i], NULL, dotprod, (void *)i);
   }

   /* wait on all the threads to finish */
   for(i=0; i<NUMTHRDS; i++)
   {
      pthread_join(callThd[i], &status);
   }

   /* now you can safely print out the results and cleanup */
   printf ("Sum =  %ld \n", dotstr.sum);
   free (a);
   free (b);
   pthread_mutex_destroy(&mutexsum);
   return 0;
}  /* end main */
