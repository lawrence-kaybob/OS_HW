#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "buffer.h"

/* the buffer */

buffer_item buffer[BUFFER_SIZE];

int main(int argc, char const *argv[])
{
	/* 1. Get command line arguments argv[1],argv[2],argv[3] */
	int sleeTime = atoi(argv[1]);
	int producerNum = atoi(argv[2]);
	int consumerNum = atoi(argv[3]);

	/* 2. Initialize buffer */
	/* 3. Create producer thread(s) */
	/* 4. Create consumer thread(s) */
	/* 5. Sleep */
	/* 6. Exit */
	return 0;
}

int insert_item(buffer_item item) {
	/* insert item into buffer
	return 0 if successful, otherwise
	return -1 indicating an error condition */
}
int remove_item(buffer_item *item){
	/* remove an object from buffer
	placing it in item
	return 0 if successful, otherwise
	return -1 indicating an error condition */
}