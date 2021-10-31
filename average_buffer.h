// DECLARE YOUR FUNCTIONS HERE
#ifndef AVERAGE_BUFFER_H
#define AVERAGE_BUFFER_H

#include <stdbool.h>
#include <stdlib.h>


typedef void(*callbackFunc)(int, int);

struct AverageBuffer {
	int *buffer;
	int size;
	int curNumOfSamples;
	int foreverNumOfSamples;
	int quarterLowIdx;
	int quarterUpIdx;
	int oldestSampleIdx;
	double sum;
	double foreverSum; 
	double upperQuarterSum;
	double lowerQuarterSum;
	int threshold;
	callbackFunc aboveThresholdCB;
};
typedef struct AverageBuffer AverageBuffer;

AverageBuffer* allocAverageBuffer(int maxSize, callbackFunc above_threshold_cb, int threshold); // threshhold maybe double

void clearAverageBuffer(AverageBuffer* ab);

void freeAverageBuffer(AverageBuffer* ab);

double getAverage(AverageBuffer* ab);

double getAverageForever(AverageBuffer* ab);

void addSample(AverageBuffer* ab, int number);

bool isFull(AverageBuffer* ab);

double getUpperQuarterAverage(AverageBuffer* ab);
double getLowerQuarterAverage(AverageBuffer* ab);
void updateLowerQuarterSum(AverageBuffer* ab, int oldNumberToReduce);
void updateUpperQuarterSum(AverageBuffer* ab, int newNumberToAdd);
void updateSum(AverageBuffer* ab, int newNumberToAdd, int oldNumberToReduce);
void* safeMalloc(size_t size);

#endif
