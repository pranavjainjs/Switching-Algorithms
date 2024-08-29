# Authors
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