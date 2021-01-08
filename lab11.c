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

 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      /* header file for the POSIX API */
#include <string.h>      /* string handling functions */
#include <errno.h>       /* for perror() call */
#include <pthread.h>     /* POSIX threads */ 
#include <sys/ipc.h>     /* SysV IPC header */
#include <sys/sem.h>     /* SysV semaphore header */
#include <sys/syscall.h> /* Make a syscall() to retrieve our TID */
#include <sys/wait.h>
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

int pipefd[2];
 char buf [20];
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
   memset(buf, 0, sizeof(buf));
   sprintf(buf,"%d",mysum);
	write(pipefd[1], buf , sizeof(buf));
   pthread_mutex_unlock(&mutexsum);
   pthread_exit((void*) 0);
}


int main (int argc, char *argv[])
{
   unsigned long i;
   int *a, *b;
   void *status;
   int ChildStatus=0;
   pthread_attr_t attr;
   
  
   //printf("sizeof(size_t) is %d\n", sizeof(size_t));
   
    if (pipe(pipefd) < 0) {     /* create the pipe before the fork */
       perror("pipe");
       exit(1);
   }
   
   
  /* CHILD reads from the pipe */
   
   
   pid_t cpid;
   cpid = fork();
   if (cpid == -1) {
       perror("fork");
       exit(EXIT_FAILURE);
   }
   
   if (cpid == 0) {         
       close(pipefd[1]);	  /* close unused write end */
	   
	   int ChildSum = 0;

       /* by default a read blocks on the pipe; read returns num bytes read */
	   memset (buf, 0, sizeof(buf));
       while (read(pipefd[0], buf, sizeof(buf)) > 0){
   	      ChildSum += atoi(buf);
	   }
	   printf("The child sum is %d\n", ChildSum);
       //write(STDOUT_FILENO, "\n", 1);
       close(pipefd[0]);  /* close read end */
       exit(EXIT_SUCCESS);

   } 
    /* PARENT writes argv[1] to pipe outgoing end */
   else  {

       close(pipefd[0]);	  /* close unused read end */
   }
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
   /* close the pipe - this sends EOF down the pipe */
       close(pipefd[1]);
	   
/* now you can safely print out the results and cleanup */
   printf ("Sanity check in parent. dotstr.sum =  %ld \n", dotstr.sum);
 
 
   wait(&ChildStatus); //wait for child to exit 
	
	if (WIFEXITED(ChildStatus))
		printf("child exited with exit code: %d\n", WEXITSTATUS(ChildStatus));
   
  // write(1, buf, strlen(buf));
   free (a);
   free (b);
   pthread_mutex_destroy(&mutexsum);
   return 0;
   
}  /* end main */
