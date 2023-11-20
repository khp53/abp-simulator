#include <queue>
#include <vector>
#include <iterator>
using namespace std;

// Structure to represent an event
struct Event
{
    // enums for event types
    enum EventType{
        TIMEOUT,
        ACK,
        NIL,
        LOST,
        ERROR,
        NO_ERROR
    };
    int type;
    double time;
    int sn; // sequesnce number
    int flag; // flag to set error no_error
};

class EventScheduler
{
    
private:
    // Function to compare events based on their time
    // used for priority queue
    struct CompareTime
    {
        bool operator()(const Event &event1, Event &event2)
        {
            return event1.time > event2.time;
        }
    };
    // Priority queue to store events, ordered by their time
    priority_queue<Event, vector<Event>, CompareTime> queue;
    
public:
    // push function for events like
    // forward chnl, reverse channel etc
    void pushEvent(Event event)
    {
        queue.push(event);
    };
    // pop function to dequeue
    void popEvent()
    {
        queue.pop();
    };
    // func for storing time out event
    void storeTimeOutEvent(double time, int SN)
    {
        Event event;
        event.type = Event::TIMEOUT;
        event.time = time;
        event.sn = SN;
        event.flag = Event::NO_ERROR;
        // push the timeout to queue
        pushEvent(event);
    };
    void purgeTimeOut()
    {
        vector<Event> tempQueue;
        while (!queue.empty())
        {
            Event event = pullFromTop();
            // Exclude timeout events from the temporary event queue
            if (event.type != Event::TIMEOUT)
            {
                tempQueue.push_back(event);
            }
            popEvent();
        }
        // Restore events back to the event queue -> which not timed out
        for (std::vector<Event>::iterator iterator = tempQueue.begin(); iterator != tempQueue.end(); iterator++)
        {
            pushEvent(*iterator);
        }
    };
    
    bool isTimedOut()
    {
        vector<Event> tempQueue;
        bool result = false;

        while (!queue.empty())
        {
            Event event = pullFromTop();
            if (event.type == Event::TIMEOUT)
            {
                result = true;
            }
            tempQueue.push_back(event);
            popEvent();
        }

        // Restore events back to the event queue
        for (std::vector<Event>::iterator iterator = tempQueue.begin(); iterator != tempQueue.end(); iterator++)
        {
            pushEvent(*iterator);
        }

        return result;
    };
    Event pullFromTop()
    {
        return queue.top();
    };
};
