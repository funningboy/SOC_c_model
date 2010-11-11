#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>


int thread_flag,ret;
pthread_cond_t flag1,flag2;
pthread_mutex_t mut,mutx;
pthread_t print_wait;

void initialize_flag ()
{
  /* Initialize the mutex and condition variable.  */
  pthread_mutex_init (&mut, NULL);
  pthread_cond_init (&flag1, NULL);
  pthread_mutex_init (&mutx, NULL);
  pthread_cond_init (&flag2, NULL);
  /* Initialize the flag value. */
  thread_flag = 0;
}

void* thread_2_fun(void* arg)
{
    while(1)
    {
        printf("\nThe Thread Ended the Command with the return :%d\n",ret);
        pthread_cond_wait(&flag2,&mutx);

    }return NULL;
}

void* print_dot(void* arg)
{
    while(1)printf(".");
    return NULL;
}
void* thread_function(void* arg)
{
    char* value = (char*)arg;
    while(1)
    {
        pthread_mutex_lock(&mut);
        while(!thread_flag)
        {
            pthread_cond_wait(&flag1,&mut);
            pthread_mutex_unlock(&mut);
            ret = system(value);
            //pthread_mutex_lock(&mut);
            pthread_cancel(print_wait);
            printf("i canceled th print_wait thrad now.\n");
            //pthread_join(&print_wait,NULL);
            //pthread_destroy(
            pthread_cond_signal(&flag2);
            //pthread_mutex_unlock(&mut);
        }
        return NULL;
    }
}

int main()
{
    printf("Hello world!\n");

    //int return_value;
    //return_value = system ("ls -l /");

    pthread_t thread1,thread2;

    //printf("Enter your command : ");
    initialize_flag();

    printf("I am going to get the output of command :ls -l \n");

    pthread_create(&print_wait,NULL,&print_dot,NULL);

    pthread_create(&thread2,NULL,&thread_2_fun,NULL);

    pthread_create(&thread1,NULL,&thread_function,(void*)"ls -l");

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    return 0;
}
