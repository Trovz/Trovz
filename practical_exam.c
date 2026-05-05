#include<stdio.h>
#define MAX 100
#define INT_MAX 99999

// STRUCTURE for Process Information
typedef struct {
    int pid;         // Process ID (P1, P2, P3...)
    int arrival;     // Arrival Time
    int burst;       // Burst Time (execution time needed)
    int priority;    // Priority value (lower number = higher priority)
    int remaining;   // Remaining Burst Time (used in preemptive algorithms)
    int completion;  // Completion Time
    int turnaround;  // Turnaround Time
    int waiting;     // Waiting Time
    int finished;    // Flag if process is finished (1 = done, 0 = not yet)
} Process;


/*
    FUNCTION: copyProcesses()
    PURPOSE:
        Copies process data from src[] to dest[]
        Used so the original process list stays unchanged.
*/
void copyProcesses(Process dest[], Process src[], int n) {
    for (int i = 0; i < n; i++)
        dest[i] = src[i];
}


/*
    FUNCTION: getInput()
    PURPOSE:
        Takes user input for arrival time, burst time,
        and optionally priority time.

    PARAMETERS:
        p[] = process array
        n = number of processes
        needPriority = if 1, it asks for priority input
*/
void getInput(Process p[], int n, int needPriority) {
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;

        printf("\n  Process P%d:\n", i + 1);

        printf("    Arrival Time : ");
        scanf("%d", &p[i].arrival);

        printf("    Burst Time   : ");
        scanf("%d", &p[i].burst);

        // Only ask priority if algorithm needs it
        if (needPriority) {
            printf("    Priority     : ");
            scanf("%d", &p[i].priority);
        }

        // Initialize other values
        p[i].remaining  = p[i].burst;
        p[i].finished   = 0;
        p[i].completion = 0;
        p[i].turnaround = 0;
        p[i].waiting    = 0;
    }
}


/*
    FUNCTION: printResults()
    PURPOSE:
        Displays the results of scheduling including:
        - Completion time
        - Turnaround time
        - Waiting time
        - Average turnaround time
        - Average waiting time
*/
void printResults(Process p[], int n, const char *algoName) {
    float totalTAT = 0, totalWT = 0;

    printf("\n========================================\n");
    printf("  Results: %s\n", algoName);
    printf("========================================\n");
    printf("%-5s %-9s %-7s %-12s %-12s %-10s\n",
           "PID", "Arrival", "Burst", "Completion", "Turnaround", "Waiting");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        printf("P%-4d %-9d %-7d %-12d %-12d %-10d\n",
               p[i].pid,
               p[i].arrival,
               p[i].burst,
               p[i].completion,
               p[i].turnaround,
               p[i].waiting);

        totalTAT += p[i].turnaround;
        totalWT  += p[i].waiting;
    }

    printf("------------------------------------------------------------\n");
    printf("  Average Turnaround Time : %.2f\n", totalTAT / n);
    printf("  Average Waiting Time    : %.2f\n", totalWT  / n);
    printf("========================================\n");
}


/*
    FUNCTION: fcfs()
    PURPOSE:
        First-Come First-Serve Scheduling.
        Process executed based on earliest arrival time.

    HOW IT WORKS:
        1. Sort processes by arrival time
        2. Execute each process fully in order
*/
void fcfs(Process p[], int n) {

    // Sort by arrival time using bubble sort
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (p[i].arrival > p[j].arrival) {
                Process tmp = p[i];
                p[i] = p[j];
                p[j] = tmp;
            }

    int time = 0;

    for (int i = 0; i < n; i++) {

        // If CPU is idle, jump to arrival time
        if (time < p[i].arrival)
            time = p[i].arrival;

        // Completion time = current time + burst time
        p[i].completion  = time + p[i].burst;

        // Turnaround = completion - arrival
        p[i].turnaround  = p[i].completion - p[i].arrival;

        // Waiting = turnaround - burst
        p[i].waiting     = p[i].turnaround - p[i].burst;

        // Move time forward
        time             = p[i].completion;
    }

    printResults(p, n, "First-Come, First-Serve (FCFS)");
}


/*
    FUNCTION: sjfNonPreemptive()
    PURPOSE:
        Shortest Job First Non-Preemptive Scheduling.

    HOW IT WORKS:
        1. At each time, pick the process with smallest burst time
        2. Run it completely until it finishes
*/
void sjfNonPreemptive(Process p[], int n) {
    int done = 0, time = 0;

    while (done < n) {
        int idx = -1, minBurst = INT_MAX;

        // Find the shortest burst among arrived processes
        for (int i = 0; i < n; i++) {
            if (!p[i].finished && p[i].arrival <= time) {
                if (p[i].burst < minBurst) {
                    minBurst = p[i].burst;
                    idx = i;
                }
            }
        }

        // If no process has arrived yet, increase time
        if (idx == -1) { 
            time++; 
            continue; 
        }

        // Run chosen process fully
        time             += p[idx].burst;
        p[idx].completion = time;
        p[idx].turnaround = p[idx].completion - p[idx].arrival;
        p[idx].waiting    = p[idx].turnaround - p[idx].burst;
        p[idx].finished   = 1;

        done++;
    }

    printResults(p, n, "Shortest Job First - Non-Preemptive");
}


/*
    FUNCTION: sjfPreemptive()
    PURPOSE:
        Shortest Remaining Time First (SRTF)

    HOW IT WORKS:
        1. At every time unit, pick the process with smallest remaining time
        2. Execute 1 unit, then check again
        3. This allows preemption (switching)
*/
void sjfPreemptive(Process p[], int n) {
    int done = 0, time = 0;

    while (done < n) {
        int idx = -1, minRemaining = INT_MAX;

        // Find process with smallest remaining time
        for (int i = 0; i < n; i++) {
            if (!p[i].finished && p[i].arrival <= time) {
                if (p[i].remaining < minRemaining) {
                    minRemaining = p[i].remaining;
                    idx = i;
                }
            }
        }

        // If no process is available, increase time
        if (idx == -1) { 
            time++; 
            continue; 
        }

        // Execute 1 unit
        p[idx].remaining--;
        time++;

        // If process is done
        if (p[idx].remaining == 0) {
            p[idx].completion = time;
            p[idx].turnaround = p[idx].completion - p[idx].arrival;
            p[idx].waiting    = p[idx].turnaround - p[idx].burst;
            p[idx].finished   = 1;
            done++;
        }
    }

    printResults(p, n, "Shortest Job First - Preemptive (SRTF)");
}


/*
    FUNCTION: priorityNonPreemptive()
    PURPOSE:
        Priority Scheduling Non-Preemptive

    HOW IT WORKS:
        1. Choose the highest priority process (lowest number)
        2. Run it until it finishes
*/
void priorityNonPreemptive(Process p[], int n) {
    int done = 0, time = 0;

    while (done < n) {
        int idx = -1, highestPriority = INT_MAX;

        // Find highest priority process
        for (int i = 0; i < n; i++) {
            if (!p[i].finished && p[i].arrival <= time) {
                if (p[i].priority < highestPriority) {
                    highestPriority = p[i].priority;
                    idx = i;
                }
            }
        }

        // If none is ready, CPU idle
        if (idx == -1) { 
            time++; 
            continue; 
        }

        // Execute process fully
        time             += p[idx].burst;
        p[idx].completion = time;
        p[idx].turnaround = p[idx].completion - p[idx].arrival;
        p[idx].waiting    = p[idx].turnaround - p[idx].burst;
        p[idx].finished   = 1;

        done++;
    }

    printResults(p, n, "Priority Scheduling - Non-Preemptive");
}


/*
    FUNCTION: priorityPreemptive()
    PURPOSE:
        Priority Scheduling Preemptive

    HOW IT WORKS:
        1. At every time unit, choose the highest priority process
        2. Execute 1 unit
        3. Switch if a new process arrives with higher priority
*/
void priorityPreemptive(Process p[], int n) {
    int done = 0, time = 0;

    while (done < n) {
        int idx = -1, highestPriority = INT_MAX;

        // Find highest priority process available
        for (int i = 0; i < n; i++) {
            if (!p[i].finished && p[i].arrival <= time) {
                if (p[i].priority < highestPriority) {
                    highestPriority = p[i].priority;
                    idx = i;
                }
            }
        }

        // CPU idle if none ready
        if (idx == -1) { 
            time++; 
            continue; 
        }

        // Execute 1 unit
        p[idx].remaining--;
        time++;

        // If done, compute times
        if (p[idx].remaining == 0) {
            p[idx].completion = time;
            p[idx].turnaround = p[idx].completion - p[idx].arrival;
            p[idx].waiting    = p[idx].turnaround - p[idx].burst;
            p[idx].finished   = 1;
            done++;
        }
    }

    printResults(p, n, "Priority Scheduling - Preemptive");
}


/*
    FUNCTION: roundRobin()
    PURPOSE:
        Round Robin Scheduling

    HOW IT WORKS:
        1. Uses a queue
        2. Each process gets a time slice (quantum)
        3. If unfinished, it goes back to queue
*/
void roundRobin(Process p[], int n, int quantum) {
    int queue[MAX * 100];      // queue array for scheduling
    int front = 0, rear = 0;   // queue pointers
    int time = 0, done = 0;
    int inQueue[MAX] = {0};    // track which process is inside queue

    // Sort by arrival time
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (p[i].arrival > p[j].arrival) {
                Process tmp = p[i];
                p[i] = p[j];
                p[j] = tmp;
            }

    // Add first process to queue
    queue[rear++] = 0;
    inQueue[0] = 1;

    while (done < n) {

        // If queue is empty, jump to next process arrival
        if (front == rear) {
            for (int i = 0; i < n; i++) {
                if (!p[i].finished && !inQueue[i]) {
                    time = p[i].arrival;
                    queue[rear++] = i;
                    inQueue[i] = 1;
                    break;
                }
            }
        }

        int idx = queue[front++];

        // Execute either quantum or remaining time
        int execTime = (p[idx].remaining < quantum)
                        ? p[idx].remaining : quantum;

        p[idx].remaining -= execTime;
        time += execTime;

        // Add newly arrived processes to queue
        for (int i = 0; i < n; i++) {
            if (!p[i].finished && !inQueue[i] && p[i].arrival <= time) {
                queue[rear++] = i;
                inQueue[i] = 1;
            }
        }

        // If process finishes
        if (p[idx].remaining == 0) {
            p[idx].completion = time;
            p[idx].turnaround = p[idx].completion - p[idx].arrival;
            p[idx].waiting    = p[idx].turnaround - p[idx].burst;
            p[idx].finished   = 1;
            done++;
        } 
        else {
            // Put unfinished process back in queue
            queue[rear++] = idx;
        }
    }

    printResults(p, n, "Round Robin (RR)");
}


/*
    MAIN FUNCTION
    PURPOSE:
        Shows menu and allows user to choose scheduling algorithm.
        Takes process input then runs chosen algorithm.
*/
int main() {
    int choice, n, quantum;
    Process p[MAX], temp[MAX];

    do {
        printf("\n+======================================+\n");
        printf("|   CPU SCHEDULING ALGORITHMS MENU    |\n");
        printf("+======================================+\n");
        printf("|  1. First-Come, First-Serve (FCFS)  |\n");
        printf("|  2. SJF - Non-Preemptive            |\n");
        printf("|  3. SJF - Preemptive (SRTF)         |\n");
        printf("|  4. Priority - Non-Preemptive       |\n");
        printf("|  5. Priority - Preemptive           |\n");
        printf("|  6. Round Robin (RR)                |\n");
        printf("|  7. Exit                            |\n");
        printf("+======================================+\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        // Exit condition
        if (choice == 7) {
            printf("\nExiting... Goodbye!\n");
            break;
        }

        // Validate choice
        if (choice < 1 || choice > 7) {
            printf("Invalid choice. Please try again.\n");
            continue;
        }

        printf("\nEnter number of processes: ");
        scanf("%d", &n);

        // Validate process count
        if (n <= 0 || n > MAX) {
            printf("Invalid number of processes.\n");
            continue;
        }

        // Priority is needed only for Priority Scheduling algorithms
        int needPriority = (choice == 4 || choice == 5);

        // Input processes
        getInput(p, n, needPriority);

        // Copy processes so original is preserved
        copyProcesses(temp, p, n);

        // Run selected scheduling algorithm
        switch (choice) {
            case 1: fcfs(temp, n);                  break;
            case 2: sjfNonPreemptive(temp, n);      break;
            case 3: sjfPreemptive(temp, n);         break;
            case 4: priorityNonPreemptive(temp, n); break;
            case 5: priorityPreemptive(temp, n);    break;

            case 6:
                printf("Enter Time Quantum: ");
                scanf("%d", &quantum);

                if (quantum <= 0) {
                    printf("Invalid time quantum.\n");
                    break;
                }

                roundRobin(temp, n, quantum);
                break;
        }

    } while (1);

    return 0;
}