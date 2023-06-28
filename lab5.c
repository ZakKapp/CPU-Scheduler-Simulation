//"I pledge that I have neither given nor received help from anyone other
//than the instructor/TA for all program components included here."
//Zak Kappenman
//Student ID: 43851109


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>


typedef struct threadInfo_ {
	int processorID;
} threadInfo;

#pragma pack(1)
	typedef struct PCB_ {
		char priority;
		char name[32];
		int processID;
		char activityStatus;
		int burstTime;
		int baseRegister;
		long int limitRegister;
		char processType;
		int numberOfFiles;
	} PCB;

//global variables
int endFlag = 0;
int totalProcessors;
int totalProcesses;
int* queueCounts;
int* pProtocols;
PCB** queueList;
pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t supervisor;

//comparator for sorting by priority
int priorityComparator(const void *v1, const void *v2) {
	const PCB *p1 = (PCB *)v1;
	const PCB *p2 = (PCB *)v2;
	if(p1->priority == 0 && p2->priority == 0)
		return 0;
	else if(p1->priority == 0)
		return 1;
	else if(p2->priority == 0)
		return -1;
	else if(p1->priority < p2->priority)
		return -1;
	else if(p1->priority > p2->priority)
		return 1;
	else
		return 0;
} // end priorityComparator

//comparator for sorting by burstTime
int burstTimeComparator(const void *v1, const void *v2) {
	const PCB *p1 = (PCB *)v1;
	const PCB *p2 = (PCB *)v2;
	if(p1->burstTime == 0 && p2->burstTime == 0)
		return 0;
	else if(p1->burstTime == 0)
		return 1;
	else if(p2->burstTime == 0)
		return -1;
	else if(p1->burstTime < p2->burstTime)
		return -1;
	else if(p1->burstTime > p2->burstTime)
		return 1;
	else
		return 0;
} // end burstTimeComparator

//Helper function to push processes to the back of the queue
void push(PCB process, int pNum) {
	pthread_mutex_lock(&myMutex);
	queueList[pNum][queueCounts[pNum]] = process;
	queueCounts[pNum]++;
	pthread_mutex_unlock(&myMutex);
} // end push

//Priority Scheduling thread {4}
void * priorityScheduling(void* param) {
	threadInfo * myParam = (threadInfo *)param;
	int pNum = myParam->processorID;
	int ageCounter = 0;

	//initial sorting of queue
	qsort(queueList[pNum], totalProcesses, sizeof(PCB), priorityComparator);

	while(endFlag == 0) {
		if(queueCounts[pNum] == 0)
			continue;
		PCB currentPCB;
		pthread_mutex_lock(&myMutex);
		currentPCB = queueList[pNum][0];

		for(int i = 0; i < queueCounts[pNum]; i++) {
			queueList[pNum][i] = queueList[pNum][i+1];
		}
		queueCounts[pNum]--;
		memset(&queueList[pNum][queueCounts[pNum]], 0, sizeof(PCB));
		pthread_mutex_unlock(&myMutex);

		//process execution loop
		printf("Executing Process#%d on Processor #%d\n", currentPCB.processID, pNum);
		while(currentPCB.burstTime > 0) {
			if(ageCounter % 10 == 0) {
				printf("Aging Processes in Processor #%d\n", pNum);
				pthread_mutex_lock(&myMutex);
				for(int i = 0; i < queueCounts[pNum]; i++) {
					queueList[pNum][i].priority--;
					if(queueList[pNum][i].priority < 0)
						queueList[pNum][i].priority = 0;
				}
				pthread_mutex_unlock(&myMutex);
			}

			currentPCB.burstTime -= 2;
			sleep(0.5);
			ageCounter++;
		} // end while
		printf("Finished Executing Process#%d on Processor #%d\n", currentPCB.processID, pNum);
	} // end PS while
	return (void*)0;
} // end priorityScheduling

//Shortest Job First thread {3}
void * shortestJobFirst(void* param) {
	threadInfo * myParam = (threadInfo *)param;
	int pNum = myParam->processorID;

	//initial sorting of queue
	qsort(queueList[pNum], totalProcesses, sizeof(PCB), burstTimeComparator);

	while(endFlag == 0) {
		if(queueCounts[pNum] == 0)
			continue;
		PCB currentPCB;
		pthread_mutex_lock(&myMutex);
		currentPCB = queueList[pNum][0];

		for(int i = 0; i < queueCounts[pNum]; i++) {
			queueList[pNum][i] = queueList[pNum][i+1];
		}
		queueCounts[pNum]--;
		memset(&queueList[pNum][queueCounts[pNum]], 0, sizeof(PCB));
		pthread_mutex_unlock(&myMutex);

		//process execution loop
		printf("Executing Process#%d on Processor #%d\n", currentPCB.processID, pNum);
		while(currentPCB.burstTime > 0) {
			currentPCB.burstTime -= 2;
			sleep(0.4);
		}
		printf("Finished Executing Process#%d on Processor #%d\n", currentPCB.processID, pNum);
	} // end SJF while
	return (void*)0;
} // end shortestJobFirst

//Round Robin thread {2}
void * roundRobin(void* param) {
	threadInfo * myParam = (threadInfo *)param;
	int pNum = myParam->processorID;

	while(endFlag == 0) {
		if(queueCounts[pNum] == 0)
			continue;
		PCB currentPCB;
		pthread_mutex_lock(&myMutex);
		currentPCB = queueList[pNum][0];

		for(int i = 0; i < queueCounts[pNum]; i++) {
			queueList[pNum][i] = queueList[pNum][i+1];
		}
		queueCounts[pNum]--;
		memset(&queueList[pNum][queueCounts[pNum]], 0, sizeof(PCB));
		pthread_mutex_unlock(&myMutex);

		printf("Executing Process#%d on Processor #%d\n", currentPCB.processID, pNum);
		currentPCB.burstTime -= 4;
		sleep(0.6);
		if(currentPCB.burstTime > 0) {
			push(currentPCB, pNum);
		}
		else {
			printf("Finished Executing Process#%d on Processor #%d\n", currentPCB.processID, pNum);
		}
	} // end RR while
	return (void*)0;
} // end roundRobin

//First Come First Served thread {1}
void * firstComeFirstServed(void* param) {
	threadInfo * myParam = (threadInfo *)param;
	int pNum = myParam->processorID;

	while(endFlag == 0) {
		if(queueCounts[pNum] == 0)
			continue;
		PCB currentPCB;
		pthread_mutex_lock(&myMutex);
		currentPCB = queueList[pNum][0];

		//moving processes forward in queue
		for(int i = 0; i < queueCounts[pNum]; i++) {
			queueList[pNum][i] = queueList[pNum][i+1];
		}
		queueCounts[pNum]--;
		memset(&queueList[pNum][queueCounts[pNum]], 0, sizeof(PCB));
		pthread_mutex_unlock(&myMutex);

		//process execution loop
		printf("Executing Process#%d on Processor #%d\n", currentPCB.processID, pNum);
		while(currentPCB.burstTime > 0) {
			currentPCB.burstTime -= 2;
			sleep(0.4);
		}
		printf("Finished Executing Process#%d on Processor #%d\n", currentPCB.processID, pNum);
	} // end FCFS while
	return (void*)0;
} // end firstComeFirstServed

//Queue Supervisor thread
void * supervisorThread(void* param) {

	while(endFlag == 0) {
		int emptyQ = -1;
		int emptyCount = 0;
		while(1) {
			//checking if any of the queues are empty
			for(int i = 0; i < totalProcessors; i++) {
				if(queueCounts[i] == 0) {
					if(emptyCount == 0)
						emptyQ = i;
					emptyCount++;
				}
			}
			if(emptyQ > -1)
				break;
		} // end queue check while

		//signaling  all threads to return when all queues are empty
		if(emptyCount == totalProcessors) {
			endFlag = 1;
			continue;
		}


		//checking if redistribution is worthwhile
		pthread_mutex_lock(&myMutex);
		int distCheck = 0;
		for(int i = 0; i < totalProcessors; i++) {
			if(queueCounts[i] < 2)
				distCheck++;
		}
		pthread_mutex_unlock(&myMutex);

		if(distCheck == totalProcessors)
			continue;

		printf("\nProcessors Require Workload Redistribution\n\n");

		pthread_mutex_lock(&myMutex);

		//printing queues before distribution
		printf("Queues Before Redistribution:\n\n");
		for(int i = 0; i < totalProcessors; i++) {
			printf("Processes in Queue %d\n", i);
			for(int j = 0; j < totalProcesses; j++) {
				if(queueList[i][j].burstTime == 0)
					break;
				printf("\nProcess #%d", queueList[i][j].processID);
			}
			printf("\n\n");
		}

		printf("Balancing Workload...\n\n");

		//calculating how big each queue should be so that all the processors are balanced
		int sum = 0;
		for(int i = 0; i < totalProcessors; i++) {
			sum += queueCounts[i];
		}
		int balancedQNum = sum/totalProcessors;
		int leftOver = sum % totalProcessors;

		//calculating what queues need to be added to/taken from and the number of processes
		int qNeeds[totalProcessors];
		int posSum = 0;
		for(int i = 0; i < totalProcessors; i++) {
			int x = queueCounts[i] - balancedQNum;
			qNeeds[i] = x;
			if(x > 0)
				posSum += x;
		}

		//gathering processes to redistribute
		PCB redist[posSum];
		int c = 0;
		for(int i = 0; i < totalProcessors; i++) {
			int end = queueCounts[i]-1;
			if(qNeeds[i] > 0) {
				while(qNeeds[i] != 0) {
					redist[c] = queueList[i][end];
					memset(&queueList[i][end], 0, sizeof(PCB));
					queueCounts[i]--;
					qNeeds[i]--;
					end--;
					c++;
				}
			}
		}

		//distributing processes to the queues that need them
		c = 0;
		for(int i = 0; i < totalProcessors; i++) {
			if(qNeeds[i] < 0) {
				while(qNeeds[i] != 0) {
					queueList[i][queueCounts[i]] = redist[c];
					memset(&redist[c], 0, sizeof(PCB));
					queueCounts[i]++;
					qNeeds[i]++;
					c++;
				} // end while loop
			} // end if
		} // end for loop

		//distributing leftovers if necessary
		if(leftOver != 0) {
			c = posSum - leftOver;
			for(int i = 0; i < leftOver; i++) {
				queueList[i][queueCounts[i]] = redist[c];
				memset(&redist[c], 0, sizeof(PCB));
				queueCounts[i]++;
				c++;
			} // end for loop
		} // end if

		//resorting queues if necessary
		for(int i = 0; i < totalProcessors; i++) {
			if(pProtocols[i] == 4) {
				qsort(queueList[i], totalProcesses, sizeof(PCB), priorityComparator);
			}
			if(pProtocols[i] == 3) {
				qsort(queueList[i], totalProcesses, sizeof(PCB), burstTimeComparator);
			}
		}

		//printing queues after distribution
		printf("Queues After Redistribution:\n\n");
		for(int i = 0; i < totalProcessors; i++) {
			printf("Processes in Queue %d\n", i);
			for(int j = 0; j < totalProcesses; j++) {
				if(queueList[i][j].burstTime == 0)
					break;
				printf("\nProcess #%d", queueList[i][j].processID);
			}
			printf("\n\n");
		}

		pthread_mutex_unlock(&myMutex);
	} // end supervisor while
	return (void*)0;
} // end supervisorThread


int main(int argc, char *argv[]) {
	//error checking for format
	if(argc % 2 != 0 && argc != 2) {
		printf("ERROR: Program called incorrectly\n");
		printf("Correct Usage: <%s> <BinaryFileName> <SchedulingProtocolNumber> <ProcessDistributionPercentage> ...\n", argv[0]);
		printf("At least one <SchedulingProtocolNumber>, <ProcessDistributionPercentage> pair must be entered\n");
		return -1;
	}
	//error checking SchedulingProtocolNumbers are valid
	int errFlag = 0;
	int a = 2;
	while(a < argc) {
		if(atoi(argv[a]) != 1 && atoi(argv[a]) != 2 && atoi(argv[a]) != 3 && atoi(argv[a]) != 4)
			errFlag++;
		a += 2;
	}
	if(errFlag != 0) {
		printf("ERROR: Invalid scheduling protocol number\n");
		printf("Valid scheduling protocol numbers are:\n");
		printf("1 (First Come First Served)\n");
		printf("2 (Round Robin)\n");
		printf("3 (Shortest Job First)\n");
		printf("4 (Priority Scheduling)\n");
		return -1;
	}
	//error checking that total workload = 1
	a = 3;
	float sum;
	while(a < argc) {
		sum += atof(argv[a]);
		a += 2;
	}
	if((int)sum != 1) {
		printf("ERROR: Invalid Process Distribution Percentage\n");
		printf("Process distribution percentages must add up to 1\n");
		return -1;
	}


	//calculating the total number of processors being used
	totalProcessors = (argc-2)/2;
	printf("The simulation will use %d processors\n\n", totalProcessors);

	FILE * pFile = NULL;
	pFile = fopen(argv[1], "rb");
	if(pFile == NULL) {
		printf("ERROR: File could not be opened\n");
	}
	fseek(pFile, 0, SEEK_END);
	int fileLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	printf("The file length is %d bytes\n", fileLength);
	totalProcesses = fileLength/(sizeof(PCB));
	printf("There are %d total processes\n", totalProcesses);

	PCB test[totalProcesses];


	//reading the binary file for information on all the given processes
	int i = 0;
	while(i != totalProcesses) {
		fread(&test[i].priority, sizeof(test->priority), 1, pFile);
		fread(&test[i].name, sizeof(test->name), 1, pFile);
		fread(&test[i].processID, sizeof(test->processID), 1, pFile);
		fread(&test[i].activityStatus, sizeof(test->activityStatus), 1, pFile);
		fread(&test[i].burstTime, sizeof(test->burstTime), 1, pFile);
		fread(&test[i].baseRegister, sizeof(test->baseRegister), 1, pFile);
		fread(&test[i].limitRegister, sizeof(test->limitRegister), 1, pFile);
		fread(&test[i].processType, sizeof(test->processType), 1, pFile);
		fread(&test[i].numberOfFiles, sizeof(test->numberOfFiles), 1, pFile);
		i++;
	}
	fclose(pFile);

	//printing the information on every process
	int j = 0;
	while(j != totalProcesses) {
		printf("\nProcess %d Info:\n\n", j);
		printf("Priority: %c\n", test[j].priority);
		printf("Name: %s\n", test[j].name);
		printf("processID: %d\n", test[j].processID);
		printf("activityStatus: %d\n", test[j].activityStatus);
		printf("burstTime: %d\n", test[j].burstTime);
		printf("baseRegister: %d\n", test[j].baseRegister);
		printf("limitRegister: %ld\n", test[j].limitRegister);
		printf("processType: %d\n", test[j].processType);
		printf("numberOfFiles: %d\n", test[j].numberOfFiles);
		j++;
	}
	printf("\n");

	//initializing the queues used by the processors
	queueList = (PCB**)malloc(totalProcessors * sizeof(PCB*));
	for(int i = 0; i < totalProcessors; i++) {
		queueList[i] = (PCB*)malloc(totalProcesses * sizeof(PCB));
	}

	//initializing the queues to 0
	for(int i = 0; i < totalProcessors; i++) {
		for(int j = 0; j < totalProcesses; j++) {
			memset(&queueList[i][j], 0, sizeof(PCB));
		}
	}

	//distributing the processes among the queues
	int counter = 0;
	int x = 3;
	int qCounts[totalProcessors];
	for(int i = 0; i < totalProcessors; i++) {
		//determining distribution percentage
		float percent = atof(argv[x]);
		int processNum = (percent * totalProcesses);
		int k = 0;
		for(int j = counter; j < (counter + processNum); j++) {
			queueList[i][k] = test[j];
			k++;
		}
		qCounts[i] = processNum;
		counter = counter + processNum;
		x += 2;
	}
	queueCounts = qCounts;

	// printing initial process distributions
	printf("Initial Process Distributions:\n\n");
	for(int i = 0; i < totalProcessors; i++) {
		printf("Processes in Queue %d\n", i);
		for(int j = 0; j < totalProcesses; j++) {
			if(queueList[i][j].burstTime == 0)
				break;
			printf("\nProcess #%d", queueList[i][j].processID);
		}
		printf("\n\n");
	}

	//creating threads for each processor and the supervisor
	pthread_create(&supervisor, NULL, supervisorThread, (void*)0);

	int schedulers[totalProcessors];
	pthread_t threadArray[totalProcessors];
	int protocolCounter = 2;
	for(int i = 0; i < totalProcessors; i++) {
		threadInfo * v = malloc(sizeof(threadInfo));
		v->processorID = i;
		if(atoi(argv[protocolCounter]) == 1) {
			pthread_create(&threadArray[i], NULL, firstComeFirstServed, (void*)v);
			schedulers[i] = atoi(argv[protocolCounter]);
			printf("Created Processor#%d using First Come First Served\n", i);
		}
		else if(atoi(argv[protocolCounter]) == 2) {
			pthread_create(&threadArray[i], NULL, roundRobin, (void*)v);
			schedulers[i] = atoi(argv[protocolCounter]);
			printf("Created Processor#%d using Round Robin\n", i);
		}
		else if(atoi(argv[protocolCounter]) == 3) {
			pthread_create(&threadArray[i], NULL, shortestJobFirst, (void*)v);
			schedulers[i] = atoi(argv[protocolCounter]);
			printf("Created Processor#%d using Shortest Job First\n", i);
		}
		else {
			pthread_create(&threadArray[i], NULL, priorityScheduling, (void*)v);
			schedulers[i] = atoi(argv[protocolCounter]);
			printf("Created Processor#%d using Priority Scheduling\n", i);
		}
		protocolCounter += 2;
	}
	pProtocols = schedulers;
	printf("\n");

	//joining the threads
	pthread_join(supervisor, NULL);
	for(int i = 0; i < totalProcessors; i++) {
		pthread_join(threadArray[i], NULL);
	}

	printf("\nAll processes have been executed\nExiting simulation...\n");

	//freeing allocated memory
	for(int i = 0; i < totalProcessors; i++) {
		free(queueList[i]);
	}
	free(queueList);
	return 0;
} // end main
