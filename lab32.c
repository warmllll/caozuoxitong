#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>

int ticketcount = 1000;	
int temp = 0;
sem_t s1;	
sem_t s2;	

void *worker1(void *arg){
	while(1){
	sem_wait(&s1);	
	temp = ticketcount;
	pthread_yield();	
	temp = temp - 1;
	pthread_yield();
	ticketcount = temp;
	printf("shou piao %d\n",ticketcount);
	usleep(500);		
	sem_post(&s2);	
	}
	return NULL;
}

void *worker2(void *arg){
	while(1){
	sem_wait(&s2);	
	temp = ticketcount;
	pthread_yield();
	temp = temp + 1;
	pthread_yield();
	ticketcount = temp;
	printf("tui piao %d\n",ticketcount);
	usleep(500);
	sem_post(&s1);	
	}
	return NULL;
}

int main(int argc, char *argv[]){	
	sem_init(&s1, 0, 1000);
	sem_init(&s2, 0, 0);

	pthread_t p1, p2;
	pthread_create(&p1, NULL, worker1, NULL);
	pthread_create(&p2, NULL, worker2, NULL);

	pthread_join(p1, NULL);
	pthread_join(p2, NULL);

	sem_destroy(&s1);
	sem_destroy(&s2);

	return 0;
}

