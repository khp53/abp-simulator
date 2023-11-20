//
//  abp_sim.cpp
//  project-2-xcode
//
//  Created by Karimul Hasan on 2023-11-17.
//

#include <iostream>
#include <stdio.h>
#include <random>

#include "event_scheduler.cpp"

// struct containing frame info
struct Frame
{
    int sequenceNumber = 0;
    int length = (1500 * 8);    // packet length in bits
    int HEADER_LENGTH = 54 * 8; // 54 bytes * 8 bits/byte = 432 bits
};

class ABPSimulator
{
private:
    // declared some of these as a global variable for ease of use
    // I know some of them were supposed be only local
    int SN = 0;
    int NEXT_EXPECTED_ACK = 1;
    int NEXT_EXPECTED_FRAME = 0;
    int C = 5000;
    int PS = 0;
    int PR = 0;
    int PN = 5000;
    int propagationDelay;
    double BER;
    double t_c = 0;
    double timeOut;
    EventScheduler es;
    
public:
    // constructor
    ABPSimulator(double delta, double ber, int two_tau)
    {
        propagationDelay = two_tau;
        BER = ber;
        timeOut = delta * two_tau;
    }
    
    // sending frame func
    Event abp_send(double time, int seqNum)
    {
        // initialize frame and event
        Event event;
        Frame frame;
        
        // calculate frame bits
        int frameLength = frame.length + frame.HEADER_LENGTH;
        
        event.time = time;
        event.sn = seqNum;
        event = SEND(event, frameLength, frame);
        
        // transmission delay
        int transmissionDelay = frameLength / C;
        int transmissionDelay2 = frame.HEADER_LENGTH / C;
        
        // Set the event time to current time plus the total round trip time of the packet
        // frame's journey from sender to reciver
        // and acknowledgement from receiver to sender
        event.time = t_c + propagationDelay + transmissionDelay + propagationDelay + transmissionDelay2;
        return event;
    };
    
    Event SEND(Event event, int frameLength, Frame frame) {
        event = channel(event, frameLength); // for forward channel sending frameLength(contains header).
        event = receiver(event); // receiver takes forward channel event
        event = channel(event, frame.HEADER_LENGTH); // for reverse channel sending only header (ACK)
        return event;
    }
    
    Event channel(Event cEvent, int fH_length)
    {
        // Calculate the number of errors in the channel
        int numErrors = hasFrameError(fH_length);
        // error logic from ques
        if (numErrors >= 5)
        {
            cEvent.flag = Event::LOST;
        }
        else if (numErrors >= 1 && numErrors <= 4)
        {
            cEvent.flag = Event::ERROR;
        }
        else
            cEvent.flag = Event::NO_ERROR;
        
        return cEvent;
    };
    
    // receiver end func
    Event receiver(Event rEvent)
    {
        // in case of no error found and seq no matches the next frame no
        if (rEvent.flag == Event::NO_ERROR)
        {
            if (rEvent.sn == NEXT_EXPECTED_FRAME)
            {
                // increament the next frame counter
                NEXT_EXPECTED_FRAME = (NEXT_EXPECTED_FRAME + 1) % 2;
                rEvent.sn = NEXT_EXPECTED_FRAME;
                rEvent.type = Event::ACK;
            } else
            {
                // don't increament just setting the type
                rEvent.type = Event::ACK;
                rEvent.sn = NEXT_EXPECTED_FRAME;
            }
        }
        // If the frame was lost then set the type for the event and do nothing
        else if (rEvent.flag == Event::LOST)
        {
            rEvent.type = Event::NIL;
        }
        // finally the same thing as before, not increamenting the no.
        else
        {
            rEvent.type = Event::ACK;
            rEvent.sn = NEXT_EXPECTED_FRAME;
        }
        
        return rEvent;
    };
    
    // Simulate frame errors
    bool hasFrameError(int frameLength)
    {
        int numErrors = 0;
        
        for (int i = 0; i < frameLength; i++)
        {
            double randomValue = generateRandomValue(); // Generate a random value between 0 and 1
            if (randomValue < BER)
            {
                numErrors++;
            }
        }
        
        return numErrors >= 1 && numErrors <= 4;
    }
    
    // generate rand num
    double generateRandomValue()
    {
        return (double)rand() / (double)RAND_MAX;
    };
    
    // the main simulation function
    // gets intregated with event sim
    double simulate()
    {
        Frame frame;
        while (PR < PN)
        {
            if (!es.isTimedOut())
            {
                // Purge old timeout
                es.purgeTimeOut();
                
                int tDelay = (frame.length + frame.HEADER_LENGTH) / C;
                
                // Register new timeout with the timeout delay and transmission delay
                int t = t_c + timeOut + tDelay;
                es.storeTimeOutEvent(t, SN);
                
                // Send packet with current time and sequence number
                Event res = abp_send(t_c, SN);
                //PS++;
                
                if (res.type != Event::NIL)
                {
                    es.pushEvent(res);
                }
            }
            // get the next event from queue
            Event nextEvent = es.pullFromTop();
            // set the current time
            t_c = nextEvent.time;
            // dequeue event
            es.popEvent();
            
            // for a successfull ack without error and if seq no matches
            if (nextEvent.type == Event::ACK && nextEvent.flag == Event::NO_ERROR && nextEvent.sn == NEXT_EXPECTED_ACK)
            {
                // increament received counter
                PR++;
                // purge old timeout for next event
                es.purgeTimeOut();
                // update seq no
                SN = (SN + 1) % 2;
                // update next expected ack
                NEXT_EXPECTED_ACK = (SN + 1) % 2;
            }
            // for error or seq no not matched just continue so after timeout can resend
            else if ((nextEvent.type == Event::ACK && nextEvent.flag == Event::ERROR) || (nextEvent.type == Event::ACK && nextEvent.sn != NEXT_EXPECTED_ACK))
            {
                continue;
            }
        }
        
        // Calculate throughput
        double throughput = (PR * frame.length) / t_c; // considered in bits/sec
        //std::cout << throughput << std::endl;
        return throughput;
    };
};
