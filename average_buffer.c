// IMPLEMENT YOUR FUNCTIONS HER
// todo: remove this
#define _CRT_SECURE_NO_WARNINGS

#include "average_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// here also 
AverageBuffer* allocAverageBuffer(int size, callbackFunc aboveThresholdCB, int threshold) // threshhold maybe double
{
	AverageBuffer* ab = (AverageBuffer*)safeMalloc(sizeof(AverageBuffer));

	ab->buffer = (int*)safeMalloc(size * sizeof(int));
	ab->size = size;
	ab->threshold = threshold;
	ab->aboveThresholdCB = aboveThresholdCB;
	clearAverageBuffer(ab);

	return ab;
}

void addSample(AverageBuffer* ab, int newNumber)
{
	int oldestSample = 0;

	if (isFull(ab))
	{
		oldestSample = ab->buffer[ab->oldestSampleIdx];
	}

	ab->buffer[ab->oldestSampleIdx] = newNumber;
	ab->oldestSampleIdx = (ab->oldestSampleIdx + 1) % ab->size;
	updateSum(ab, newNumber, oldestSample);
	updateLowerQuarterSum(ab, oldestSample);
	updateUpperQuarterSum(ab, newNumber);
	ab->foreverSum += newNumber;

	if (!isFull(ab)) // if buffer isnt full yet increase 
	{
		ab->curNumOfSamples++;
	}

	ab->foreverNumOfSamples++;

	// It would be a nice touch if you check that callback != NULL before you call it,
	// this way the buffer can be used  without sending any callback and it would not break.
	// JUST 1 !!!!!!!!!!!!!!!!!!!!111
	double average = getAverage(ab);
	if (average > ab->threshold) // if avg is above threshold call callback
	
	{
		if (ab->aboveThresholdCB != NULL)
		{
			ab->aboveThresholdCB(average, ab->threshold);
		}
	}
	if (ab->aboveThresholdCB != NULL && average > ab->threshold) // if avg is above threshold call callback
	{
		ab->aboveThresholdCB(average, ab->threshold);
	}
	// JUST 1 !!!!!!!!!!!!!!!!!!!!111

}

// It's weird that you always send a variable called numberToReduce but you don't always reduce it. 
// Either call it oldestNumber or call it toReduce, and always reduce it (sometimes it's zero)
void updateSum(AverageBuffer* ab, int newNumberToAdd, int toReduce)
{
	ab->sum -= toReduce; // If no need to reduce because the buffer is not full then toReduce = 0
	ab->sum += newNumberToAdd;
}

void updateUpperQuarterSum(AverageBuffer* ab, int newNumberToAdd)
{
	int nextNumOfSamples = ab->curNumOfSamples + 1;
	int toReduce = ab->buffer[ab->quarterUpIdx];

	if (!isFull(ab) && nextNumOfSamples % 4 == 0)
	{
		int indexNewSampleAdd = (ab->quarterUpIdx + nextNumOfSamples / 4) - 1;
		ab->upperQuarterSum += ab->buffer[indexNewSampleAdd];
	}
	else
	{
		// Here oldesNumberToReduce is ab->buffer[ab->indexQuarterUp] and it's calculated inside the function, 
		// but for updateSum and updateLowerQuarterSum it's calculated outside. Why the inconsistency?
		//ab->upperQuarterSum -= ab->buffer[ab->quarterUpIdx];
		ab->upperQuarterSum -= toReduce;
		ab->upperQuarterSum += newNumberToAdd;
		ab->quarterUpIdx = (ab->quarterUpIdx + 1) % ab->size;
	}
}

void updateLowerQuarterSum(AverageBuffer* ab, int oldestNumber)
{
	if (!isFull(ab))
	{
		// you can move this outside the if and have if (!isFull(ab) && shouldAddNewNumber) { ... } else { ... }
		int nextNumOfSamples = ab->curNumOfSamples + 1;

		// Don't compare between int and double like this, it's unsafe and unpredictable.
		// You can get 2.00001 == 2 which is false
		// Also, isn't it more elegant to just check if nextNumOfSamples % 4 == 0 ?
		if (nextNumOfSamples % 4 == 0)
		{
			int indexNewSampleAdd = (ab->quarterLowIdx + nextNumOfSamples / 4) - 1;
			ab->lowerQuarterSum += ab->buffer[indexNewSampleAdd];
		}
	}
	else
	{
		ab->quarterLowIdx = (ab->quarterLowIdx + 1) % ab->size;
		int indexNewSampleAdd = ((ab->quarterLowIdx + ab->curNumOfSamples / 4) - 1) % ab->size;

		ab->lowerQuarterSum -= oldestNumber;		
		ab->lowerQuarterSum += ab->buffer[indexNewSampleAdd];
	}
}

double getUpperQuarterAverage(AverageBuffer* ab)
{
	if ((ab->curNumOfSamples / 4) != 0)
	{
		return ab->upperQuarterSum / (ab->curNumOfSamples / 4);
	}

	return ab->upperQuarterSum;
}

double getLowerQuarterAverage(AverageBuffer* ab)
{
	if ((ab->curNumOfSamples / 4) != 0)
	{
		return ab->lowerQuarterSum / (ab->curNumOfSamples / 4);
	}

	return ab->lowerQuarterSum;
}

double getAverage(AverageBuffer* ab)
{
	return 	ab->sum / ab->curNumOfSamples;
}

double getAverageForever(AverageBuffer* ab)
{
	return ab->foreverSum / ab->foreverNumOfSamples;
}

bool isFull(AverageBuffer* ab)
{
	return ab->size == ab->curNumOfSamples;
}

void clearAverageBuffer(AverageBuffer* ab)
{
	ab->curNumOfSamples = 0;
	ab->foreverNumOfSamples = 0;
	ab->lowerQuarterSum = 0;
	ab->upperQuarterSum = 0;
	ab->foreverSum = 0;
	ab->sum = 0;
	ab->oldestSampleIdx = 0;
	ab->quarterLowIdx = 0;
	ab->quarterUpIdx = 0;
}

void freeAverageBuffer(AverageBuffer* ab)
{
	free(ab->buffer);
	free(ab);
}

void* safeMalloc(size_t size) 
{
	void* tmp = malloc(size);
	if (tmp == NULL)
	{
		exit(1);
	}
	return tmp;
}