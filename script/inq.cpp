#include <bits/stdc++.h>
#include <unistd.h>     
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
        fout << "\tIP #" << input_port << "  OP#" << output_port << "  Time:" << arrival_time << endl;
    }

    bool operator<(const Packet &p) const // Comparison operator for Packet class
    {
        return input_port == p.input_port ? arrival_time < p.arrival_time : input_port < p.input_port;
    }
};

// Command line arguments
int inputBufferSize = 4;           // Input buffer size
double packetGenerationProb = 0.5; // Packet generation probability
int maxtimeslots = 10000;          // Maximum number of time slots
int num_ports = 4;                 // Number of ports

// Global variables
double currentTime = 1.0;          // Current simulation time
multiset<Packet> generatedPackets; // Set of generated packets

// Variables to calculate metrics
double TotalDelay = 0.0;         // Total delay
double TotalSquaredDelay = 0.0;  // Total squared delay
int TotalPacketsTransmitted = 0; // Total packets transmitted

// Function to generate packets and perform INQ scheduling
void inq(int n, vector<multiset<Packet>> &ip_ports, vector<vector<Packet>> &op_ports)
{
    mt19937 gen(time(nullptr));                                            // Random number generator
    uniform_real_distribution<double> timeOffsetDistribution(0.001, 0.01); // Distribution for start time offset
    uniform_int_distribution<int> destinationPortDistribution(0, n - 1);   // Distribution for destination port

    int n_timeslots = 0; // Time slot counter
    int dp;              // Destination port
    double startTime;    // Start time of packet transmission
    double step;         // Variable to store step duration
    Packet p;            // Packet object

    // Iterate over time slots
    while (true)
    {
        if (n_timeslots == maxtimeslots) // Check if maximum time slots reached
            break;

        currentTime += 1; // Increment time
        n_timeslots++;

        // Generate packets for each input port
        for (int i = 0; i < n; i++)
        {
            if (ip_ports[i].size() >= inputBufferSize) // Check if input buffer is full
                continue;

            if (double(rand() % 100) / 100 >= packetGenerationProb) // Check packet generation probability
                continue;

            dp = destinationPortDistribution(gen);                     // Generate random destination port
            startTime = timeOffsetDistribution(gen);                   // Generate random start time offset
            p = Packet({i, dp, currentTime, currentTime + startTime}); 
            ip_ports[i].insert(p);                                     
            op_ports[p.output_port].push_back(p);                      
            // p.printPacket();                                           
            generatedPackets.insert(p);                                
        }

        // Process packets for transmission
        for (int i = 0; i < n; i++)
        {
            Packet packet;
            if (op_ports[i].size() > 0) // Check if there are packets in output port queue
            {
                int selectedIndex = rand() % op_ports[i].size(); // Select random packet for transmission
                packet = op_ports[i][selectedIndex];

                /* packet transmission happens here */
                step = currentTime + 1 - packet.arrival_time; // Calculate packet transmission duration
                TotalDelay += (step);                
                TotalSquaredDelay += step * step;             
                TotalPacketsTransmitted++;                    

                op_ports[i].erase(op_ports[i].begin() + selectedIndex); // Erase transmitted packet from output port queue

                // Erase transmitted packet from input queue
                if (generatedPackets.find(packet) != generatedPackets.end())
                    generatedPackets.erase(generatedPackets.find(packet));
                if (ip_ports[packet.input_port].find(packet) != ip_ports[packet.input_port].end())
                {
                    ip_ports[packet.input_port].erase(ip_ports[packet.input_port].find(packet));
                }
            }
        }
    }

    // Calculate performance metrics
    double avglu = (double)TotalPacketsTransmitted / (n * maxtimeslots);  // Average link utilization
    double mean = TotalDelay / TotalPacketsTransmitted;                   // Average packet delay
    double avgSquaredDelay = TotalSquaredDelay / TotalPacketsTransmitted; // Average squared delay
    long double variance = avgSquaredDelay - mean * mean;                 // Variance of packet delay

    // Output results
    fout << "*** INQ scheduling ***" << endl;
    fout << "number of input/output ports : " << num_ports << endl;
    fout << "packet generation probability : " << packetGenerationProb << endl;
    fout << "Avg link utilization is " << avglu * 100 << endl;
    fout << "Avg packet delay is " << mean << endl;
    fout << "Standard deviation for packet delay is " << sqrtl(variance) << endl;

    cout << "*** INQ scheduling ***" << endl;
    cout << "number of input/output ports : " << num_ports << endl;
    cout << "packet generation probability : " << packetGenerationProb << endl;
    cout << "Avg link utilization is " << avglu * 100 << endl;
    cout << "Avg packet delay is " << mean << endl;
    cout << "Standard deviation for packet delay is " << sqrtl(variance) << endl;
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
            packetGenerationProb = stod(argv[i + 1]);
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
    fout.open("inq-output.txt", ios::out); // Open output file

    vector<multiset<Packet>> ip_ports(num_ports, multiset<Packet>()); // Input port queues
    vector<vector<Packet>> op_ports(num_ports, vector<Packet>());     // Output port queues
    inq(num_ports, ip_ports, op_ports);                               // Perform INQ scheduling
}
