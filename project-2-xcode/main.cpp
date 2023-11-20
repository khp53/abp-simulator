//
//  main.cpp
//  project-2-xcode
//
//  Created by Karimul Hasan on 2023-11-17.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "abp_sim.cpp"

int main()
{
    // question 1
    double throughput1 = 0.0;
    double throughput2 = 0.0;
    double delta[] = {2.5, 5, 7.5, 10, 12.5};
    // q1
    //double BER = 0.0;
    // q2
    //double BER = 1e-5;
    // q3
    double BER = 1e-4;
    double propDelay[] = {0.01, 0.5}; // converted to sec
    
    // Loop through delta values
    for (int d = 0; d < 5; d++) // did not use sizeOf cause array length is defined
    {
        double curr_delta = delta[d];
        std::cout << "============================================"<<std::endl;
        std::cout << "current delta: " << curr_delta <<std::endl;
        std::cout << "============================================"<<std::endl;
        
        // Loop through propagation delays or two tau
        for (int i = 0; i < 2; i++)
        {
            double curr_propDelay = propDelay[i];
            // instance of ABPSIM
            ABPSimulator sim = ABPSimulator(curr_delta, BER, curr_propDelay);
            // throughput for the first 5000 packets
            throughput1 = sim.simulate();
            // std::cout << throughput1 << std::endl;
            sim = ABPSimulator(curr_delta, BER, curr_propDelay);
            // throughput for the second 5000 packets
            throughput2 = sim.simulate();
            
            std::cout << "Propagation Delay: " << curr_propDelay << " sec."<< std::endl;;
            std::cout << "Bit Error Rate: " << BER<< std::endl;
            std::cout << "Throughput 1: " << throughput1<< std::endl;
            std::cout << "Throughput 2: " << throughput2 << std::endl;
            std::cout << "=================================================="<< std::endl;
            // Reset throughput variables for next iteration
            throughput1 = 0.0;
            throughput2 = 0.0;
        }
    }
    return 0;
}


