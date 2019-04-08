#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
pthread_mutex_t mutex;
pthread_mutex_init(&mutex,NULL);


pid_t pid1 = fork();	
if(pid1 == 0){
	printf("I am the process P1.\n");
	return 0;
}

waitpid(pid1, NULL, 0);	
pid_t pid2 = fork();
if(pid2 == 0){
	pthread_mutex_lock(&mutex);	
	printf("I am the process P2.\n");
	pthread_mutex_unlock(&mutex);
	return 0;
}

waitpid(pid1, NULL, 0);
pid_t pid3 = fork();
if(pid3 == 0){
	pthread_mutex_lock(&mutex);
	printf("I am the process P3. \n");
	pthread_mutex_unlock(&mutex);
	return 0;
}

waitpid(pid2, NULL, 0);
waitpid(pid3, NULL, 0);
pid_t pid4 = fork();
if(pid4 == 0){
	printf("I am the process P4. \n");
	return 0;
}

else if(pid1 != -1){
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
	waitpid(pid3, NULL, 0);
	waitpid(pid4, NULL, 0);	
	printf("done\n");
}
 
pthread_mutex_destroy(&mutex);
return 0;
}

