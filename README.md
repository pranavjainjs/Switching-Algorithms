# CS544 : Queuing in packet switches

We have a crossbar switch with N input ports and N output ports. In every time slot, packets are generated at every input port with some generation probability and then the generated packet follows respective scheduling algorithms, and they are forwarded accordingly to an output port. Packets are buffered at input and output ports.

We have implemented and compared various packet transmission algorithms in crossbar switch and simulated traffic using C++.

## Authors
Parth Kasture 210101074
Pranav Jain 210101078

# File structure
The code folder contains three cpp files for three scheduling algorithms
    - inq.cpp, 
    - kouq.cpp, 
    - islip.cpp.

# Run the code
To run either of the scheduling algorithms, open the file in vscode and press run button or else
in terminal type the following command

    g++ inq.cpp
    ./a.out [-N switchportcount] [-B buffersize] [-p packetgenprob] [-T maxtimeslots]

## Compiling using make (Windows)
1. Compile the Source Files

Open your terminal, navigate to the project directory where the `Makefile` is located, and run:

    make

2. Clean Up Generated Files

After compilation, or if you need to clean up, run:

    make clean

# Example usage
    g++ ./kouq.cpp
    ./a.out -N 4 -B 10 -p 0.6 -T 1000
    ./a.out -N 6 -B 6 -p 0.4 -T 2000
    ./a.out

# Description
- The values inside [] are optional and can be omitted. In that case, the code will take up default values.
- switchportcount(int) is number of input and output ports
- buffersize(int) is size of buffer at input and output queue
- packetgenprob(double) is the probability with which a packet is generated at input buffer
- maxtimeslots(int) is the max number of slots for which we have to run the simulation.

# Range
We have tested the code for following range :-
    1 <= switchportcount <= 256
    1 <= buffersize <= 100
    0 <= packetgenprob <= 1
    1 <= maxtimeslots <= 10000

# Output
Output containing avg packet delay, standard deviation, avg link utilization is printed in terminal 
Logs are printed in correspoding .txt file