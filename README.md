# CPU-Scheduler-Simulation
A C program designed to simulate a CPU with multiple processors that are independent of each other.
The program takes a binary file containing PCB information about multiple processes and distributes them among a user-set number of simulated processors.
Each processor is handled by its own thread. On program execution, the user sets the number of processors that the imaginary CPU has, the scheduling algorithm
of each processor, and the workload distribution among the processors. The program will also rebalance the queues of each processor in order to insure that no
processor is stitting idle while there are still processes that need to be executed.
