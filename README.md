# CPU-Scheduler-Simulation
A C program designed to simulate a CPU with multiple processors that are independent of each other.
The goal of this project is to practice different scheduling algorithms used by CPUs and to practice using multiple threads at one time in a program.
The instructions for the project are as follows:

1) Read the binary file and print out the number of processes available in the file, the total
number of memory allocated by the processes and the overall number of open files –
considering all the processes. The binary file with the PCBs will be provided as command
line argument with the structure described above. [The test file will be same in structure
but not exactly same in size and content as the example provided with the assignment.
Make sure to create a generic solution to read all files following the structure mentioned
earlier.]

2) The different processors and their governing scheduling algorithm will be provided as
command line arguments (variable number), along with their initial load.
Example: ./scheduling PCB.bin 1 0.4 2 0.5 4 0.1
meaning that there will be 3 processors running the simulation. The first one running
Priority Scheduling (see 4) and 40% (see 0.4) of the original processes from the process
file (see PCB.bin) should be assigned to this processor. The second one will run Shortest
Job First (see 3) and 50% (see 0.5) of the PCB.bin file will be assigned to this processor.
Finally, the third processor will run based on First Come First Served (see 1) and the
remaining 10% (see 0.1) of the processes from the PCB.bin (also provided as command
line argument) will run on this processors. It is recommended (not enforced though) to
populate originally the ready queues of each processor in a sequential order from the
original file, meaning that the first 40% of the PCB records can go to the ready queue of
processor 1, the next 50% of the PCB records can be assigned to the second processor’s
ready queue, etc. Random assignment is also allowed, but the data percentage should be
strictly considered. Each processor should run in a separate thread. The number of
processors and their corresponding load is not limited. It can be one processor, it can be
two processors and in general it can be n processors. However, their cumulative load
should be always 100%. Everything depends only on how you call the program from the
command line.

3) When the process ends up at the beginning in the “ready queue” (when you start the
experiment) make sure you allocate the corresponding memory using calloc()/malloc() or
new and make sure when a process is finished (CPU burst becomes 0) you have to
deallocate the memory for each process.

4) Each process is “executed” when it is its “turn” (considering the given scheduling
algorithm) by decrementing the CPU burst with a certain value. Let that quantum be 2.
To mimic the “execution” introduce a sleep() for a certain number of milliseconds. Ex. 200
or 400 milliseconds. (Make sure that you are not waiting forever to finish the simulation!)
You can store the processes in a ready queue (each processor will have its own).

5) For the priority scheduling (if any involved) an aging mechanism should kick in at each
time quantum of 5 seconds. In that case the priority of each process from the priority
based scheduling queue should be increased by one. Do not forget the priority is inversely
proportional with the priority number!

6) When one ready queue is empty (all the processes originally have been executed from
that particular processor using the selected scheduling algorithm) the system should
perform a load balancing (moving processes from the other ready queues into the empty
one in such a way to balance out (have equal or close to equal number of processes) the
system.

7) For the load balancing you can select the processor which has the most processes yet to
run and do a balancing between that processor and the one which has no processes to
run. You can also distribute the load from all existing processors
which is an optimal solution. Once the load balancing has happened, each CPU is running
its own scheduling algorithm as initially provided in the command line. If a processor is
not involved in the load balancing that processor will continue executing its processes
without stopping.

8) Each step in the simulation should be documented (printed) on the screen to see which
processor is running which process, using what algorithm, what CPU burst has left, what
is their current priority for those where priority does matter, when the load balancing is
happening, which ready queues are involved in the load balancing, how many processes
were transferred, when a processor completed the processes allocated to it, etc.

9) The simulation stops when all the processes from the original file have been “executed".
