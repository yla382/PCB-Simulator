### PCB simulator
### Program Execution Manual:
1. Go to Terminal
2. Type "make"
3. Type "./run"

### Program Structure:
		Init: Special process that is automatically created and running when the program starts:

		Process Control Block:
					In the code made as struct "PCB"
					Contains:
							int pid -> Process ID
							int priority -> 0 ~ HIGH, 1 ~ NORMAL, 2 ~ LOW
							int state -> 0 ~ RUNNING, 1 ~ READY, 2 ~ BLOCKED
							LIST *msg -> LIST that contains all the received messages
							MSG *receiveRightAway -> Message that must be received when process runs again
									- Case 1: Process from Receive Queue returns to ready queue once it has recieved the message
									- Case 2: Process from Send Queue retturn to ready queue once it has been replied.
							int sendPid -> Process ID of the process this process has send message to. 


		Ready Queues: 3 queues for containing processes in READY state based on their priorities:
					In the code "readyQ[3]":
							readyQ[0] -> High priority
							readyQ[1] -> Normal priority
							readyQ[2] -> Low priority

		Send Queue: Queue that stores processes that has sent out messages and waiting for replies
					In the code "sendQ"

		Receieve Queue: Queue that store processes that is waiting to receive an message
					In the code "receiveQ"

		Message: Message that Process use to Send/Receive/Reply
					In the code mads as struct "MSG"
					Contains:
							int receivedFromPID -> Process ID of the process that has sent this Message
							char msg[41] -> The actual Message, size 41 due to the new line 

		Semaphore:
				In the code made as struct "SEM". There are only 5 semaphores in the program
				Contains:
						int sid -> Semaphore ID
						int value -> value that Semaphore uses to block and free processes
						int isInitialized -> Flag to check whether the following Semaphore has been intialized with value or not, (1 ~ Initialized, 0 ~ Not Initialized)
						LIST *blockedQ -> Contains list of processes that has been blocked by the Semaphore

### Program Commands:
		Create(C) 				- Creates a new Process
		Fork(F)   				- Copy the current running process with different PID
		Kill(K)	  				- Kill the process with specified PID
		Quantum(Q)				- Represents running time expired for the current running process and runs the next process based on scheduling algorithm
		Send(S)					- Process send messages to a process with specified PID
		Receive(R)				- Process received a message
		Reply(Y)				- Process replies to a process that has sent it a message
		New Semaphore(N)		- Initialize value of the semaphore of specified SID
		Semaphore P(P)			- Decrement the value of the semaphore of specified SID
		Semaphore V(V)		    - Increment the value of the semaphore of specified SID
		Procinfo(I)				- Displays the contents of the process of specified PID
		Totalinfo(T)			- Displays the contents of all the process in the system

### Program Design:
		INIT process:
				- Special process that only runs when there are no avilable processes in the readyQ
				- Cannot be blocked, state can only be either Running or Ready
				- Cannot be delete unless there are no other process in the readyQ
		Scheduling algorithm (Q)
				- Round Robin with 3 level of priority ready queues
						- When Q happens append the currently running process back into the ready queue based on its priority 
						- Start at HiGHEST Priority queue
						- If Highest priority is not empty, take out the first process from the queue and run
								- If Highest Priority is empty, move down to NORMAL priority queue
						- If Normal priority is not empty, take out the first process from the queue and run
								- If Normalk Priority is empty, move down to LOW priority queue
						- If LOW priority is not empty, take out the first process from the queue and run
								- If Normalk Priority is empty, continue running the current process
				- When current running process is INIT process
						- If all the ready queues are empty, it will continue to run when Q command occurs
						- If all the ready queues are empty and new process is created
								- The program now runs the newly created process automically without the Q commands

		Send/Recieve/Reply (S/R/Y)
				- INIT process cannot send/receive/reply messages
				- Processes cannot send/receive/reply INIT process
				- Send (S)
						- When process sends message to processes:
						- Once process sends messages it is send to SendQ and blocked
								- the message is stored at receiving process's message queue
						- Once the process is blocked, it cannot return to its readyQ until replied
				- Receieve (R)
						- When process receives an message (perform R command)
								- The process take out the first message in its message queue and display it
								- If the process's message queue is empty, this means that no message has been sent to it
										- This causes the process to be placed to receiveQ and blocked
												-Once process receives messages it is placed back to the readyQ when it runs, displays messages without R command
				- Reply (Y)
						- Process can only reply to process that has send it a message (Process in the sendQ).
						- Once the the process replies, the following process that receives reply will be placed back to the readQ, and when it runs displays the replied messages with R command

		Semaphore (N/P/V)
				- Any processes can perform N/P/V commmands
				- There are only 5 semaphores in the program
						- Making N/P/V with SID that is less than 0 or greater than 4 will result in Fail message
				- New Semaphore(N)
						- When this command occurs, it initializes the semaphore with specified SID with a value
						- Once the semaphore's value has been initialized, it cannot be re-initialized
				- Semaphore (P)
						- Decrements the value of the semaphore with specified SID
								- if the value is less than 0, the running process shalled be blocked and sent to semaphore's blocked queue
										- Next process to run will be decided based on scheduling algorithm mentioned above
										- INIT process can use P command, but it will not be blocked and stays running
				- Semaphore (V)
						- Increments the value of the semaphore with specified SID
								- if the value is greater or equal to 0, semaphore will release first item in its blocked queue and place it back to its ready queue

		Exit (E)
				- Exits the current running process (removed from the system)
				- Next running process determined by the previously mentioned Scheduling algorithm
				- INIT process cannot be exited with there are processes in the ready queue
						- once the INIT process is executed the whole program will terminate

		Kill (K)
				- Kills the process with specified PID (removed from the system)
				- Processes cannot kill INIT process
				- Processes are allowed to kill itself by specifying its own PID
						- INIT process can only kill itself if there are no processes in the ready queues

		Process Info (I)
				- Display the content of the process with specified PID
						- Displays : PID, state, priority, its location

		Total Info (T)
				- Displays the content of the every processes in the system
						- Displays : PID, state, priority, its location
								- if the process is in send queue, it will display its sendPID



