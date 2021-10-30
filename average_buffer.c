// IMPLEMENT YOUR FUNCTIONS HER
#define _CRT_SECURE_NO_WARNINGS

#include "average_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

AverageBuffer* allocAverageBuffer(int maxSize, callbackFunc above_threshold_cb, int threshold) // threshhold maybe double
{
	AverageBuffer* ab = (AverageBuffer*)safeMalloc(sizeof(AverageBuffer));

	ab->buffer = (int*)safeMalloc(maxSize * sizeof(int));
	ab->maxSize = maxSize;
	ab->threshold = threshold;
	ab->curNumOfSamples = 0;
	ab->foreverNumOfSamples = 0;
	//ab->quarterNumOfSamples = 0;
	ab->upperQuarterSum = 0;
	ab->lowerQuarterSum = 0;

	ab->foreverSum = 0;
	ab->sum = 0;
	ab->upperQuarterSum = 0;
	ab->lowerQuarterSum = 0;
	ab->indexOldestSample = 0;
	ab->indexQuarterLow = 0;
	ab->indexQuarterUp = 0;
	//ab->indexNewSample = 0;

	ab->currAverage = 0;
	ab->above_threshold_cb = above_threshold_cb;

	return ab;
}

//- Every time a new sample enters the buffer the oldest sample will be overwritten in the buffer (if the buffer is full).
//- Upon adding a new sample to the buffer, a check will be made to see whether the newly-calculated average is above a certain threshold,
//and if it is, a callback function will be called (in this example the threshold is 40 and the callback function is above_threshold_cb())
void addSample(AverageBuffer* ab, int number)
{
	int saveOldSample = 0;

	if (isFull(ab))
	{
		saveOldSample = ab->buffer[ab->indexOldestSample];
	}

	ab->buffer[ab->indexOldestSample] = number;

	ab->indexOldestSample++;
	ab->indexOldestSample = ab->indexOldestSample % ab->maxSize;

	updateSum(ab, number, saveOldSample);
	updateLowerQuarterSum(ab, saveOldSample);

	if (!isFull(ab)) // if buffer isnt full yet increase 
	{
		ab->curNumOfSamples++;
	}

	updateAverage(ab);
	//updateUpperQuarterSum(ab, number);

	ab->foreverNumOfSamples++;
	ab->foreverSum += number;

	if (ab->currAverage > ab->threshold) // if avg is above threshold call callback
	{
		ab->above_threshold_cb(ab->currAverage, ab->threshold);
	}
}

void updateUpperQuarterSum(AverageBuffer* ab, int newNumberToAdd)
{
	if (!isFull(ab))
	{
		if (round(ab->curNumOfSamples / 4) == ab->curNumOfSamples / 4)
		{
			//ab->upperQuarterSum += ab->buffer[ab->indexQuarterUp];
		}

	}
	else
	{
		ab->upperQuarterSum -= ab->buffer[ab->indexQuarterUp];
		ab->upperQuarterSum += newNumberToAdd;
		ab->indexQuarterUp = (ab->indexQuarterUp + 1) % ab->maxSize;
	}
}

void updateSum(AverageBuffer* ab, int newNumberToAdd, int oldNumberToReduce)
{
	if (isFull(ab))
	{
		ab->sum -= oldNumberToReduce;
	}

	ab->sum += newNumberToAdd;
}

void updateLowerQuarterSum(AverageBuffer* ab, int oldNumberToReduce)
{
	if (!isFull(ab))
	{
		int nextNumOfSamples = ab->curNumOfSamples + 1;

		if ((double)nextNumOfSamples / 4 == nextNumOfSamples / 4)
		{
			int indexNewSampleAdd = (ab->indexQuarterLow + nextNumOfSamples / 4) - 1;
			ab->lowerQuarterSum += ab->buffer[indexNewSampleAdd];
		}
	}
	else
	{
		ab->indexQuarterLow = (ab->indexQuarterLow + 1) % ab->maxSize;
		int indexNewSampleAdd = ((ab->indexQuarterLow + ab->curNumOfSamples / 4) % ab->maxSize) - 1;

		ab->lowerQuarterSum -= oldNumberToReduce;		
		ab->lowerQuarterSum += ab->buffer[indexNewSampleAdd];
	}
}

double getLowerQuarterAverage(AverageBuffer* ab)
{
	if ((ab->curNumOfSamples / 4) != 0)
	{
		return ab->lowerQuarterSum / (ab->curNumOfSamples / 4);
	}

	return ab->lowerQuarterSum;
}

void clearAverageBuffer(AverageBuffer* ab)
{
	ab->indexOldestSample = 0;
	ab->curNumOfSamples = 0;
	ab->foreverNumOfSamples = 0;
	ab->sum = 0;
	ab->foreverSum = 0;
	ab->currAverage = 0;
}

void freeAverageBuffer(AverageBuffer* ab)
{
	free(ab->buffer);
	free(ab);
}

double getAverage(AverageBuffer* ab)
{
	return ab->currAverage;
}

double getAverageForever(AverageBuffer* ab)
{
	return ab->foreverSum / ab->foreverNumOfSamples;
}

bool isFull(AverageBuffer* ab)
{
	//return ab->indexOldestSample < ab->curNumOfSamples;
	return ab->maxSize == ab->curNumOfSamples;
}

void updateAverage(AverageBuffer* ab)
{
	ab->currAverage = ab->sum / ab->curNumOfSamples;
}

void* safeMalloc(size_t size) // ask about
{
	void* tmp = malloc(size);
	if (tmp == NULL)
	{
		exit(1);
	}
	return tmp;
}