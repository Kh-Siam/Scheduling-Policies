#include <iostream>
#include <queue>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <stack>

using namespace std;

// The data structure for each process
struct 
Process {
    // These are the givens
	char name = 0;
    int arrival = 0;
    int service = 0;

    // These are the calculated values for each
    // process to be used in displaying the outputs
    int start = 0;
    int finish = 0;
    int turnAround = 0;
    float normTurn = 0;

    // Auxiliary variables that help with the 
    // implementation of each policy algorithm
    int temp = 0;
    int blocked = 0;
    string state = "";
};

// Stores whether the output is stats or trace
bool outputType;

// Stores the current scheduling policy used
string currentSchedulingPolicy;

// Stores the timeline given
int timeline;

// Stores the number of processes given
int numberOfProcesses;

void 
firstComeFirstServe( Process* process ) {
    currentSchedulingPolicy = "FCFS";

    queue<int> q;
    int running = -1;

    for ( int i = 0; i <= timeline; i ++ ) {
        // A loop to insert the process into the queue on its arrival
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].arrival == i )
                q.push( j );
        }

        // Run a process from the ready queue if none is running
        if ( !q.empty() && running == -1 ) {
            running = q.front();
            q.pop();
            process[running].start = i;
        }

        // On the finish of a process, run a new one if one exists in the queue
        if ( running != -1 && i == process[running].start + process[running].service ) {
            process[running].finish = i;
            process[running].turnAround = process[running].finish - process[running].arrival;
            process[running].normTurn = (float)process[running].turnAround / process[running].service;
            running = -1;
            if ( !q.empty() ) {
                running = q.front();
                q.pop();
                process[running].start = i;
            }
        }

        // A loop to keep up with the state of the process on each quantum
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].name == process[running].name )
                process[j].state += "*";
            else if ( process[j].arrival <= i && process[j].finish == 0 )
                process[j].state += ".";
            else
                process[j].state += " ";
        }
    }
}

void 
roundRobin( Process* process, int quantum ) {
    currentSchedulingPolicy = "RR-" + to_string( quantum );

    queue<int> q;
    int running = -1;

    for ( int i = 0; i <= timeline; i ++ ) {
        // A loop to insert the process into the queue on its arrival
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].arrival == i )
                q.push( j );
        }

        // Run a process from the ready queue if none is running
        if ( !q.empty() && running == -1 ) {
            running = q.front();
            q.pop();
            process[running].start = i;
        }

        // On the finish of a process, run a new one if one exists in the queue
        if ( running != -1 && process[running].temp == process[running].service ) {
            process[running].finish = i;
            process[running].turnAround = process[running].finish - process[running].arrival;
            process[running].normTurn = (float)process[running].turnAround / process[running].service;
            running = -1;
            if ( !q.empty() ) {
                running = q.front();
                q.pop();
                process[running].start = i;
            }
        }

        // If the allowed numbers of quantum for the running process is finished,
        // push it back into the ready queue and run a new process
        if ( running != -1 && quantum <= i - process[running].start && !q.empty() ) {
            q.push( running );
            running = q.front();
            q.pop();
            process[running].start = i;
        }

        // Keep an eye on the how long the running process has been running
        if ( running != -1 )
            process[running].temp ++;

        // A loop to keep up with the state of the process on each quantum
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].name == process[running].name )
                process[j].state += "*";
            else if ( process[j].arrival <= i && process[j].finish == 0 )
                process[j].state += ".";
            else
                process[j].state += " ";
        }
    }
}

void
shortestProcessNext( Process* process ) {
    currentSchedulingPolicy = "SPN";

    queue<int> q;
    queue<int> temp;
    int running = -1; 

    for ( int i = 0; i <= timeline; i ++ ) {
        // A loop to insert the process into the queue on its arrival
        // but insert the new process into its correct place depending on its 
        // service time.
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].arrival == i && q.empty() )
                q.push( j );
            else if ( process[j].arrival == i ) {
                while ( process[q.front()].service <= process[j].service && !q.empty() ) {
                    temp.push( q.front() );
                    q.pop();
                }
                temp.push( j );
                while ( !q.empty() ) {
                    temp.push( q.front() );
                    q.pop();
                }
                q.swap( temp );
            }
        }

        // Run a process from the ready queue if none is running
        if ( !q.empty() && running == -1 ) {
            running = q.front();
            q.pop();
            process[running].start = i;
        }

        // On the finish of a process, run a new one if one exists in the queue
        if ( i == process[running].start + process[running].service ) {
            process[running].finish = i;
            process[running].turnAround = process[running].finish - process[running].arrival;
            process[running].normTurn = (float)process[running].turnAround / process[running].service;
            running = -1;
            if ( !q.empty() ) {
                running = q.front();
                q.pop();
                process[running].start = i;
            }
        }

        // A loop to keep up with the state of the process on each quantum
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].name == process[running].name )
                process[j].state += "*";
            else if ( process[j].arrival <= i && process[j].finish == 0 )
                process[j].state += ".";
            else
                process[j].state += " ";
        }
    }
}

void
shortestRemainingTime( Process* process ) {
    currentSchedulingPolicy = "SRT";

    queue<int> q;
    queue<int> temp;
    int running = -1; 

    for ( int i = 0; i <= timeline; i ++ ) {
        // A loop to insert the process into the queue on its arrival
        // but insert the new process into its correct place depending on its 
        // remaining time. Also, all the other processes that were already in the queue
        // are updated to their right place as their remaining time changes.
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].arrival == i && q.empty() )
                q.push( j );
            else if ( process[j].arrival == i ) {
                while ( process[q.front()].service - process[q.front()].temp <= process[j].service - process[j].temp && !q.empty() ) {
                    temp.push( q.front() );
                    q.pop();
                }
                temp.push( j );
                while ( !q.empty() ) {
                    temp.push( q.front() );
                    q.pop();
                }
                q.swap( temp );
            }
        }

        // Run a process from the ready queue if none is running
        if ( !q.empty() && running == -1 ) {
            running = q.front();
            q.pop();
            process[running].start = i;
        }

        // On the finish of a process, run a new one if one exists in the queue
        if ( process[running].temp == process[running].service ) {
            process[running].finish = i;
            process[running].turnAround = process[running].finish - process[running].arrival;
            process[running].normTurn = (float)process[running].turnAround / process[running].service;
            running = -1;
            if ( !q.empty() ) {
                running = q.front();
                q.pop();
                process[running].start = i;
            }
        }

        // Sort the queue again to make sure that the process running is indeed 
        // the one with the shortest remaining time.
        if ( running != -1 && !q.empty() ) {
            while ( process[q.front()].service - process[q.front()].temp < process[running].service - process[running].temp && !q.empty() ) {
                temp.push( q.front() );
                q.pop();
            }
            temp.push( running );
            while ( !q.empty() ) {
                temp.push( q.front() );
                q.pop();
            }
            q.swap( temp );

            running = q.front();
            q.pop();
            process[running].start = i;
        }

        // Keep an eye on the how long the running process has been running
        if ( running != -1 )
            process[running].temp ++;

        // A loop to keep up with the state of the process on each quantum
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].name == process[running].name )
                process[j].state += "*";
            else if ( process[j].arrival <= i && process[j].finish == 0 )
                process[j].state += ".";
            else
                process[j].state += " ";
        }
    }
}

void
highestResponseRatioNext( Process* process ) {
    currentSchedulingPolicy = "HRRN";

    queue<int> q;
    stack<int> temp;
    stack<int> s;
    
    int running = -1; 

    for ( int i = 0; i <= timeline; i ++ ) {
        // A loop to insert the process into the queue on its arrival
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].arrival == i )
                q.push( j );
        }

        // Update the waiting time of each process in the ready queue
        if ( i != 0 ) {
            for ( int j = 0; j < numberOfProcesses; j ++ ) {
                if ( process[j].arrival <= i && process[j].finish == 0 ) 
                    process[j].temp ++;
            }
        }

        // An algorithm using two stacks to sort the ready queue based on 
        // each process' ratio
        float ratio1, ratio2;
        while ( !q.empty() ) {
            if ( s.empty() ) {
                s.push( q.front() );
                q.pop();
            } 
            else {
                ratio1 = (float)(process[s.top()].service + process[s.top()].temp) / process[s.top()].service;
                ratio2 = (float)(process[q.front()].service + process[q.front()].temp) / process[q.front()].service;            
                if ( ratio2 > ratio1 ) {
                    s.push( q.front() );
                    q.pop();
                }
                else {
                    while ( !s.empty() && ratio2 <= ratio1 ) {
                        temp.push( s.top() );
                        s.pop();
                        if ( !s.empty() )
                            ratio1 = (float)(process[s.top()].service + process[s.top()].temp) / process[s.top()].service;
                    }
                    s.push( q.front() );
                    q.pop();
                    while ( !temp.empty() ) {
                        s.push( temp.top() );
                        temp.pop();
                    }
                }
            }
        }
        while ( !s.empty() ) {
            q.push( s.top() );
            s.pop();
        }
        
        // Run a process from the ready queue if none is running
        if ( running == -1 && !q.empty() ) {
            running = q.front();
            q.pop();
            process[running].start = i;
        }

        // On the finish of a process, run a new one if one exists in the queue
        if ( running != -1 && i == process[running].start + process[running].service ) {
            process[running].finish = i;
            process[running].turnAround = process[running].finish - process[running].arrival;
            process[running].normTurn = (float)process[running].turnAround / process[running].service;
            running = -1;
            
            if ( !q.empty() ) {
                running = q.front();
                q.pop();
                process[running].start = i;
            }
        }

        // A loop to keep up with the state of the process on each quantum
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].name == process[running].name )
                process[j].state += "*";
            else if ( process[j].arrival <= i && process[j].finish == 0 )
                process[j].state += ".";
            else
                process[j].state += " ";
        }
    }
}

void
feedback1( Process* process ) {
    currentSchedulingPolicy = "FB-1";

    queue<int> q[timeline + 1];
    int running = -1;
    int quantum = 1;
    int numberOfLevels = 0;

    for ( int i = 0; i <= timeline; i ++ ) {
        // A loop to insert the process into the queue on its arrival
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].arrival == i ) 
                q[0].push ( j );
        }

        // Run a process from the ready queue if none is running
        // Note that multiple levels of queues exist and we 
        // search from the lowest level with the highest 
        // priority going up.
        if ( running == -1 ) {
            for ( int j = 0; j <= numberOfLevels; j ++ ) {
                if ( !q[j].empty() ) {
                    running = q[j].front();
                    q[j].pop();
                    process[running].start = i; 
                    break;
                }
            }
        }

        // On the finish of a process, run a new one if one exists in the queue 
        // with highest priority
        if ( running != -1 && process[running].temp == process[running].service ) {
            process[running].finish = i;
            process[running].turnAround = process[running].finish - process[running].arrival;
            process[running].normTurn = (float)process[running].turnAround / process[running].service;
            running = -1;
            for ( int j = 0; j <= numberOfLevels; j ++ ) {
                if ( !q[j].empty() ) {
                    running = q[j].front();
                    q[j].pop();
                    process[running].start = i; 
                    break;
                }
            }
        }
        
        // If the allowed numbers of quantum for the running process is finished,
        // push it back into the ready right queue that is needed for its level
        // and run a new process
        if ( running != -1 && quantum <= i - process[running].start ) {
            for ( int j = 0; j <= numberOfLevels; j ++ ) {
                if ( !q[j].empty() ) {
                    process[running].blocked ++;
                    if ( process[running].blocked > numberOfLevels )
                        numberOfLevels ++;
                    q[process[running].blocked].push( running );
                    running = q[j].front();
                    q[j].pop();
                    process[running].start = i; 
                    break;
                }
            }
        }

        // Keep an eye on the how long the running process has been running
        if ( running != -1 )
            process[running].temp ++;

        // A loop to keep up with the state of the process on each quantum    
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].name == process[running].name )
                process[j].state += "*";
            else if ( process[j].arrival <= i && process[j].finish == 0 )
                process[j].state += ".";
            else
                process[j].state += " ";
        }
    }
}

void
feedback2( Process* process ) {
    currentSchedulingPolicy = "FB-2i";

    queue<int> q[timeline + 1];
    int running = -1;
    int quantum;
    int numberOfLevels = 0;

    for ( int i = 0; i <= timeline; i ++ ) {
        // A loop to insert the process into the queue on its arrival
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].arrival == i ) 
                q[0].push ( j );
        }

        // Run a process from the ready queue if none is running
        // Note that multiple levels of queues exist and we 
        // search from the lowest level with the highest 
        // priority going up.
        if ( running == -1 ) {
            for ( int j = 0; j <= numberOfLevels; j ++ ) {
                if ( !q[j].empty() ) {
                    running = q[j].front();
                    q[j].pop();
                    process[running].start = i; 
                    break;
                }
            }
        }

        // On the finish of a process, run a new one if one exists in the queue 
        // with highest priority
        if ( running != -1 && process[running].temp == process[running].service ) {
            process[running].finish = i;
            process[running].turnAround = process[running].finish - process[running].arrival;
            process[running].normTurn = (float)process[running].turnAround / process[running].service;
            running = -1;
            for ( int j = 0; j <= numberOfLevels; j ++ ) {
                if ( !q[j].empty() ) {
                    running = q[j].front();
                    q[j].pop();
                    process[running].start = i;
                    break;
                }
            }
        }

        // The quantum's value is dependant on the level of the process running
        if ( running != -1 ) {
            quantum = pow( 2, process[running].blocked );
        }

        // If the allowed numbers of quantum for the running process is finished,
        // push it back into the ready right queue that is needed for its level
        // and run a new process
        if ( running != -1 && quantum <= i - process[running].start ) {
            for ( int j = 0; j <= numberOfLevels; j ++ ) {
                if ( !q[j].empty() ) {
                    process[running].blocked ++;
                    if ( process[running].blocked > numberOfLevels )
                        numberOfLevels ++;
                    q[process[running].blocked].push( running );
                    running = q[j].front();
                    q[j].pop();
                    process[running].start = i;
                    break;
                }
            }
        }

        // Keep an eye on the how long the running process has been running
        if ( running != -1 )
            process[running].temp ++;

        // A loop to keep up with the state of the process on each quantum     
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].name == process[running].name )
                process[j].state += "*";
            else if ( process[j].arrival <= i && process[j].finish == 0 )
                process[j].state += ".";
            else
                process[j].state += " ";
        }
    }
}

void
aging( Process* process, int quantum ) {
    currentSchedulingPolicy = "Aging";

    queue<int> q;
    stack<int> temp;
    stack<int> s;
    int running = -1;

    for ( int i = 0; i <= timeline; i ++ ) {
        // If there is a process running, make its current priority = its initial priority
        if ( running != -1 ) 
            process[running].temp = process[running].service;


        // Insert newly arrived processes into ready queue
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].arrival == i ) {
                q.push( j );
                process[j].temp = process[j].service;
            }
        }

        // Increment all the priorities of processes waiting in the ready queue
        for ( int j = 0; j < numberOfProcesses; j++ ) {
            if ( running != j && process[j].arrival <= i )
                process[j].temp ++;
        }

        // Check for the quantum of the running process and if it has reached the 
        // required quantum, block the process and push it into the ready queue
        if ( running != -1 && quantum <= i - process[running].start && !q.empty() ) {
            process[running].finish = i;
            process[running].turnAround = process[running].finish - process[running].arrival;
            process[running].normTurn = (float)process[running].turnAround / process[running].service;

            q.push( running );
            running = -1;
        }

        // Sort the ready queue based on the current priority of each process
        // This algorithm uses two stacks to help sort the queue.
        while ( !q.empty() ) {
            if ( s.empty() ) {
                s.push( q.front() );
                q.pop();
            } 
            else {           
                if ( process[q.front()].temp > process[s.top()].temp ) {
                    s.push( q.front() );
                    q.pop();
                }
                else {
                    while ( !s.empty() && process[q.front()].temp <= process[s.top()].temp ) {
                        temp.push( s.top() );
                        s.pop();
                    }
                    s.push( q.front() );
                    q.pop();
                    while ( !temp.empty() ) {
                        s.push( temp.top() );
                        temp.pop();
                    }
                }
            }
        }
        while ( !s.empty() ) {
            q.push( s.top() );
            s.pop();
        }


        // If no process is running, run the process with the highest queue 
        // in the ready queue
        if ( !q.empty() && running == -1 ) {
            running = q.front();
            q.pop();
            process[running].start = i;
        }

        // A loop to keep up with the state of the process on each quantum     
        for ( int j = 0; j < numberOfProcesses; j ++ ) {
            if ( process[j].name == process[running].name ) 
                process[j].state += "*";
            else if ( process[j].arrival <= i ) 
                process[j].state += ".";
            else
                process[j].state += " ";
        }

    }
}

void 
writeOutputTrace( Process* process ) {
    int temp = 0;

    // First line in output
    cout << setw( 6 ) << left << currentSchedulingPolicy;
    for ( int i = 0; i <= timeline; i ++ ) {
        temp = i % 10;
        cout << setw( 2 ) << left << temp;
    }
    cout << endl;

    // The drawn line that splits between the header and the table
    string drawLine = "--------";
    for ( int i = 0; i < timeline; i++ ) {
        drawLine += "--";
    }
    cout << drawLine << endl;

    // A loop to print the required output of all the processes
    for ( int i = 0; i < numberOfProcesses; i ++ ) {
        cout << setw( 6 ) << left << process[i].name;
        cout << "|";
        for ( int j = 0; j < timeline; j ++ ) {
            cout << process[i].state[j] << "|";
        }
        cout << " " << endl;
    }

    // Finish with the drawn line
    cout << drawLine << endl;
}

void
writeOutputStats( Process* process ) {
    float temp = 0;

    // First line: name of used policy
    cout << currentSchedulingPolicy << endl;
    
    // Second line: process names
    cout << setw( 11 ) << left << "Process";
    cout << "|";
    for ( int i = 0; i < numberOfProcesses; i++ ) {
        cout << setw( 3 ) << right << process[i].name;
        cout << setw( 3 ) << right << "|";
    }
    cout << endl;

    // Third line: arrival values of each process
    cout << setw( 11 ) << left << "Arrival";
    cout << "|";
    for ( int i = 0; i < numberOfProcesses; i++ ) {
        cout << setw( 3 ) << right << process[i].arrival;
        cout << setw( 3 ) << right << "|";
    }
    cout << endl;

    // Fourth line: service time value of each process
    cout << setw( 11 ) << left << "Service";
    cout << "|";
    for ( int i = 0; i < numberOfProcesses; i++ ) {
        cout << setw( 3 ) << right << process[i].service;
        cout << setw( 3 ) << right << "|";
    }
    cout << " Mean|" << endl;

    // Fifth line: the finish time of each process
    cout << setw( 11 ) << left << "Finish";
    cout << "|";
    for ( int i = 0; i < numberOfProcesses; i++ ) {
        cout << setw( 3 ) << right << process[i].finish;
        cout << setw( 3 ) << right << "|";
    }
    cout << "-----|" << endl;

    // Sixth line: the turnaround values of each process
    cout << setw( 11 ) << left << "Turnaround";
    cout << "|";
    for ( int i = 0; i < numberOfProcesses; i++ ) {
        temp += process[i].turnAround;
        cout << fixed;
        cout << setprecision(2);
        cout << setw( 3 ) << right << process[i].turnAround;
        cout << setw( 3 ) << right << "|";
    }
    cout << fixed;
    cout << setprecision(2);
    // calculate and print the mean
    cout << setw( 5 ) << right << temp / numberOfProcesses ;
    cout << "|" << endl;

    temp = 0;

    // Seventh line: the normturn of each value
    cout << setw( 11 ) << left << "NormTurn";
    cout << "|";
    for ( int i = 0; i < numberOfProcesses; i++ ) {
        temp += process[i].normTurn;
        cout << fixed;
        cout << setprecision(2);
        cout << setw( 5 )  << right << process[i].normTurn;
        cout << "|";
    }
    cout << fixed;
    cout << setprecision(2);
    // calculate and print the mean
    cout << setw( 5 ) << right << temp / numberOfProcesses ;
    cout << "|" << endl;
}

void
clear( Process* process ) {
    // A loop to clear all the values of each process
    // exxcept for the ones that were a given.
    for ( int i = 0; i < numberOfProcesses; i ++ ) {
        process[i].start = 0;
        process[i].finish = 0;
        process[i].turnAround = 0;
        process[i].normTurn = 0;

        process[i].temp = 0;
        process[i].blocked = 0;
        process[i].state = "";
    }
}

void 
execute( string* policy, Process* process, int numberOfPolicies ) {
    // A loop to run through multiple policies
    // and run each accordingly
    for ( int i = 0; i < numberOfPolicies; i++ ){
        if ( policy[i][0] == '1' )
            firstComeFirstServe( process );
        else if ( policy[i][0] == '2' )
            roundRobin( process,  (int)policy[i][2] - 48 );
        else if ( policy[i][0] == '3' )
            shortestProcessNext( process );
        else if ( policy[i][0] == '4' )
            shortestRemainingTime( process );
        else if ( policy[i][0] == '5' )
            highestResponseRatioNext( process );
        else if ( policy[i][0] == '6' )
            feedback1( process );
        else if ( policy[i][0] == '7' )
            feedback2( process );
        else if ( policy[i][0] == '8' )
            aging( process, (int)policy[i][2] - 48 ); 

        // The output shape: either Trace or Stats
        outputType ? writeOutputStats( process ) : writeOutputTrace( process );

        cout << endl;

        // clear all the unnecessary values in the processes
        // to have them ready for the following scehduling policy  
        clear( process );
    }
}

void 
readInput() {
    string line;

    // Check whether it's trace or stats
    getline( cin, line );
    line.compare( "trace" ) ? outputType = 1 : outputType = 0;

    // Extract all the policies required
    getline( cin, line );
    int colons = count( line.begin(), line.end(), ',' );
    string policy[ colons + 1 ];
    for ( int i = 0; line.find( ',' ) != line.npos; i++ ) {
        int position = line.find( ',' );
        policy[i] = line.substr( 0, position );
        line.erase( 0, position + 1 );
    }
    policy[ colons ] = line;

    // Read the last instant to be used
    getline( cin, line );
    timeline = stoi( line );

    // Read number of processes
    getline( cin, line );
    numberOfProcesses = stoi( line );

    // Extract all the processes
    Process process[ numberOfProcesses ];
    for ( int i = 0; i < numberOfProcesses; i++ ) {
        getline( cin, line );
        int position = line.find( ',' );
        process[i].name = line.substr( 0, position )[ 0 ];
        line.erase( 0, position + 1 );
        position = line.find( ',' );
        process[i].arrival = stoi( line.substr( 0, position ) );
        line.erase( 0, position + 1 );
        process[i].service = stoi( line );
    }

    // Execute the algorithm according to the inputs
    execute ( policy, process, colons + 1 );
}

int 
main() {
    readInput();
}