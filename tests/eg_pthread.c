/* **************************************************************** *
 * Example pthread code                                             *
 * Each thread increments a shared counter                          *
 * **************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/errno.h>
#include <sys/types.h>
#include "m5op.h"


#define MAX_NUMP 16

int NumProcs;

pthread_mutex_t   SyncLock; /* mutex */
pthread_cond_t    SyncCV; /* condition variable */
int               SyncCount; /* number of processors at the barrier so far */

pthread_mutex_t   ThreadLock; /* mutex */
int               Count; /* the shared counter */

/* Ad-hoc Barrier Code. This function will cause all the threads to synchronize
 * with each other.  What happens is that the mutex lock is used to control
 * access to the condition variable & SyncCount variable.  SyncCount is
 * initialized to 0 in the beginning, and when barrier is called by each
 * thread, SyncCount is incremented.  When it reaches NumProcs, the
 * number of threads/processors, a conditional broadcast is sent out which
 * wakes up all the threads.  The bad part is that each thread will then
 * contend over the mutex lock, SyncLock, and will be released sequentially.
 *
 * see man for further descriptions about cond_broadcast, cond_wait, etc. 
 *
 * Barrier locks could also be implemented in many other ways, using
 * semaphores, and other sync. functions
 */
void Barrier()
{
  int ret;

  pthread_mutex_lock(&SyncLock); /* Get the thread lock */
  SyncCount++;
  if(SyncCount == NumProcs) {
    ret = pthread_cond_broadcast(&SyncCV);
SyncCount = 0;	
    assert(ret == 0);
  } else {
    ret = pthread_cond_wait(&SyncCV, &SyncLock); 
    assert(ret == 0);
  }
  pthread_mutex_unlock(&SyncLock);
}


/* The function which is called once the thread is allocated */
void* ThreadLoop(void* tmp)
{
  /* each thread has a private version of local variables */
  int threadId = (int) tmp; 
  int ret;
  int i;

  /* ********************** Thread Synchronization*********************** */
  if(threadId==0)
    m5_reset_stats(0,0);
  Barrier();
  

  /* ********************** Execute Job ********************************* */
  for (i = 0; i < (10000 / NumProcs); i++) {
    m5_work_begin(0,threadId);
    pthread_mutex_lock(&ThreadLock); /* Get the thread lock */
    Count++;
    pthread_mutex_unlock(&ThreadLock); /* Release the lock */
    m5_work_end(0,threadId);
  }  
}


int main(int argc, char** argv)
{
  pthread_t*     threads;
  pthread_attr_t attr;
  int            ret;
  int            dx;

  if(argc != 1) {
    fprintf(stderr, "USAGE: %s <numProcesors>\n", argv[0]);
    exit(-1);
  }
  assert(argc == 1);
  NumProcs = 1;
  assert(NumProcs > 0 && NumProcs <= MAX_NUMP);

  /* Initialize array of thread structures */
  threads = (pthread_t *) malloc(sizeof(pthread_t) * NumProcs);
  assert(threads != NULL);

  /* Initialize thread attribute */
  pthread_attr_init(&attr);
//  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); // sys manages contention

  /* Initialize mutexs */
  ret = pthread_mutex_init(&SyncLock, NULL);
  assert(ret == 0);
  ret = pthread_mutex_init(&ThreadLock, NULL);
  assert(ret == 0);
  
  /* Init condition variable */
  ret = pthread_cond_init(&SyncCV, NULL);
  assert(ret == 0);
  SyncCount = 0;

  Count = 0;

  /* get high resolution timer, timer is expressed in nanoseconds, relative
   * to some arbitrary time.. so to get delta time must call gethrtime at
   * the end of operation and subtract the two times.
   */
  for(dx=0; dx < NumProcs; dx++) {
    /* ************************************************************
     * pthread_create takes 4 parameters
     *  p1: threads(output)
     *  p2: thread attribute
     *  p3: start routine, where new thread begins
     *  p4: arguments to the thread
     * ************************************************************ */
    ret = pthread_create(&threads[dx], &attr, ThreadLoop, (void*) dx);
    assert(ret == 0);

  }

  /* Wait for each of the threads to terminate */
  for(dx=0; dx < NumProcs; dx++) {
    ret = pthread_join(threads[dx], NULL);
    assert(ret == 0);
  }
  printf("Count: %d\n",Count);

  pthread_mutex_destroy(&ThreadLock);

  pthread_mutex_destroy(&SyncLock);
  pthread_cond_destroy(&SyncCV);
//  pthread_attr_destroy(&attr);

  return 0;
}
