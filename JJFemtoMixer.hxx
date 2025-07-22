/**
 * @file JJFemtoMixer.hxx
 * @author Jędrzej Kołaś (jedrzej.kolas.dokt@pw.edu.pl)
 * @brief Class responsible for mixing and storing pairs.
 * @version 1.0
 * @date 2024-01-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef JJFemtoMixer_hxx
    #define JJFemtoMixer_hxx

    #include <vector>
    #include <deque>
    #include <map>
    #include <functional>
    #include <random>
    #include <string>
    #include <type_traits>
    #include <memory>
    #include <iostream>
    #include "JJUtils.hxx"

    namespace Mixing
    {
        template<typename Event, typename Track, typename Pair>
        class JJFemtoMixer
        {
            static_assert(std::is_class<Event>::value,"Provided event-type template parameter is not a class or a struct!");
            static_assert(std::is_class<Track>::value,"Provided track-type template parameter is not a class or a struct!");
            static_assert(std::is_class<Pair>::value,"Provided pair-type template parameter is not a class or a struct!");

            private:
                std::size_t fBufferSize;
                bool fWaitForBuffer,fEventHashingFunctionIsDefined,fPairHashingFunctionIsDefined,fPairCutFunctionIsDefined;
                std::map<std::string, std::deque<std::tuple<std::string, std::shared_ptr<Track>, std::shared_ptr<Track> > > > fSimilarityMap;
                std::function<std::string(const std::shared_ptr<Event> &)> fEventHashingFunction;
                std::function<std::string(const std::shared_ptr<Pair> &)> fPairHashingFunction;
                std::function<bool(const std::shared_ptr<Pair> &)> fPairCutFunction;
                /**
                 * @brief Create pairs of identical particles from given tracks
                 * 
                 * @param tracks tracks vector
                 * @return std::vector<Pair> vector of pairs
                 */
                [[nodiscard]] std::vector<std::shared_ptr<Pair> > MakePairs(const std::vector<std::shared_ptr<Track> > &tracks1, const std::vector<std::shared_ptr<Track> > &tracks2);
                /**
                 * @brief Divide pairs into corresponding category (given by the pair hash)
                 * 
                 * @param pairs pairs vector
                 * @return std::map<std::string, std::vector<Pair> > map of sorted vectors (each "branch"/bucket is a single group of similar pairs)
                 */
                [[nodiscard]] std::map<std::string, std::vector<std::shared_ptr<Pair> > > SortPairs(const std::vector<std::shared_ptr<Pair> > &pairs);
                /**
                 * @brief 
                 * 
                 * @tparam Event 
                 * @tparam Track 
                 * @tparam Pair 
                 * @param event 
                 * @param tracks 
                 * @return std::map<std::string, std::vector<std::shared_ptr<Pair> > > 
                 */
                std::map<std::string, std::vector<std::shared_ptr<Pair> > > AddEventImpl(const std::shared_ptr<Event> &event, const std::vector<std::shared_ptr<Track> > &tracks1, const std::vector<std::shared_ptr<Track> > &tracks2);

            public:
                /**
                 * @brief Default constructor. Create mixer object with: buffer size 10, don't wait for full buffer, no hashing in event, track or pair, and no pair cut
                 * 
                 */
                constexpr JJFemtoMixer() : fBufferSize(10), 
                                fWaitForBuffer(false), 
                                fEventHashingFunctionIsDefined(false),
                                fPairHashingFunctionIsDefined(false),
                                fPairCutFunctionIsDefined(false),
                                fEventHashingFunction([](const std::shared_ptr<Event> &){return "0";}),
                                fPairHashingFunction([](const std::shared_ptr<Pair> &){return "0";}),
                                fPairCutFunction([](const std::shared_ptr<Pair> &){return false;}) {}

                /**
                 * @brief Set the Event Hashing Function object.
                 * 
                 * @param func Function object, can be lambda, standard function or std::function object.
                 */
                constexpr void SetEventHashingFunction(const std::function<std::string(const std::shared_ptr<Event> &)> &func) noexcept {fEventHashingFunction = func; fEventHashingFunctionIsDefined = true;}
                /**
                 * @brief Set the Event Hashing Function object.
                 * 
                 * @param func Function object, can be lambda, standard function or std::function object.
                 */
                constexpr void SetEventHashingFunction(std::function<std::string(const std::shared_ptr<Event> &)> &&func) noexcept {fEventHashingFunction = std::move(func);  fEventHashingFunctionIsDefined = true;}
                /**
                 * @brief Get the corresponding hash for given Event object.
                 * 
                 * @param obj Event-type object
                 * @return std::string 
                 */
                [[nodiscard]] std::string GetEventHash(const std::shared_ptr<Event> &obj) const noexcept {return fEventHashingFunction(obj);}
                /**
                 * @brief Set the Pair Hashing Function object.
                 * 
                 * @param func Function object, can be lambda, standard function or std::function object.
                 */
                constexpr void SetPairHashingFunction(const std::function<std::string(const std::shared_ptr<Pair> &)> &func) noexcept {fPairHashingFunction = func; fPairHashingFunctionIsDefined = true;}
                /**
                 * @brief Set the Pair Hashing Function object.
                 * 
                 * @param func Function object, can be lambda, standard function or std::function object.
                 */
                constexpr void SetPairHashingFunction(std::function<std::string(const std::shared_ptr<Pair> &)> &&func) noexcept {fPairHashingFunction = std::move(func); fPairHashingFunctionIsDefined = true;}
                /**
                 * @brief Get the corresponding hash for given Pair object.
                 * 
                 * @param obj Pair-type object.
                 * @return std::string 
                 */
                [[nodiscard]] std::string GetPairHash(const std::shared_ptr<Pair> &obj) const noexcept {return fPairHashingFunction(obj);}
                /**
                 * @brief Set the Pair Cutting Function object. The function should return true if pair should be rejected and false if accepted.
                 * 
                 * @param func Function object, can be lambda, standard function or std::function object.
                 */
                constexpr void SetPairCuttingFunction(const std::function<bool(const std::shared_ptr<Pair> &)> &func) noexcept {fPairCutFunction = func; fPairCutFunctionIsDefined = true;}
                /**
                 * @brief Set the Pair Cutting Function object. The function should return true if pair should be rejected and false if accepted.
                 * 
                 * @param func Function object, can be lambda, standard function or std::function object.
                 */
                constexpr void SetPairCuttingFunction(std::function<bool(const std::shared_ptr<Pair> &)> &&func) noexcept {fPairCutFunction = std::move(func); fPairCutFunctionIsDefined = true;}
                /**
                 * @brief Get the result of pair selection function for a given Pair object.
                 * 
                 * @param obj Pair-type object.
                 * @return true Pair is rejected.
                 * @return false Pair is not rejected.
                 */
                [[nodiscard]] constexpr bool GetPairCutResult(const std::shared_ptr<Pair> &obj) const noexcept {return fPairCutFunction(obj);}
                /**
                 * @brief Set the max mixing buffer size for each "branch".
                 * 
                 * @param buffer Max buffer size.
                 */
                constexpr void SetMaxBufferSize(std::size_t buffer) noexcept {fBufferSize = buffer;}
                /**
                 * @brief Get the max mixing buffer size.
                 * 
                 * @return std::size_t Max buffer size.
                 */
                [[nodiscard]] constexpr std::size_t GetMaxBufferSize() const noexcept {return fBufferSize;}
                /**
                 * @brief Define whether mixing should occur only for "branches" with size equal (if true) / equal or less than (if flase) the max buffer size.
                 * 
                 * @param isFixed Buffer size usage flag.
                 */
                constexpr void SetFixedBuffer(bool isFixed) noexcept {fWaitForBuffer = isFixed;}
                /**
                 * @brief Get the mixing buffer flag.
                 * 
                 * @return true - Mixing with fixed size.
                 * @return false - Mixnig with any size.
                 */
                [[nodiscard]] constexpr bool GetBufferState() const noexcept {return fWaitForBuffer;};
                /**
                 * @brief Prints to the standard output information about current setup of JJFemtoMixer.
                 * 
                 */
                void PrintSettings() const;
                /**
                 * @brief Prints to the standard output information about the amounts of tracks and events currently stored in JJFemtoMixer.
                 * 
                 */
                void PrintStatus() const;
                /**
                 * @brief Add currently processed event to the mixer. Call this method when performing femtoscopic analysis of identical particles.
                 * 
                 * @param event Current event.
                 * @param tracks Tracks from the current event.
                 * @return std::map<std::string, std::vector<Pair> > Sorted pairs from provided tracks for given event.
                 */
                std::map<std::string, std::vector<std::shared_ptr<Pair> > > AddEvent(const std::shared_ptr<Event> &event, const std::vector<std::shared_ptr<Track> > &tracks);
                /**
                 * @brief Add currently processed event to the mixer. Call this method when performing femtoscopic analysis of non-identical particles.
                 * 
                 * @param event Current event.
                 * @param tracks1 Tracks from the current event of the first particle type.
                 * @param tracks2 Tracks from the current event of the second paritlce type.
                 * @return std::map<std::string, std::vector<Pair> > Sorted pairs from provided tracks for given event.
                 */
                std::map<std::string, std::vector<std::shared_ptr<Pair> > > AddEvent(const std::shared_ptr<Event> &event, const std::vector<std::shared_ptr<Track> > &tracks1, const std::vector<std::shared_ptr<Track> > &tracks2);
                /**
                 * @brief Get the sorted pairs which come from similar events, but not from this event.
                 * 
                 * @param event Current event (the event from which we don't want to get tracks).
                 * @return std::map<std::string, std::vector<Pair> > Sorted pairs from stored tracks for similar events.
                 */
                [[nodiscard]] std::map<std::string, std::vector<std::shared_ptr<Pair> > > GetSimilarPairs(const std::shared_ptr<Event> &event);
        };

        template<typename Event, typename Track, typename Pair>
        std::vector<std::shared_ptr<Pair> > JJFemtoMixer<Event,Track,Pair>::MakePairs(const std::vector<std::shared_ptr<Track> > &tracks1, const std::vector<std::shared_ptr<Track> > &tracks2)
        {
            bool reverse = false;
            const std::size_t size1 = tracks1.size();
            const std::size_t size2 = tracks2.size();

            std::vector<std::shared_ptr<Pair> > tmpVector;
            if (size1 > 0 && size2 > 0) // this is so we don't create std::size_t(-1) by accident
            {
                tmpVector.reserve(std::min(size1,size2) * (std::max(size1,size2) - 1)); // reserve the expected number of pairs
            }
            else
            {
                return {};
            }

            for (std::size_t it1 = 0; it1 < size1; ++it1)
                for (std::size_t it2 = 0; it2 < size2; ++it2)
                {
                    if (it1 != it2) // make a pair only if the two tracks are not on the diagonal (this is required by the backgound unfortunately)
                    {
                        if (reverse)
                            tmpVector.emplace_back(new Pair(tracks2.at(it2),tracks1.at(it1)));
                        else
                            tmpVector.emplace_back(new Pair(tracks1.at(it1),tracks2.at(it2)));

                        reverse = !reverse; // reverse the order of tracks every other time (get rid of the bias from the track sorter)
                    }
                }

            return tmpVector;
        }

        template<typename Event, typename Track, typename Pair>
        std::map<std::string, std::vector<std::shared_ptr<Pair> > > JJFemtoMixer<Event,Track,Pair>::SortPairs(const std::vector<std::shared_ptr<Pair> > &pairs)
        {
            std::string currentPairHash = "0";
            std::map<std::string, std::vector<std::shared_ptr<Pair> > > pairMap;

            for (const auto &pair : pairs)
            {
                if (fPairCutFunction(pair))
                {
                    currentPairHash = "bad";
                }
                else
                {
                    currentPairHash = fPairHashingFunction(pair);
                }

                // an entry for given currentPairHash may already exist so we must check if that's the case
                if (pairMap.find(currentPairHash) != pairMap.end())
                {
                    pairMap.at(currentPairHash).push_back(pair);
                }
                else
                {
                    pairMap.emplace(currentPairHash,std::vector<std::shared_ptr<Pair> >(1,pair));
                }
            }

            return pairMap;
        }

        template<typename Event, typename Track, typename Pair>
        std::map<std::string, std::vector<std::shared_ptr<Pair> > > JJFemtoMixer<Event,Track,Pair>::AddEventImpl(const std::shared_ptr<Event> &event, const std::vector<std::shared_ptr<Track> > &tracks1, const std::vector<std::shared_ptr<Track> > &tracks2)
        {
            std::string evtHash = fEventHashingFunction(event);
            std::tuple<std::string, std::shared_ptr<Track>, std::shared_ptr<Track> > trackPair{event->GetID(),*JJUtils::select_randomly(tracks1.begin(),tracks1.end()),*JJUtils::select_randomly(tracks2.begin(),tracks2.end())};

            // an entry for given evtHash may not exist, so we must check if that's the case
            if (fSimilarityMap.find(evtHash) == fSimilarityMap.end())
            {
                fSimilarityMap.emplace(evtHash,std::deque<std::tuple<std::string, std::shared_ptr<Track>, std::shared_ptr<Track> > >(1,trackPair));
            }
            else
            {
                fSimilarityMap.at(evtHash).push_back(trackPair);
                if (fSimilarityMap.at(evtHash).size() > fBufferSize)
                    fSimilarityMap.at(evtHash).pop_front(); 
            }

            return SortPairs(MakePairs(tracks1,tracks2));
        }

        template<typename Event, typename Track, typename Pair>
        void JJFemtoMixer<Event,Track,Pair>::PrintSettings() const
        {
            std::cout << "\n------=========== JJFemtoMixer Settings ===========------\n";
            std::cout << "Max Background Mixing Buffer Size: " << fBufferSize << ((fWaitForBuffer) ? " (FIXED)\n" : " (FLEXIBLE)\n");
            std::cout << "Event Hashing Function: " << ((fEventHashingFunctionIsDefined) ? " User-defined\n" : " Not set\n");
            std::cout << "Pair Hashing Function: " << ((fPairHashingFunctionIsDefined) ? " User-defined\n" : " Not set\n");
            std::cout << "Pair Rejection Function: " << ((fPairCutFunctionIsDefined) ? " User-defined\n" : " Not set\n");
            std::cout << "------==============================================------\n" << std::endl;
        }

        template<typename Event, typename Track, typename Pair>
        void JJFemtoMixer<Event,Track,Pair>::PrintStatus() const
        {
            std::cout << "\n------============ JJFemtoMixer Status ============------\n";
            std::cout << "Currently stored events:\n";
            for (const auto &[key,val] : fSimilarityMap)
                std::cout << val.size() << "/" << fBufferSize << "\t event hash: " << key << "\n";
            std::cout << "------=============================================------\n" << std::endl;
        }

        template<typename Event, typename Track, typename Pair>
        std::map<std::string, std::vector<std::shared_ptr<Pair> > > JJFemtoMixer<Event,Track,Pair>::AddEvent(const std::shared_ptr<Event> &event, const std::vector<std::shared_ptr<Track> > &tracks)
        {
            const std::size_t halfSize = tracks.size() / 2;
            std::vector<std::shared_ptr<Track> > tracks1(tracks.begin(), tracks.begin() + halfSize);
            std::vector<std::shared_ptr<Track> > tracks2(tracks.begin() + halfSize, tracks.end()); // iterator ranges represent half open ranges [begin, end); no need to add 1 to the second begin iterator
            
            return AddEventImpl(event,tracks1,tracks2);
        }

        template<typename Event, typename Track, typename Pair>
        std::map<std::string, std::vector<std::shared_ptr<Pair> > > JJFemtoMixer<Event,Track,Pair>::AddEvent(const std::shared_ptr<Event> &event, const std::vector<std::shared_ptr<Track> > &tracks1, const std::vector<std::shared_ptr<Track> > &tracks2)
        {
            return AddEventImpl(event,tracks1,tracks2);
        }

        template<typename Event, typename Track, typename Pair>
        std::map<std::string, std::vector<std::shared_ptr<Pair> > > JJFemtoMixer<Event,Track,Pair>::GetSimilarPairs(const std::shared_ptr<Event> &event)
        {
            std::vector<std::shared_ptr<Track> > outputVec1, outputVec2;
            std::string evtHash = fEventHashingFunction(event);

            if (fSimilarityMap.at(evtHash).size() == fBufferSize || fWaitForBuffer == false)
            {
                for (const auto &[evtId,trck1,trck2] : fSimilarityMap.at(evtHash))
                {
                    if (evtId != event->GetID())
                    {
                        outputVec1.push_back(trck1);
                        outputVec2.push_back(trck2);
                    }
                }
            }

            return SortPairs(MakePairs(outputVec1,outputVec2));
        }
    } // namespace Mixing
    
#endif