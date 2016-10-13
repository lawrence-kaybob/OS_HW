/*

Operating System First Assignment @ Fall 2016
by Sung Hyuk Kay (201320910)

Development Environment : VI Improved 7.4.52 & gcc 4.84
						  on Windows Subsystem for Linux on Windows 10 Build 1607
Testing Environment : Windows Subsystem for Linux (Ubuntu 14.0.5 Trusty) on Windows 10 Build 1607

*/

#include <stdio.h>			/* 기본 입출력에 대한 header 파일*/
#include <stdlib.h>			/* exit 함수를 사용하기 위한 header 파일 */
#include <pthread.h>		/* pthread를 활용하기 위한 header 파일*/

#define NUM_THREAD 3		/* 만들 Thread의 갯수 (Sorting Thread 2개 & Merging Thread 1개) */
#define MAX_SIZE 100		/* 최대 입력 갯수는 100개이다. */

typedef struct {			/* Sorting Thread에서 필요한 인자를 구조체로 구성했다. */
	int fullSize;			/* 실제 데이터의 길이 */
	int half;				/* 앞부분인가? 뒷부분인가? */
} info;

int original[MAX_SIZE] = {0};	/* input.txt에서 받아온 원본 */
int sorted[MAX_SIZE] = {0};		/* Merge Thread를 통해 정렬한 최종 결과 */

/*

< sortingThreadFunc >
original 배열의 절반부분을 정렬하는 Thread에 대한 Function.
Insertion Sorting(삽입정렬)을 사용했으며,
마지막엔 output.txt에 각 Sorting Thread에 대한 결과를 출력한다.

*/

void *sortThreadFunc(void *arg) {
	info* param = (info*) arg;									/* 인자로 받은 내용을 pointer type에 맞게 바꿔준다. */
	int half = param -> half;									/* 앞쪽(0)인가? 뒤쪽(1)인가?*/
	int size = param -> fullSize / 2 + param -> fullSize % 2;	/* 전체 크기가 홀수인 경우, 뒤쪽이 */
	int start = (size - param -> fullSize % 2) * half;			/* 어느 지점부터 시작하는지 계산해준다. */
	int i, j;													/* 반복문을 사용하기 위한 변수 */

	/* < Insertion Sorting(삽입정렬) 시작 > */

	for(i = 0; i < size; i++){
		int minIndex = start + i;							/* Insertion Sorting의 특성으로 인해 각 Interation에서 최솟값에 대한 index를 */
															/* 저장할 변수가 필요하다 */
		for(j = i; j < size; j++){						
			if(original[minIndex] > original[start + j])	/* 만약 현재 최솟값에서 더 작은 값이 나오면*/
				minIndex = start + j;						/* 새로운 index로 업데이트 해준다. */
		}

		{
			int temp = original[start + i];					/* 그리고 그 최솟값의 위치와*/
			original[start + i] = original[minIndex];		/* 현재 iteration의 맨 앞의 값과*/
			original[minIndex] = temp;						/* 교체 시켜준다.*/
		}
	}

	pthread_exit(NULL);	/* Thread를 정상 종료시킨다. */

}

/*
< mergeThreadFunc >
두 번의 Sorting Thread를 수행한 후 각 결과를
하나의 통일된 결과로 만들기 위해 Merge Thread를 수행한다.
*/
void *mergeThreadFunc(void *arg) {
	info* param = (info*) arg;			/* 받은 인자를 알맞은 pointer type으로 casting한다. */
	int fullSize = param -> fullSize;	/* 정렬하려고 하는 배열의 전체 크기*/
	int leftSize = fullSize / 2;		/* 앞쪽부분의 크기, 뒤쪽부분과 같거나 1개 적다. */
	int i, j, k;						/* Merge를 하기 위해 배열 추적동안 필요한 index */
	FILE* output;						/* output을 위한 File Stream*/

	i = 0;	j = leftSize;	k = 0;	/* i는 앞쪽 절반, j는 뒤쪽 절반, k는 sorted의 index를 추적*/

	/* Merge Sort 방법으로 병합 시작 */

	while(i < leftSize && j < fullSize) {	/* 둘 다 아직 남아있는 경우엔 */
		if(original[i] > original[j])		/* 둘 중 작은 것을 */
			sorted[k++] = original[j++];	/* 먼저 넣어준다. */
		else
			sorted[k++] = original[i++];
	}

	while(i < leftSize)						/* 그리고 어느 한 쪽을 다 쓰게되면*/
		sorted[k++] = original[i++];		/* 나머지를 몽땅 넣어준다. */

	while(j < fullSize)						/* 여기도 마찬가지*/
		sorted[k++] = original[j++];

	if((output = fopen("/OS/output.txt", "w")) == NULL) {					/* output.txt를 수정모드로 열고 */
		printf("\aProblem with Output File Stream. Abort Program.\n\n");	/* 만약 열기에 문제가 있으면 */
		exit(EXIT_FAILURE);													/* 프로그램을 종료한다. */
	}


	for(i = 0; i < 2; i++){
		fprintf(output, "[Sorting Thread %d]\n", i + 1);		/* 먼저 Sorting Thread가 한 결과값을 */

		for(j = 0; j < leftSize + (fullSize % 2) * i; j++)
			fprintf(output, "%d ", original[leftSize * i + j]);

		fprintf(output, "\n\n");
	}

	fprintf(output, "[Merging Thread]\n");	/* 정상적으로 열렸으면*/

	for(i = 0; i < fullSize; i++)			/* output에 결과값을 출력한다.*/
		fprintf(output, "%d ", sorted[i]);

	fprintf(output, "\n\n");

	fclose(output);							/* 그리고 File Stream을 닫고*/

	pthread_exit(NULL);						/* Thread도 종료한다. */
}

/*

< main >
실제 프로그램의 실행을 책임지고
Sorting Thread와 Merge Thread를
만들어 실행한다.

*/

int main(int argc, char const *argv[])
{
	FILE* input;							/* 처음 파일을 불러오기 위한 File Stream */

	int i, j;								/* 반복문 돌리기 위한 변수 */

	pthread_t tid[NUM_THREAD];				/* 만들 Thread들에 대한 배열 */
	info arg[2];

	input = fopen("/OS/input.txt", "r");	/* input.txt 파일을 읽기 모드로 연다.*/

	if(input == NULL){						/* 만약 파일 열기에 문제가 있으면*/
		printf("Something wrong with input file. Abort Program");
		exit(EXIT_FAILURE);					/* 프로그램 종료*/
	}

	i = 0;

	do {										/* 100개의 정수 혹은 EOF(-1)를 받기까지*/
		fscanf(input, "%d", &original[i++]);	/* 정수를 받아서 original에 넣어준다. */
	}while(i < MAX_SIZE && original[i-1] != -1);

	fclose(input);								/* 그리고 input에 대한 File Stream은 종료한다. */

	for(j = 0 ; j < 2; j++) {					/* 방금 받은 입력에 대한 정보를 구조체에 넣는다. */
		arg[j].fullSize = i - 1;				/* fullSize는 입력받은 숫자들의 갯수를 저장하며*/
		arg[j].half = j;						/* half는 앞쪽이면 0, 뒤쪽이면 1을 저장한다. */
	}

	pthread_create(&tid[0], NULL, sortThreadFunc, &arg[0]);	/* Sorting Thread들을 배열 반씩 나눠서*/
	pthread_create(&tid[1], NULL, sortThreadFunc, &arg[1]);	/* 수행한다 */
	
	for(i = 0; i < 2; i++)				/* 그리고 실행한 Sorting Thread들이*/
		pthread_join(tid[i], NULL);		/* 확실히 끝날때까지 기다린다.*/

	pthread_create(&tid[2], NULL, mergeThreadFunc, &arg[0]);	/* 마지막으로 Merge Thread를 실행하고 */
	pthread_join(tid[2], NULL);								/* 끝나기까지 기다린다. */
	
	return 0;		/* 프로그램 종료 */
}