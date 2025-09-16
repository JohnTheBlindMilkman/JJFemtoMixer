#include "../JJFemtoMixer.hxx"

#include "MyObjects.hxx"

void example1()
{
    // create mixer with default settings
    Mixing::JJFemtoMixer<MyEvent,MyTrack,MyPair> mixer;
    std::shared_ptr<MyEvent> eventPtr;
    std::shared_ptr<MyTrack> trackPtr;

    // pass my function to the mixer
    mixer.SetEventHashingFunction(EventHashingFunction);

    // print mixer setup
    mixer.PrintSettings();

    std::cout << "Event no.:\t" << "Signal pairs:\t" << "Background pairs:\n";

    for (int event = 0; event < 100; ++event)
    {
        // init my event with some values
        // important point: centrality is event mod 2 (so 0 or 1), which will give us 2 event gropus
        eventPtr = std::shared_ptr<MyEvent>(new MyEvent{std::to_string(event),event%2,static_cast<float>(event*10),{}});

        // here do some event selection and other stuff

        for (int track : {0,1,2,3,4,5})
        {
            // init my track in a similar fashion
            trackPtr = std::shared_ptr<MyTrack>(new MyTrack{static_cast<float>(track + 1),static_cast<float>(track - 1),static_cast<float>(track)});
            
            // here do track selection
            
            // if the track has passed the cuts, add it to my event
            eventPtr->tracks.push_back(trackPtr);
        }

        // mix signal and background
        auto sign = mixer.AddEvent(eventPtr,eventPtr->tracks);
        auto bckg = mixer.GetSimilarPairs(eventPtr);

        //here I print it just for visualisation purposes
        std::cout << event << "\t\t" << sign["0"].size() << "\t\t" << bckg["0"].size() << "\n";
    }

    // shows how much each buffer is filled
    mixer.PrintStatus();

    // write data to some output
}