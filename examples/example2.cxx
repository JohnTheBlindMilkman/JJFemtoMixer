#include "../JJFemtoMixer.hxx"

#include "MyObjects.hxx"

void example2()
{
    // create mixer with default settings
    Mixing::JJFemtoMixer<MyEvent,MyTrack,MyPair> mixer;
    std::shared_ptr<MyEvent> eventPtr;
    std::shared_ptr<MyTrack> trackPtr;

    // pass my function to the mixer
    mixer.SetPairHashingFunction(PairHashingFunction);

    // print mixer setup
    mixer.PrintSettings();

    for (int event = 0; event < 20; ++event)
    {
        // init my event with some values
        eventPtr = std::shared_ptr<MyEvent>(new MyEvent{std::to_string(event),0,static_cast<float>(event*10),{}});

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
        std::cout << "================================ Event " << event << " ========================================\n";
        std::cout << "Signal:\n";
        std::cout << sign;
        std::cout << "\nBackground:\n";
        std::cout << bckg;
        std::cout << "==================================================================================\n";
    }

    // shows how much each buffer is filled
    mixer.PrintStatus();

    // write data to some output
}