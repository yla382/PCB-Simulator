#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h" 

typedef struct MSG {
	int receivedFromPID;  //PID of the process that sent this message
	char msg[40];
} MSG;

typedef struct PCB {
	int pid;
	int priority; //0 ~ HIGH, 1 ~ NORMAL, 2 ~ LOW
	int state;    //0 ~ running, 1 ~ ready, 2 ~ blocked
	LIST *msg;    //List of *MSG
	MSG *receiveRightAway; //MSG that needs to be displayed when the process runs
	int sendPid;
} PCB;

typedef struct SEM {
	int sid;
	int value;
	int isInitialized;  //1 ~ value initialized, 0 ~ value initialized
	LIST *blockedQ;     //List of PCB that are blocked by the semaphore
} SEM;

int generatePid;
LIST *readyQ[3];
LIST *sendQ;
LIST *receiveQ;
PCB *init;
PCB *runningProcess;
SEM *sems[5];
int control;

void intro() {  //Runs when the program starts
	printf("------------------------------------------\n");
	printf("PCB’s and Process Scheduling Simulation\n");
	printf("         Begin your command\n");
	printf("------------------------------------------\n");
	printf("\n");
}

void FreeItemMSG(void *item) {   //Function for ListFree to use to free MSG
	MSG *temp = (MSG*)item;
	free(temp);
}

void FreeitemPCB(void *item) {   //Funtion for ListFree ot use to free PCB 
	PCB *temp = (PCB*)item;
	free(temp->receiveRightAway);
	ListFree(temp->msg, FreeItemMSG);
	free(temp);
}

void FreeItemSem(SEM* temp) {    //Frees Semaphores
	ListFree(temp->blockedQ, FreeitemPCB);
	free(temp);
}

PCB *CreateInit(int id) {        //Generates INIT process
	PCB *process = (PCB*) malloc(sizeof(PCB));
	process->pid = id;
	process->priority = -1;      //-1 for special process to indicate that it has no priority
	process->state = 0;
	process->msg = NULL;
	process->sendPid = -1;
	return process;
}

void Create(int priority) {
	if(priority < 0 || priority > 3) {        //When input is in valid priority
		printf("Fail: Invalid priority number\n");
	} else {
		generatePid++;                             //Generating new process
		PCB *process = (PCB*) malloc(sizeof(PCB));
		process->pid = generatePid;
		process->priority = priority;
		process->state = 1;
		process->msg = ListCreate();
		process->sendPid = -1;
		process->receiveRightAway = NULL;
	
		if(runningProcess == init && ListCount(readyQ[0]) == 0 && ListCount(readyQ[1]) == 0 && ListCount(readyQ[2]) == 0) { //Runs the newly created process if current running
			init->state = 1;
			runningProcess = process;                                                                                       //process is INIT process and readyQs are all empty
			process->state = 0;
			printf("Sucess: Process (PID: %d) created and running now\n", process->pid);
		} else {
			int status = ListAppend(readyQ[priority], process); //Append the newly created process
			if(status == 0) {  //checks if ListAppend executed successfully 
				printf("Sucess: Process (PID: %d) created\n", ((PCB*)ListCurr(readyQ[priority]))->pid);
			} else {
				printf("Fail: Unable to create process\n");
			}
		}
	}

	printf("\n");
}

void Fork() { 
	if(runningProcess->pid == 0) {  //Check if the running process is INIT process
		printf("Error: Init process cannot be forked\n");
	} else {
		generatePid++;
		PCB *forked_process = (PCB*) malloc(sizeof(PCB));
		forked_process->pid = generatePid;
		forked_process->priority = runningProcess->priority;
		forked_process->state = 1;
		forked_process->msg = ListCreate();
		forked_process->sendPid = -1; 
		forked_process->receiveRightAway = NULL;
		ListAppend(readyQ[runningProcess->priority], forked_process);
		printf("New forked process (pid: %d) created and placed in ready queue\n", forked_process->pid);
	}
	printf("\n");
}

int comparator(void *a, void *b){
	if(((PCB*)a)->pid == *(int*)b) {
		return 1;
	} else {
		return 0;
	}
}
void Exit();

void Kill(int pid) { 	
	if(pid == runningProcess->pid) { //Check if the process wants to kill itself
		printf("Success: Process with PID: %d killed\n\n", pid);
		Exit();
		return;
	}

	if(pid == 0) {  //Check if the process wants to kill INIT process
		printf("Error: Process cannot kill init process\n\n");
		return;
	}

	PCB *search_process = 0;
	for(int i = 0; i < 3; i++) { //Search for the process with specified pid in the readyQ
		if(ListCount(readyQ[i]) > 0) {
			ListFirst(readyQ[i]);
			search_process = ListSearch(readyQ[i], comparator, &pid);
			if(search_process != 0) {
				break;
			}
		}
	}

	if(search_process != 0) { //Check if the process is found
		printf("Success: Process with PID: %d killed\n\n", search_process->pid);
		ListRemove(readyQ[search_process->priority]);
		//free(search_process);
		FreeitemPCB(search_process);
		return;
	}

	search_process = ListSearch(sendQ, comparator, &pid); //Search for the process with specified pid in the sendQ
	if(search_process != 0) { //Check if the process is found
		printf("Success: Process with PID: %d killed\n\n", search_process->pid);
		ListRemove(sendQ);
		//free(search_process);
		FreeitemPCB(search_process);
		return;
	}

	search_process = ListSearch(receiveQ, comparator, &pid); //Search for the process with specified pid in the receiveQ
	if(search_process != 0) { //Check if the process is found
		printf("Success: Process with PID: %d killed\n\n", search_process->pid);
		ListRemove(receiveQ);
		//free(search_process);
		FreeitemPCB(search_process);
		return;
	}

	for(int i = 0; i < 5; i++) {
		if(ListCount(sems[i]->blockedQ) > 0) { //Search for the process with specified pid in the sems
			search_process = ListSearch(sems[i]->blockedQ, comparator, &pid);
			if(search_process != 0) { //Check if the process is found
				printf("Success: Process with PID: %d killed\n\n", search_process->pid);
				ListRemove(receiveQ);
				//free(search_process);
				FreeitemPCB(search_process);
				return;
			}
		}
	}

	printf("Fail: No process with PID: %d\n\n", pid);
}

void Exit() { 
	if(runningProcess->pid == 0) {  //Check if the running process is INIT process
		if(ListCount(readyQ[0]) == 0 && ListCount(readyQ[1]) == 0 && ListCount(readyQ[2]) == 0) { //Check if the the readyQs are empty -> if so allow for INIT to be exited
			free(init);
			init = NULL; //remember to make it kill the program
			runningProcess = NULL;
			control = 0;
		} else {	
			printf("Fail: Cannot Kill Init process while there are processes left in ready queue\n");
		}
	} else {
		int check = 0;
		for(int i = 0; i < 3; i++) { //Search for the next process to run in the readyQs
			if(ListCount(readyQ[i]) > 0) {
				check++;
				ListFirst(readyQ[i]);
				//free(runningProcess);
				FreeitemPCB(runningProcess);
				runningProcess = ListRemove(readyQ[i]);
				printf("Success: Now runnning process PID: %d\n", runningProcess->pid);
				break;
			}
		}
		if(check == 0) {  //Check if readyQs is empty -> if so run INIT process
			//free(runningProcess);
			FreeitemPCB(runningProcess);
			init->state = 0;
			runningProcess = init;
			//FreeitemPCB(runningProcess);
			printf("Success: Now running init process\n");
		}
	}
	printf("\n");
}

void Quantum() { 
	if(ListCount(readyQ[0]) == 0 && ListCount(readyQ[1]) == 0 && ListCount(readyQ[2]) == 0) {
		if(runningProcess == 0) { //Check if there are any running process
			runningProcess = init;
			printf("No process available in ready Queue, now running Init process\n");
		} else if(runningProcess->pid != 0) { //Check if running process is not INIT process
			//ListAppend(readyQ[runningProcess->priority], runningProcess);
			//runningProcess = init;
			//printf("No process available in ready Queue, now running Init process\n");
			printf("No process available in ready Queue, continue running process PID: %d\n", runningProcess->pid);
		} else {
			printf("No process available in ready Queue, continue running Init process\n");
		}
	} else {
		for(int i = 0; i < 3; i++) {  //Search for next process to run in readyQs
			if(ListCount(readyQ[i]) > 0) {
				ListFirst(readyQ[i]);
				PCB* temp = ListRemove(readyQ[i]);
				if(runningProcess == 0) {
					
				} else if(runningProcess->pid == 0) {

				} else {
					runningProcess->state = 1;
					ListAppend(readyQ[runningProcess->priority], runningProcess);
				}
				runningProcess = temp;
				runningProcess->state = 0;
				printf("Now running process PID: %d\n", runningProcess->pid);
				if(runningProcess->receiveRightAway != NULL) { //Check if there are any messages to be displayed right away
					MSG* temp = runningProcess->receiveRightAway;
					runningProcess->receiveRightAway = NULL;
					printf("From PID: %d\n", temp->receivedFromPID);
					printf("Message: %s\n", temp->msg);
					free(temp);
				}
				break;
			}
		}
	}
	printf("\n");
}

void transferMsg(int pid, char *m, PCB* p) { //Transfer messages to the specified process
	MSG *msg = (MSG*) malloc(sizeof(MSG));
	msg->receivedFromPID = pid;
	strcpy(msg->msg, m);
	ListAppend(p->msg, msg);
}

int Send(int pid, char *m) {
	PCB *temp = 0;
	if(runningProcess->pid == 0) { //Check if the running process is INIT process
		printf("Error: Init process cannot send messages\n");
		printf("\n");
		return 0;
	}  else if(pid == 0) { //Check if the process wants to send the message to INIT process
		printf("Error: Init process cannot receive messages\n");
		printf("\n");
		return 0;
	} else if(runningProcess->pid == pid) { //Check if the process wants to send the messages to itself
		printf("Error: process cannot send messages to itself\n");
		printf("\n");
		return 0;
	} //else if(ListCount(readyQ[0]) == 0 && ListCount(readyQ[1]) == 0 && ListCount(readyQ[2]) == 0){
	//	printf("Error: No process availalbe to send messages\n");
	//	printf("\n");
	//	return 0;
	//} 
	else {
		if(ListCount(receiveQ) != 0) { //Search for the specified process in the receiveQ
			ListFirst(receiveQ);
			temp = ListSearch(receiveQ, comparator, &pid);
			if(temp != 0) {
				MSG *msg = (MSG*) malloc(sizeof(MSG));
				msg->receivedFromPID = runningProcess->pid;
				strcpy(msg->msg, m);
				temp->receiveRightAway = msg;
				//transferMsg(pid, m, temp);
				ListRemove(receiveQ);
				temp->state = 1;
				ListAppend(readyQ[temp->priority], temp);
				runningProcess->sendPid = pid;
				runningProcess->state = 2;
				ListAppend(sendQ, runningProcess);
				runningProcess = 0;
				printf("Success: Message sent to PID: %d\n", temp->pid);
				Quantum();
				printf("\n");
				return 1;
			}
		}

		for(int i = 0; i < 3; i++) { //Search for the specified process in the readyQs
			if(ListCount(readyQ[i]) > 0) {
				ListFirst(readyQ[i]);
				temp = ListSearch(readyQ[i], comparator, &pid);
				if(temp != 0) {
					transferMsg(runningProcess->pid, m, temp);
					runningProcess->sendPid = pid;
					runningProcess->state = 2;
					ListAppend(sendQ, runningProcess);
					runningProcess = 0;
					printf("Success: Message sent to PID: %d\n", temp->pid);
					Quantum();
					printf("\n");
					return 1;
				}
			}
		}

		if(ListCount(sendQ) > 0) {  //Search for the specified process in the sendQ
			ListFirst(sendQ);
			temp = ListSearch(sendQ, comparator, &pid);
			if(temp != 0) {
				transferMsg(runningProcess->pid, m, temp);
				runningProcess->sendPid = pid;
				runningProcess->state = 2;
				ListAppend(sendQ, runningProcess);
				runningProcess = 0;
				printf("Success: Message sent to PID: %d\n", temp->pid);
				Quantum();
				printf("\n");
				return 1;
			}
		}

		for(int i = 0; i < 5; i++) { //Search for the specified process in the sems
			if(ListCount(sems[i]->blockedQ) > 0) {
				temp = ListSearch(sems[i]->blockedQ, comparator, &pid);
				if(temp != 0) {
					transferMsg(runningProcess->pid, m, temp);
					runningProcess->sendPid = pid;
					runningProcess->state = 2;
					ListAppend(sendQ, runningProcess);
					runningProcess = 0;
					printf("Success: Message sent to PID: %d\n", temp->pid);
					Quantum();
					printf("\n");
					return 1;
				}
			}
		}

		printf("Fail: No such process PID: %d can be found\n", pid);
		printf("\n");
		return 0; 
	}
}

void Receive() {
	if(runningProcess->pid == 0) { //Check if running process is INIT process
		printf("Error: Init process cannot receive messages\n");
	} else {
		if(ListCount(runningProcess->msg) == 0) {  //Check if the process has any messages to recieve
			printf("No current pending msgs to receive. Currently Process will be blocked");
			runningProcess->state = 2;
			ListAppend(receiveQ, runningProcess);
			runningProcess = 0;
			Quantum();
		} else {
			ListFirst(runningProcess->msg);
			MSG* temp = (MSG*)(ListRemove(runningProcess->msg));
			printf("From PID: %d\n", temp->receivedFromPID);
			printf("Message: %s\n", temp->msg);
			free(temp);
		}
	}
	printf("\n");
}

void Reply(int pid, char *msg) {
	if(runningProcess->pid == 0) { //Check if the running process is INIT process
		printf("Fail: Init process cannot reply to any processes\n");
	} else if(ListCount(sendQ) == 0) {  //Check if sendQ is zero
		printf("Fail: No processes have sent messages yet\n");
	} else {
		PCB* temp = 0;
		ListFirst(sendQ);
		temp = ListSearch(sendQ, comparator, &pid); //Search for the specified process in sendQ to reply message to
		if(temp != 0) {
			if(temp->sendPid == runningProcess->pid) {
				//transferMsg(runningProcess->pid, msg, temp);
				MSG *m = (MSG*) malloc(sizeof(MSG));
				m->receivedFromPID = runningProcess->pid;
				strcpy(m->msg, msg);
				ListRemove(sendQ);
				temp->sendPid = -1;
				temp->state = 1;
				temp->receiveRightAway = m;
				ListAppend(readyQ[temp->priority], temp);
				printf("Success: Message replied to Process with PID: %d\n", pid);
			} else {
				printf("Fail: process with PID: %d did not messages to current running process\n", pid);
			}
		} else {
			printf("Fail: No success process with PID: %d exists\n", pid);
		}
	}
	printf("\n");
}

void NewSem(int sid, int value) {
	if(sid < 0 || sid > 4) {  //Check if the sid is valid
		printf("Fail: Invalid semaphore id\n");
	} else if(sems[sid]->isInitialized == 1) { //Check if the specified semaphore is initialized or not
		printf("Fail: Semaphore with SID: %d, already initialized\n", sid);
	} else {
		sems[sid]->isInitialized = 1;
		sems[sid]->value = value;
		printf("Success: Semaphore SID: %d, given value: %d\n", sid, value);
	}
	printf("\n");
}

void PSem(int sid) {
	if(sid < 0 || sid > 4) { //Check if the sid is valid
		printf("Fail: Invalid semaphore id\n");
	} else if (sems[sid]->isInitialized == 0) { //Check if the specified semaphore is initialized or not
		printf("Fail: Semaphore SID: %d's value have not been initialized\n", sid);
	} else {
		sems[sid]->value = sems[sid]->value - 1;
		if(sems[sid]->value < 0 && runningProcess->pid != 0) { //Check if value is < 0 to block the running process and check if the running process is INIT Process
			runningProcess->state = 2;
			ListAppend(sems[sid]->blockedQ, runningProcess);
			runningProcess = 0;
			printf("Success: current process blocked\n");
			Quantum();
		} else {
			printf("Success: current process still running\n");
		}
	}
	printf("\n");
}

void VSem(int sid) {
	if(sid < 0 || sid > 4) { //Check if the specified semaphore is initialized or not
		printf("Fail: Invalid semaphore id\n");
	} else if(sems[sid]->isInitialized == 0) {
		printf("Fail: Semaphore SID: %d's value have not been initialized\n", sid);
	} else {
		sems[sid]->value = sems[sid]->value + 1;
		if(sems[sid]->value >= 0) { //Check if the value is >= 0 in order free the process 
			if(ListCount(sems[sid]->blockedQ) > 0) {
				ListFirst(sems[sid]->blockedQ);
				PCB* temp = ListRemove(sems[sid]->blockedQ);
				temp->state = 1;
				ListAppend(readyQ[temp->priority], temp);
				printf("Success: Process PID: %d returned to its readyQ\n", temp->pid);
			} else {
				printf("Success: Semaphore SID: %d has value: %d\n", sid, sems[sid]->value);
			}
		} else {
			printf("Success: Semaphore SID: %d has value: %d\n", sid, sems[sid]->value);
		}
	}
	printf("\n");
}

PCB* searchInReadyQ(int pid) { //Search for process with specifided pid in readyQs
	PCB* temp = 0;
	for(int i = 0; i < 3; i++) {
		ListFirst(readyQ[i]);
		temp = ListSearch(readyQ[i], comparator, &pid);
		if(temp != 0) {
			return temp;
		}
	}
	return temp;
}

PCB* searchPCB(LIST* list, int pid) { //Search for process with specifided pid in specified LIST
	PCB* temp = 0;
	ListFirst(list);
	temp = ListSearch(list, comparator, &pid);
	return temp;
}

void displayInfo(PCB* p) {
	if(p->pid == 0) { //Check p is running process or not
		printf("INIT Process\n");
		printf("State: ");
		if(init->state == 0) {
			printf("Running\n");
		} else {
			printf("Ready\n");
		}
	} else {
		printf("Process ID: %d\n", p->pid);
		printf("Priority: ");
		if(p->priority == 0) {
			printf("High\n");
		} else if(p->priority == 1) {
			printf("Normal\n");
		} else {
			printf("Low\n");
		}

		printf("State: ");
		if(p->state == 0) {
			printf("Running\n");
		} else if(p->state == 1) {
			printf("Ready\n");
		} else {
			printf("Blocked\n");
		}
	}
}

int Procinfo(int pid) {
	if(pid == 0) { 
		//displayInfo(init);
		printf("INIT Process\n");
		printf("State: ");
		if(init->state == 0) {
			printf("Running\n");
		} else {
			printf("Ready\n");
		}
		printf("\n");
		return 1;
	}

	if(pid == runningProcess->pid) {  //Check if the pid is the pid of the running process
		displayInfo(runningProcess);
		printf("\n");
		return 1;
	}

	PCB* temp = 0;
	temp = searchInReadyQ(pid); //Search for following process with pid in readyQs
	if(temp != 0) {
		displayInfo(temp);
		printf("Queue Location: ");
		if(temp->priority == 0) {
			printf("High priority ready queue\n");
		} else if(temp->priority == 1) {
			printf("Normal priority ready queue\n");
		} else {
			printf("Low priority ready queue\n");
		}
		printf("\n");
		return 0;
	}

	temp = searchPCB(sendQ, pid); //Search for following process with pid in sendQ
	if(temp != 0) {
		displayInfo(temp);
		printf("Queue Location: Send queue\n");
		printf("\n");
		return 1;
	}

	temp = searchPCB(receiveQ, pid); //Search for following process with pid in receiveQ
	if(temp != 0) {
		displayInfo(temp);
		printf("Queue Location: Receive queue\n");
		printf("\n");
		return 1;
	}

	for(int i = 0; i < 5; i++) { //Search for following process with pid in sems
		temp = searchPCB(sems[i]->blockedQ, pid);
		if(temp != 0) {
			displayInfo(temp);
			printf("Queue Location: Semaphore SID: %d\n", i);
			printf("\n");
			return 1;
		}
	}

	printf("Process with PID: %d not found\n\n", pid);
	return 0;
}

void TotalInfo() {
	printf("Displaying all process queues and their contents\n");
	printf("Current running process:\n");
	displayInfo(runningProcess);
	printf("----------------------------------------------------\n");
	if(runningProcess->pid != 0) {
		printf("INIT process:\n");
		printf("State: ");
		if(init->state == 0) {
			printf("Running\n");
		} else {
			printf("Ready\n");
		}
		printf("----------------------------------------------------\n");
	}
	int size;
	for(int i = 0; i < 3; i++) {  //Display processes in readyQs
		if(i == 0) {
			printf("High Priority Queue:\n");
		} else if(i == 1) {
			printf("Normal Priority Queue:\n");
		} else {
			printf("Low Priority Queue:\n");
		}
		size = ListCount(readyQ[i]);
		if(size > 0) {
			ListFirst(readyQ[i]);
			for(int j = 0; j < size; j++) {
				displayInfo(ListCurr(readyQ[i]));
				ListNext(readyQ[i]);
				if(j != size - 1) {
					printf("\n");
				}
			}
		} else {
			printf("Empty\n");
		}
		printf("----------------------------------------------------\n");
	}

	printf("Send Queue:\n"); //Display processes in sendQ
	size = ListCount(sendQ); 
	if(size > 0) {
		ListFirst(sendQ);
		for(int i = 0; i < size; i++) {
			displayInfo(ListCurr(sendQ));
			printf("Sent to: PID: %d\n", ((PCB*)ListCurr(sendQ))->sendPid);
			if(i != size - 1) {
				printf("\n");
			}
			ListNext(sendQ);
		}
	} else {
		printf("Empty\n");
	}

	printf("----------------------------------------------------\n");

	printf("Receive Queue:\n"); //Display processes in receiveQ
	size = ListCount(receiveQ);
	if(size > 0) {
		ListFirst(receiveQ);
		for(int i = 0; i < size; i++) {
			displayInfo(ListCurr(receiveQ));
			if(i != size - 1) {
					printf("\n");
			}
			ListNext(receiveQ);
		}
	} else {
		printf("Empty\n");
	}

	printf("----------------------------------------------------\n");

	for(int i = 0; i < 5; i++) { //Display processes in sems
		printf("Blocked queue by Semaphore SID: %d\n", i);
		size = ListCount(sems[i]->blockedQ);
		if(size > 0) {
			ListFirst(sems[i]->blockedQ);
			for(int j = 0; j < size; j++) {
				displayInfo(ListCurr(sems[i]->blockedQ));
				ListNext(sems[i]->blockedQ);
				if(j != size - 1) {
					printf("\n");
				}
			}
		} else {
			printf("Empty\n");
		}
		if(i != 4) {
			printf("----------------------------------------------------\n");
		}
	}

	printf("End of TotalInfo\n");
	printf("\n");
}

void FreeEverything() { //Free everything that is left in the queue
	free(runningProcess);
	free(init);
	for(int i = 0; i < 3; i++) {
		ListFree(readyQ[i], FreeitemPCB);
	}

	ListFree(sendQ, FreeitemPCB);
	ListFree(receiveQ, FreeitemPCB);

	for(int i = 0; i < 5; i++) {
		FreeItemSem(sems[i]);
	}

}

int main() {
	generatePid = 0;
	readyQ[0] = ListCreate();
	readyQ[1] = ListCreate();
	readyQ[2] = ListCreate();
	sendQ = ListCreate();
	receiveQ = ListCreate();
	control = 1;

	for(int i = 0; i < 5; i++) {
		SEM *sem = (SEM*) malloc(sizeof(SEM));
		sem->sid = i;
		sem->isInitialized = 0;
		sem->blockedQ = ListCreate();
		sems[i] = sem;
	}

	intro();

	init = CreateInit(generatePid);
	runningProcess = init;

	int i;
	int j;
	char *m;

	printf("Type in the Command: ");
	while(control == 1) {
		char c;
		scanf("%c", &c);

		switch(c)
		{
			case 'C':
				printf("Command: Create\n");
				printf("Choose priority(0 = high, 1 = norm, 2 = low): ");
				scanf("%d", &i);
				Create(i);
				break;

			case 'F':
				printf("Command: Fork\n");
				Fork();
				break;

			case 'K':
				printf("Command: Kill\n");
				printf("Choose process(pid): ");
				scanf("%d", &i);
				Kill(i);
				break;

			case 'E':
				printf("Command: Exit\n");
				Exit();
				break;

			case 'Q':
				printf("Command: Quantum\n");
				Quantum();
				break;

			case 'S':
				m = (char *)malloc(41*sizeof(char));
				printf("Command: Send\n");
				printf("Enter process(pid): ");
				scanf("%d", &i);
				printf("Enter sending message (MAX 40): ");
				getchar();
				fgets(m, 41, stdin);
				Send(i, m);
				free(m);
				printf("Type in the Command: ");
				break;

			case 'R':
				printf("Command: Recieve\n");
				Receive();
				break;

			case 'Y':
				m = (char *)malloc(41*sizeof(char));
				printf("Command: Reply\n");
				printf("Enter process(pid): ");
				scanf("%d", &i);
				printf("Enter replying message (MAX 40): ");
				getchar();
				fgets(m, 41, stdin);
				Reply(i, m);
				free(m);
				printf("Type in the Command: ");	
				break;

			case 'N':
				printf("Command: New Semaphore\n");
				printf("Enter semaphore id: ");
				scanf("%d", &i);
				printf("Enter value: ");
				scanf("%d", &j);
				NewSem(i, j);
				break;

			case 'P':
				printf("Command: Semaphore P\n");
				printf("Enter semaphore id: ");
				scanf("%d", &i);
				PSem(i);
				break;

			case 'V':
				printf("Command: Semaphore V\n");
				printf("Enter semaphore id: ");
				scanf("%d", &i);
				VSem(i);
				break;

			case 'I':
				printf("Command: Procinfo\n");
				printf("Enter PID: ");
				scanf("%d", &i);
				Procinfo(i);
				break;

			case 'T':
				printf("Command: TotalInfo\n");
				TotalInfo();
				break;

			case '\n':
				printf("Type in the Command: ");
				break;

			default:
				printf("Command not recognized\n");		
		}
	}

	FreeEverything();
	printf("PCB and Process Scheduling Simulation terminated\n");
	return 0;
}
