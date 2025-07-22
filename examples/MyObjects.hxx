#ifndef MyObjects_hxx
    #define MyObjects_hxx

    #include <vector>
    #include <memory>

    struct MyTrack
    {
        float px,py,pz;
    };
    
    struct MyEvent
    {
        std::string ID;
        int centrality;
        float Z;
        std::vector<std::shared_ptr<MyTrack> > tracks;
        std::string GetID() const noexcept {return ID;}
    };

    struct MyPair
    {
        float qInv,kT;
        MyPair(const std::shared_ptr<MyTrack> &trck1, const std::shared_ptr<MyTrack> &trck2)
        {
            kT = trck1->px + trck2->px + trck1->py + trck2->py;
        }
    };
    
    std::string EventHashingFunction(const std::shared_ptr<MyEvent> &event)
    {
        return std::to_string(event->centrality);
    }

    std::string PairHashingFunction(const std::shared_ptr<MyPair> &pair)
    {
        return std::to_string(static_cast<std::size_t>(pair->kT));
    }

    std::ostream& operator<<(std::ostream &os, const std::map<std::string,std::vector<std::shared_ptr<MyPair> > > &map)
    {
        // iterate over each map key, i.e. each pair group
        // here i use structured binding (only since C++17!) https://en.cppreference.com/w/cpp/language/structured_binding
        for (const auto &[key,pairVec] : map)
        {
            os << "Key: " << key << " Pairs: " << pairVec.size() << "\n";
        }

        return os;
    }

#endif