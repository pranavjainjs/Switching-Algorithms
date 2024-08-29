#include <bits/stdc++.h> // Include all standard libraries
#include <windows.h>     // Include Windows-specific library
#include <fstream>       // File I/O library
#include <random>        // Random number generation

using namespace std;

fstream fout; // File stream for output

class Packet // Definition of Packet class
{
public:
    int input_port;      // Input port number
    int output_port;     // Output port number
    double arrival_time; // Arrival time of packet
    double start_time;   // Start time of packet transmission

    void printPacket() // Print packet information
    {
        cout << "\tIP #" << input_port << "  OP#" << output_port << "  Time:" << arrival_time << endl;
    }
};

// Command line argument values
int inputBufferSize = 4;    // Input buffer size
double packetgenprob = 0.5; // Packet generation probability
int maxtimeslots = 10000;   // Maximum number of time slots
int num_ports = 4;          // Number of ports

// Global queues
vector<Packet> input_queue[1005]; // Array of input queues

// Variables to calculate metrics
double TotalDelay = 0.0;             // Total delay
long double TotalSquaredDelay = 0.0; // Total squared delay
int TotalPacketsTransmitted = 0;     // Total packets transmitted

// Function to generate packets for each input port
void generatePackets(int N, double arrivalTime, mt19937 &gen, mt19937 &gen2)
{
    // Initialize random distributions
    uniform_real_distribution<double> timeOffsetDistribution(0.001, 0.01);
    uniform_real_distribution<double> probToTransmit(0.0, 1.0);
    uniform_int_distribution<int> destinationPortDistribution(0, N - 1);

    double rand_prob;
    double dest_port_prob;
    double startTime;
    Packet *p = new Packet();
    int destinationPort;

    // Generate packets for each input port
    for (int port = 0; port < N; ++port)
    {
        // Check if input buffer is full
        if (input_queue[port].size() >= inputBufferSize)
            continue;

        // Generate packet transmission probability
        rand_prob = probToTransmit(gen);
        if (rand_prob <= packetgenprob) // If packet is generated
        {
            // Generate random start time and destination port
            startTime = timeOffsetDistribution(gen);
            destinationPort = destinationPortDistribution(gen2);

            // Create packet and add to input queue
            (*p) = {port, destinationPort, arrivalTime, arrivalTime + startTime};
            // p->printPacket();
            input_queue[port].push_back((*p));
        }
    }
}

// iSLIP scheduling algorithm
void iSLIP(int n)
{
    // Initialize random number generators
    mt19937 gen(time(nullptr));
    mt19937 gen2(time(nullptr));

    // Initialize data structures for iSLIP algorithm
    vector<int> GrantPointer(n, -1);  // Output
    vector<int> AcceptPointer(n, -1); // Input
    vector<int> requesting_ports[n];  // Input ports which are requesting for grant to output port[index]
    vector<int> granted_ports[n];     // Output ports which have granted to input port[index]

    double time = 1.0;   // Initialize time
    int n_timeslots = 0; // Initialize time slot counter

    double step; // Variable to store step duration

    // Iterate over time slots
    while (true)
    {
        if (n_timeslots == maxtimeslots) // Check if maximum time slots reached
            break;

        n_timeslots++;
        time += 1;

        // Clear request and grant lists
        for (auto v : requesting_ports)
            v.clear();
        for (auto v : granted_ports)
            v.clear();

        // Generate packets for each input port
        generatePackets(n, time, gen, gen2);

        // Phase 1: Request Phase
        for (int i = 0; i < n; i++)
        {
            for (auto p : input_queue[i])
            {
                requesting_ports[p.output_port].push_back(p.input_port);
                // fout << "IP#" << p.input_port << " requests from OP#" << p.output_port << endl;
            }
        }

        // Phase 2: Grant Phase
        int grant_to_port = -1; // Stores input port to which each output port grants
        for (int i = 0; i < n; i++)
        {                                    // iterating over output ports
            if (requesting_ports[i].empty()) // if no requests
                continue;

            // Sort the list of requesting ports
            sort(requesting_ports[i].begin(), requesting_ports[i].end());

            // Find the next requesting port to grant
            auto reqd_pointer = lower_bound(requesting_ports[i].begin(), requesting_ports[i].end(), GrantPointer[i]);
            if (reqd_pointer == requesting_ports[i].end())
            {
                reqd_pointer = requesting_ports[i].begin();
            }
            grant_to_port = *(reqd_pointer);
            // fout << "op #" << i << " grants to port #" << grant_to_port << endl;
            requesting_ports[i].clear();
            granted_ports[grant_to_port].push_back(i);
        }

        // Phase 3: Accept Phase
        int accepted_port = -1;
        for (int ip = 0; ip < n; ip++)
        {                                  // iterating over input ports
            if (granted_ports[ip].empty()) // if no grants
                continue;

            // Sort the list of granted ports
            sort(granted_ports[ip].begin(), granted_ports[ip].end());

            // Find the next granted port to accept
            auto reqd_pointer = lower_bound(granted_ports[ip].begin(), granted_ports[ip].end(), AcceptPointer[ip]);
            if (reqd_pointer == granted_ports[ip].end())
                reqd_pointer = granted_ports[ip].begin();
            accepted_port = *(reqd_pointer);
            // fout << "ip #" << ip << " accepts from port #" << accepted_port << endl;

            // Imitate packet transfer
            granted_ports[ip].clear();

            // Update AcceptPointer and GrantPointer
            AcceptPointer[ip] = (accepted_port + 1) % n;
            GrantPointer[accepted_port] = (ip + 1) % n;

            // Calculate packet delay and update metrics
            for (auto it = input_queue[ip].begin(); it != input_queue[ip].end(); it++)
            {
                if ((*it).output_port == accepted_port)
                {
                    input_queue[ip].erase(it);
                    step = time + 1 - (*it).arrival_time;
                    // fout << step << endl;
                    TotalDelay += step;
                    TotalSquaredDelay += (step * step);
                    TotalPacketsTransmitted++;
                    break;
                }
            }
        }
    }

    // Calculate performance metrics
    double avglu = 0;
    avglu = TotalPacketsTransmitted / (n);

    double mean = TotalDelay / TotalPacketsTransmitted;
    double avgSquaredDelay = TotalSquaredDelay / TotalPacketsTransmitted;
    long double variance = avgSquaredDelay - mean * mean;

    // Output results
    fout << "*** ISLIP scheduing ***" << endl;
    fout << "number of input/output ports : " << num_ports << endl;
    fout << "packet generation probability : " << packetgenprob << endl;
    fout << "Avg packet delay : " << mean << endl;
    fout << "Standard deviation for packet delay : " << sqrtl(variance) << endl;
    fout << "Avg link utilization : " << avglu / n_timeslots * 100 << endl;

    cout << "*** ISLIP scheduing ***" << endl;
    cout << "number of input/output ports : " << num_ports << endl;
    cout << "packet generation probability : " << packetgenprob << endl;
    cout << "Avg packet delay : " << mean << endl;
    cout << "Standard deviation for packet delay : " << sqrtl(variance) << endl;
    cout << "Avg link utilization : " << avglu / n_timeslots * 100 << endl;
}

// Function to print error message for incorrect input format
void throwError()
{
    cout << "Input format is incorrectly given" << endl;
    cout << "Correct format is" << endl;
    cout << "./a.out -N switchportcount -B buffersize -p packetgenprob -T maxtimeslots" << endl;
    exit(1);
}

int main(int argc, char **argv)
{
    // Parse command line arguments
    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "-N")
        {
            if (i + 1 >= argc)
                throwError();
            num_ports = stoi(argv[i + 1]);
            i++;
        }
        else if (arg == "-B")
        {
            if (i + 1 >= argc)
                throwError();
            inputBufferSize = stoi(argv[i + 1]);
            i++;
        }
        else if (arg == "-p")
        {
            if (i + 1 >= argc)
                throwError();
            packetgenprob = stod(argv[i + 1]);
            i++;
        }
        else if (arg == "-T")
        {
            if (i + 1 >= argc)
                throwError();
            maxtimeslots = stoi(argv[i + 1]);
            i += 1;
        }
        else
        {
            cout << "Unknown option: " << arg << endl;
            return 1;
        }
    }

    // Open output file
    fout.open("islip.txt", ios::out);

    // Execute iSLIP scheduling algorithm
    iSLIP(num_ports);
}
