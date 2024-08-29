#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <random>
#include <fstream>

std::fstream fout;

int bufferSize = 4;       // buffer size for input and output queue
long long totaldelay = 0; // total delay for all packets
long long squaredDelay = 0;
double packetSent = 0;  // total number of packet sents in maximum simulation time
int maxSimTime = 10000; // maximum simulation time
int dropped = 0;        // number of times k drop happened

using namespace std;

void throwError()
{
    cout << "Input format is incorrectly given" << endl;
    cout << "Correct format is" << endl;
    cout << "./a.out -N switchportcount -B buffersize -p packetgenprob -T maxtimeslots" << endl;
    exit(1);
}

// function to generate packets for each input port
void generatePackets(int N, double packetgenprob, vector<queue<pair<double, int>>> &inputQueues)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> probDistribution(0.0, 1.0);
    uniform_real_distribution<double> timeOffsetDistribution(0.001, 0.01);

    for (int i = 0; i < 10000; i++)
    {
        for (int port = 0; port < N; ++port)
        {
            // check if the input port generates a packet based on probability
            if (probDistribution(gen) < packetgenprob)
            {

                int destinationPort = rand() % N;               // generate random destination port
                double startTime = timeOffsetDistribution(gen); // generating random offset

                double arrivalTime = 0.0;
                inputQueues[port].push({arrivalTime + i, destinationPort});
                // std::cout << "Packet generated at port " << port << " to port " << destinationPort
                //         << " at time " << startTime +i<< std::endl;
            }
        }
    }
}

vector<queue<pair<double, int>>> KOUQ_Scheduler(int K, vector<queue<pair<double, int>>> &inputQueues, int N, double packetgenprob)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> probDistribution(0.0, 1.0);
    uniform_real_distribution<double> timeOffsetDistribution(0.001, 0.01);

    vector<queue<pair<double, int>>> inp = inputQueues;                          // creating input queue queue for input queue
    vector<vector<pair<double, int>>> out(N, vector<pair<double, int>>());       // temporary vector for processing packets of one slot
    vector<queue<pair<double, int>>> outputQueue(N, queue<pair<double, int>>()); // output queue to store packets of all slots

    // running for simulation time considering one time slot to be of 1sec
    for (int i = 0; i < maxSimTime; i++)
    {
        for (int port = 0; port < N; port++)
        {

            // form input queue checking whether the packet is according to current time slot
            if (!inp[port].empty() && inp[port].front().first == i)
            {
                out[inp[port].front().second].push_back({i, port});
                inp[port].pop();
            }
        }

        for (int port = 0; port < N; port++)
        {
            // if number of packets destined for an output port is greater than K then we drop rest of packets
            if (out[port].size() > K)
            {
                dropped++;
            }

            // keeping only k random packets and dropping rest of the packets
            while (out[port].size() > K)
            {

                int randomIndex = rand() % out[port].size(); // choosing a random index to delete
                auto it = out[port].begin();
                advance(it, randomIndex); // processing to that index
                out[port].erase(it);      // deleting the selected index
            }

            // if current size of output buffer is less than max buffer size then we insert the packet in that output buffer or else we drop the packet
            for (int j = 0; j < out[port].size(); j++)
            {
                if (outputQueue[port].size() < bufferSize)
                {
                    outputQueue[port].push({out[port][j].first, out[port][j].second});
                }
            }
            // calculating the delay for the packet which is exiting the output port
            if (outputQueue[port].size() > 0)
            {
                packetSent++;
                totaldelay += (i + 1 - outputQueue[port].front().first);
                squaredDelay += (i + 1 - outputQueue[port].front().first) * (i + 1 - outputQueue[port].front().first);
                outputQueue[port].pop();
            }

            out[port].clear();
        }
    }
    return outputQueue;
}

int main(int argc, char **argv)
{

    fout.open("kouq.txt", std::ios::out);

    int N = 8;                  // number of input/output ports
    double packetgenprob = 0.5; // packet generation probability
    int K = 0.6 * N;            // K for KOUQ algorithm
    bufferSize = 4;             // buffer size for input and output queues

    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];
        if (arg == "-N")
        {
            if (i + 1 >= argc)
                throwError();
            N = stoi(argv[i + 1]);
            i++;
        }
        else if (arg == "-B")
        {
            if (i + 1 >= argc)
                throwError();
            bufferSize = stoi(argv[i + 1]);
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
            maxSimTime = stoi(argv[i + 1]);
            i += 1;
        }
        else
        {
            cout << "Unknown option: " << arg << endl;
            return 1;
        }
    }

    // create input queues for each port
    vector<queue<pair<double, int>>> inputQueues(N);

    // cout<<"Enter 0 if you want default values or else if you want your own values type 1"<<endl;
    // int n;
    // cin>>n;
    // if(n==1){
    //     cout<<"Enter number of input/output ports"<<endl;
    //     cin>>N;
    //     cout<<"Enter packet generation probability"<<endl;
    //     cin>>packetgenprob;
    //     cout<<"Enter buffer size"<<endl;
    //     cin>>bufferSize;
    // }
    K = 0.6 * N;
    // traffic generation
    generatePackets(N, packetgenprob, inputQueues);

    // implementing KOUQ scheduler
    vector<queue<pair<double, int>>> output = KOUQ_Scheduler(K, inputQueues, N, packetgenprob);

    cout << "*** KOUQ scheduling ***" << endl;
    cout << "N=" << N << " P=" << packetgenprob << " B=" << bufferSize << " ." << endl;
    cout << "Average packet delay : " << totaldelay / (double)(packetSent) << endl;
    cout << "Link utilization : " << packetSent / (double)(N * maxSimTime) << endl;
    cout << "KDrop probabiltiy : " << (double)dropped / (double)(N * maxSimTime) << endl;
    cout << "Standard Deviation of pkt delay : " << sqrt((double)squaredDelay / (double)packetSent - (totaldelay / (double)(packetSent)) * (totaldelay / (double)(packetSent))) << endl;

    fout << "*** KOUQ scheduling ***" << endl;
    fout << "N=" << N << " P=" << packetgenprob << " B=" << bufferSize << " ." << endl;
    fout << "Average packet delay : " << totaldelay / (double)(packetSent) << endl;
    fout << "Link utilization : " << packetSent / (double)(N * maxSimTime) << endl;
    fout << "KDrop probabiltiy : " << (double)dropped / (double)(N * maxSimTime) << endl;
    fout << "Standard Deviation of pkt delay : " << sqrt((double)squaredDelay / (double)packetSent - (totaldelay / (double)(packetSent)) * (totaldelay / (double)(packetSent))) << endl;

    return 0;
}