/* pgflt_test.c
 *  
 *  the attempt of this program is to induce page faults 
 *
 * $ gcc -o pgflt_test pgflt_test.c
 * $ ./pgflt_test 4              # set increment to 4 * 1024 for allocation
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_VALID   4194303  /* Maximum value for x where x * 1024 < 2^32 */

int loop_cnt = 500; 

int main(int argc, char *argv[]) 
{
     pid_t mypid  = getpid();
     char *buffer;

     if(argc < 2) {
         printf("Usage: %s <alloc>\n", argv[0]);
         exit(1);
     }

     printf( "mypid: %d\n", mypid);

     int arg = atoi(argv[1]);
     if(arg < 0 || arg > MAX_VALID) {
         printf("Invalid argument. Must be between 0 and %d.\n", MAX_VALID);
         exit(1);
     }

     unsigned int num_byte = arg * 1024; 

     printf("Looping with size %u\n", num_byte);
     while(loop_cnt--)
     { 
        printf(".");
        buffer = malloc(num_byte);
        buffer[0] =  'a';
        buffer[num_byte-1] =  'z';
        free(buffer); 

     }
     printf("\nEnd\n");
     exit(0);
}  
