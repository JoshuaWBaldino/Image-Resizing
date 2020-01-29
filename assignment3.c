#include "iplib2New.c"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int numRows, numCols, type, resize, imageSize, chunksPerRow, totalChunks; 
image_ptr imagePtr, imagePtr2;
struct param{
	int row;
	int col;
};
void* creatingImage(void * parameters){
	int *currentValue = (int *) malloc(sizeof(int));
	struct param *args = (struct param *) parameters;
	*currentValue=0;
	for(int k = 0; k < resize; k++){ 
		for(int l = 0; l < resize; l++){
			*currentValue+=imagePtr[(k+args->col) * numCols + (l+args->row)];
		}
	}
	pthread_exit((void *)currentValue);
}
int main(int argc, char **argv){
	if (argc != 5)
	   {
	     printf("Inputs must be in this form: %s inputImageFile outputImageFile resizeValue parameterse  ters\n", argv[0]);  
	     return 0;
	   } 
	printf("Reading image...\n");
	imagePtr = read_pnm(argv[1], &numRows, &numCols, &type);
	resize = atoi(argv[3]);
	int parametersAmount = atoi(argv[4]);
	struct param parameters[parametersAmount];
	pthread_t threadID[parametersAmount];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	printf("numRows=%d, numCols=%d, type=%d \n", numRows, numCols, type);
	printf("Starting Image Reading...\n");
	imageSize = numCols/resize;
	totalChunks = (numRows/resize) * (numRows/resize);
	chunksPerRow = imageSize/resize;
	unsigned char endingImage[imageSize][imageSize];
	printf("Creating Final Image...\n");
	int *curReturn;
	for(int i=0; i<((numCols*numRows));i+=parametersAmount){
		for(int j=0; j<parametersAmount && (i+j)<totalChunks; j++){
			parameters[j].row = ((i+j)/imageSize)*(resize);
			parameters[j].col = ((i+j)%imageSize)*(resize);
			pthread_create(&threadID[j], &attr, creatingImage,(void *) &parameters[j]);
		}
		for(int j=0; j<parametersAmount && (i+j)<totalChunks; j++){
		pthread_join(threadID[j], (void *) &curReturn);
		endingImage[parameters[j].col/resize][parameters[j].row/resize]=((float)*curReturn)/(resize*resize);
		free(curReturn);
		}
	}
	imagePtr2=(image_ptr) endingImage;
	printf("numRows=%d, numCols=%d, type=%d \n", imageSize, imageSize, type);
	printf("standard deviation= \n");
	printf("average= %d \n",chunksPerRow);
	write_pnm(imagePtr2, argv[2], imageSize, imageSize,type);
	printf("Image Completed\n");
	return 0;
}
 


