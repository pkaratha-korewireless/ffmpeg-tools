#include "rtsp_relay.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void *myThreadFun(void* arg){
	int ret;
	ret=start_stream(arg);
	printf("Stream ends with ret :%d\n",ret);
	return 0;
	}
int call_fun(){
	printf("connection timedout\n");
	stop_stream();
	return 0;
	}
int main(int argc,char* argv[]){
	int ret;
	int i=0;
	int thred_ret;
	
	while (i<10){
		i++;
		ret=init_streams(argv[1],argv[2],&call_fun);
		pthread_t tid;
		if(ret==0){
    		pthread_create(&tid, NULL, myThreadFun, argv[2]);
			getchar();
			stop_stream();
			printf("Waiting for Start finish\n");
			pthread_join(tid,NULL);
			sleep(20);
			printf("end\n");
		}
	}
}


