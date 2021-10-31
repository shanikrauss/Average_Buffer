// IMPLEMENT YOUR FUNCTIONS HER
// todo: remove this
#define _CRT_SECURE_NO_WARNINGS

#include "average_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// here also 
AverageBuffer* allocAverageBuffer(int maxSize, callbackFunc above_threshold_cb, int threshold) // threshhold maybe double
{
	AverageBuffer* ab = (AverageBuffer*)safeMalloc(sizeof(AverageBuffer));

	ab->buffer = (int*)safeMalloc(maxSize * sizeof(int));
	// If you use max size that means that size is correct name instead of "curNumOfSamples
	// If you prefer curNumOfSamples then maxSize should be just "size"
	ab->maxSize = maxSize;
	ab->threshold = threshold;
	// just call it callback or thresholdCallback o thresholdFunc 
	// Also, why is it snake_case and the rest of the code is camelCase?
	ab->above_threshold_cb = above_threshold_cb;

	// This is all the same as clean buffer, why not just call clean buffer?
	ab->curNumOfSamples = 0;
	ab->foreverNumOfSamples = 0;
	ab->upperQuarterSum = 0;
	ab->lowerQuarterSum = 0;
	ab->foreverSum = 0;
	ab->sum = 0;
	ab->upperQuarterSum = 0;
	ab->lowerQuarterSum = 0;
	ab->indexOldestSample = 0;
	ab->indexQuarterLow = 0;
	ab->indexQuarterUp = 0;
	ab->currAverage = 0;

	return ab;
}

// Cleanup function documentation, for multiline function doc use /** ... **/

//- Every time a new sample enters the buffer the oldest sample will be overwritten in the buffer (if the buffer is full).
//- Upon adding a new sample to the buffer, a check will be made to see whether the newly-calculated average is above a certain threshold,
//and if it is, a callback function will be called (in this example the threshold is 40 and the callback function is above_threshold_cb())
void addSample(AverageBuffer* ab, int number)
{
	// saveOldSample not clear what that means
	// why not just oldest?
	int saveOldSample = 0;

	if (isFull(ab))
	{
		saveOldSample = ab->buffer[ab->indexOldestSample];
	}

	// The order in english and in var names is always adjective then noun or subject, so it needs to be oldestSampleIndex instead of indexOldestSample
	// And why not oldesIdx short and clear
	ab->buffer[ab->indexOldestSample] = number;
	ab->indexOldestSample = (ab->indexOldestSample + 1) % ab->maxSize;
	//ab->indexOldestSample++;
	//ab->indexOldestSample = ab->indexOldestSample % ab->maxSize;
	// change number to newNumber
	updateSum(ab, number, saveOldSample);
	updateLowerQuarterSum(ab, saveOldSample);
	updateUpperQuarterSum(ab, number);

	if (!isFull(ab)) // if buffer isnt full yet increase 
	{
		ab->curNumOfSamples++;
	}

	// why is this necessary?
	updateAverage(ab);

	// why is it not happening together with all the others sums that are incremented?
	ab->foreverNumOfSamples++;
	ab->foreverSum += number;

	if (ab->currAverage > ab->threshold) // if avg is above threshold call callback
	{
		// It would be a nice touch if you check that callback != NULL before you call it, this way the buffer can be used  without sending any callback and it would not break.
		ab->above_threshold_cb(ab->currAverage, ab->threshold);
	}
}

// It's weird that you always send a variable called numberToReduce but you don't always reduce it. 
// Either call it oldestNumber or call it toReduce, and always reduce it (sometimes it's zero)
void updateSum(AverageBuffer* ab, int newNumberToAdd, int oldNumberToReduce)
{
	// This if is redundant unless you always pass the real oldestNumber
	if (isFull(ab))
	{
		ab->sum -= oldNumberToReduce;
	}

	ab->sum += newNumberToAdd;
}

void updateUpperQuarterSum(AverageBuffer* ab, int newNumberToAdd)
{
	int nextNumOfSamples = ab->curNumOfSamples + 1;

	if (!isFull(ab) && (double)nextNumOfSamples / 4 == nextNumOfSamples / 4)
	{
		int indexNewSampleAdd = (ab->indexQuarterUp + nextNumOfSamples / 4) - 1;
		ab->upperQuarterSum += ab->buffer[indexNewSampleAdd];
	}
	else
	{
		// Here oldesNumberToReduce is ab->buffer[ab->indexQuarterUp] and it's calculated inside the function, 
		// but for updateSum and updateLowerQuarterSum it's calculated outside. Why the inconsistency?
		ab->upperQuarterSum -= ab->buffer[ab->indexQuarterUp];
		ab->upperQuarterSum += newNumberToAdd;
		ab->indexQuarterUp = (ab->indexQuarterUp + 1) % ab->maxSize;
	}
}

void updateLowerQuarterSum(AverageBuffer* ab, int oldNumberToReduce)
{
	if (!isFull(ab))
	{
		// you can move this outside the if and have if (!isFull(ab) && shouldAddNewNumber) { ... } else { ... }
		int nextNumOfSamples = ab->curNumOfSamples + 1;

		// Don't compare between int and double like this, it's unsafe and unpredictable. You can get 2.00001 == 2 which is false
		// Also, isn't it more elegant to just check if nextNumOfSamples % 4 == 0 ?
		if ((double)nextNumOfSamples / 4 == nextNumOfSamples / 4)
		{
			int indexNewSampleAdd = (ab->indexQuarterLow + nextNumOfSamples / 4) - 1;
			ab->lowerQuarterSum += ab->buffer[indexNewSampleAdd];
		}
	}
	else
	{
		ab->indexQuarterLow = (ab->indexQuarterLow + 1) % ab->maxSize;
		int indexNewSampleAdd = ((ab->indexQuarterLow + ab->curNumOfSamples / 4) - 1) % ab->maxSize;

		ab->lowerQuarterSum -= oldNumberToReduce;		
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
	return ab->currAverage;
}

double getAverageForever(AverageBuffer* ab)
{
	return ab->foreverSum / ab->foreverNumOfSamples;
}

bool isFull(AverageBuffer* ab)
{
	return ab->maxSize == ab->curNumOfSamples;
}

void updateAverage(AverageBuffer* ab)
{
	ab->currAverage = ab->sum / ab->curNumOfSamples;
}

void clearAverageBuffer(AverageBuffer* ab)
{
	ab->indexOldestSample = 0;
	ab->indexQuarterLow = 0;
	ab->indexQuarterUp = 0;
	ab->curNumOfSamples = 0;
	ab->foreverNumOfSamples = 0;
	ab->sum = 0;
	ab->lowerQuarterSum = 0;
	ab->upperQuarterSum = 0;
	ab->foreverSum = 0;
	ab->currAverage = 0;
}

void freeAverageBuffer(AverageBuffer* ab)
{
	free(ab->buffer);
	free(ab);
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