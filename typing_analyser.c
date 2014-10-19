#define _USE_32BIT_TIME_T 1 //to fix minGW bug http://sourceforge.net/p/mingw/bugs/2152/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*Structure which functions as a linked list.
the current time of each word entered into the system
is recorded into a new instance of this structure*/
struct input {
	int time;
	struct input* next;
};

struct input* inputLL; //global head of linked list

/*
Takes a linked list and a data value.
Creates a new link and pushes the data onto this link
Links to the current head of the linked list
and then makes itself the head of the linked list.
*/
void Push(struct input** headRef, double elapsedTime) {
	struct input* newNode = malloc(sizeof(struct input));
	newNode->time = elapsedTime;
	newNode->next = *headRef; // The '*' to dereferences back to the real head
	*headRef = newNode; // ditto
}

/*
Build the linked list in the heap and store
its head pointer in a local stack variable.
Returns the head pointer to the caller.
*/
struct input* BuildLinkedList() {
	struct input* head = NULL;
	head = malloc(sizeof(struct input));
	head->time = 0;
	head->next = NULL;
	return head;
}

/*
Given a linked list head pointer, compute
and return the number of nodes in the list.
*/

void createLinkedList() {
	inputLL = BuildLinkedList();
}


/*takes the number of words the user has input, and the time period for the average to be taken in seconds
* if you wanted the average words per minute, you would pass the value 60 as the period variable.
*/
float avgPerPeriod(int wordCount, float time, float period)
{
	float multiplier = (float)time / (float)period;
	float value = (float)wordCount / multiplier;
	float rounded = floorf(value * 100 + 0.5) / 100;
	return (rounded);
}

/*takes the number of words the user has input, the interval for the peak to be calculated for, and the total running time
* if you wanted the peak words per minute, you would pass the value 60 as the interval variable.
*/
int peakWords(int wordCount, int interval, int totalTime)
{
	if (totalTime < interval) //if the program hasn't been running for long enough, we can't get a proper peak WPM
	{						  //so we simply return the average WPM per minute.
		return avgPerPeriod(wordCount, totalTime, 60);
	}
	int period;
	if (interval < 11)
	{
		period = 1;/*interval must be bigger than 10, or else we don't have enough significant figures
				   *to calculate the peak words if we used a period of less than 1*/
	}
	else
	{
		period = interval / 10; /*splits interval, i.e. 1 minute split down into
								6 second chunks so we can get a more accurate peak
								reading by measuring a 1 minute chunk at 6 second intervals.
								*/
	}

	int sizeOfArray = (totalTime / period)+1;

	int *arr;
	arr = (int*)calloc(sizeOfArray, sizeof(int)); //allocate memory
	int i;
	for (i = 0; i < sizeOfArray; i++) //instantiate array
		*(arr + i) = 0;

	struct input* current = inputLL;
	int value = 0;
	int count = 0;

	//increments index of timeArray for every input that occured in this time period
	//while (current != NULL) {
	while (count < wordCount) {
		value = ((current->time) / period);
		++*(arr + value);
		current = current->next;
		++count;
	}

	count = 0;
	int innerCount = 0;
	int best = 0;
	int potentialBest = 0;
	while (count < sizeOfArray - 11)
	{
		while (innerCount < 10)
		{
			//adds 10 intervals together to make up the one minute period.
			potentialBest = potentialBest + *(arr + (count + innerCount));
			++innerCount;
		}
		if (potentialBest > best)
		{
			//if the 10 intervals make the best peak WPM so far, then update the best value to this peak.
			best = potentialBest;
		}
		potentialBest = 0; //reset sum so we can add the next 10 intervals
		innerCount = 0;
		count++;
	}

	free(arr); //free dynamically allocated memory

	if (best == 0) //if there wasn't enough data to get a proper peak time then just get average WPM
		//the program must have ran for at least as long as a minute for peak WPM to be returned.
	{
		return avgPerPeriod(wordCount, totalTime, 60);
	}
	else
	{
		return best;
	}
}

/*Displays the statistics to the user apon completion of the program
*/
int displayStats(int wordCount, float time)
{
	printf("\n");
	printf("--------------------STATISTICS------------------\n");
	printf("	Overall time:                    %.f\n", time);
	printf("	Total word count:                %d\n", wordCount);
	printf("	Average words per minute:        %.f\n", avgPerPeriod(wordCount, time, 60));
	printf("	Average words per ten seconds:   %.f\n", avgPerPeriod(wordCount, time, 10));
	printf("	Peak words per minute:           %d\n", peakWords(wordCount, 60, time));
	printf("\n");
	printf("	   Press ENTER to confirm exit		\n");
	getchar();
	return 0; //return focus to end program
}

void welcomeScreen()
{
	printf("------------------------------------------------\n");
	printf("	Welcome to the Typing Analyser		\n");
	printf("------------------------------------------------\n");
	printf("	Type QUIT to exit the program		\n");
	printf("------------------------------------------------\n");
	printf("\n");
}

int isQuit(char input[])
{
	if (input[0] == 'Q' && input[1] == 'U' && input[2] == 'I' && input[3] == 'T')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int getInput()
{
	createLinkedList();
	time_t start, end;
	double elapTime;
	int i = 0;
	int numOfWords = 0;
	char input[1000];

	time(&start);//start timer for analysis purposes
	while (1)
	{
		fgets(input, 1000, stdin);
		if ((isQuit(input)) == 1){
			time(&end); //stop timer to return time the program ran for
			elapTime = difftime(end, start);
			displayStats(numOfWords, elapTime);
			return 0;
		}
		time(&end);
		elapTime = difftime(end, start); //calculates time elapsed

		//split up input into individual words, and then add to the linked list the time at which the words were added
		char * pch;
		pch = strtok(input, " ,.-");
		while (pch != NULL)
		{
			Push(&inputLL, elapTime);
			++numOfWords; //count how many words have been entered
			pch = strtok(NULL, " ,.-:;");
		}
		++i;
	}
}

int main()
{
	welcomeScreen();
	getInput();
	return 0;
}
