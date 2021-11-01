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

AverageBuffer* allocAverageBuffer(int size, callbackFunc aboveThresholdCB, int threshold); // threshhold maybe double

void addSample(AverageBuffer* ab, int newNumber);

void updateSum(AverageBuffer* ab, int newNumberToAdd, int toReduce);

void updateUpperQuarterSum(AverageBuffer* ab, int newNumberToAdd);

void updateLowerQuarterSum(AverageBuffer* ab, int oldestNumber);

void updateForeverSum(AverageBuffer* ab, int newNumberToAdd);

double getUpperQuarterAverage(AverageBuffer* ab);

double getLowerQuarterAverage(AverageBuffer* ab);

double getAverage(AverageBuffer* ab);

double getAverageForever(AverageBuffer* ab);

bool isFull(AverageBuffer* ab);

void clearAverageBuffer(AverageBuffer* ab);

void freeAverageBuffer(AverageBuffer* ab);

void* safeMalloc(size_t size);

#endif
