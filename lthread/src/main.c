#include <lthread.h>

void* testFunc(void* args){
	printf("*** Thread %d ****\n", args);
	int iteration=0;
	int i;
	while(1){
		i+=3;
		iteration++;
		if(iteration==100000){
			printf("T-%d Iteration-%d\n",args, iteration );
		}
		if(iteration==10000000){
			printf("T-%d Iteration-%d\n",args, iteration );
		}
		if(iteration==100000000){
			printf("T-%d Iteration-%d\n",args, iteration );
			return i;
			printf("T-%d Iteration-%d\n",args, iteration );
		}
		if(iteration==100000001){
			printf("T-%d Iteration-%d\n",args, iteration );
			return i;
		}
	}
}

int main(){
	TCB* threads = malloc(sizeof(TCB)*3);;
	int* res= malloc(sizeof(int)*3);;
	lthread_create(&threads[0],NULL,testFunc,1);
	lthread_create(&threads[1],NULL,testFunc,2);
	lthread_create(&threads[2],NULL,testFunc,3);
	lthread_join(&threads[0],&res[0]);
	lthread_join(&threads[1],&res[1]);
	lthread_join(&threads[2],&res[2]);
	return 0;
}