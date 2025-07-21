# JJFemtoMixer - Signal and background mixer for femtoscopic correlations

## Used Software

- Debian11
- C++17
- [Doxygen Awesome](https://github.com/jothepro/doxygen-awesome-css) by jothepro

## Usage

To use this mixer simply create the object. There are default values assigned for each of the features: mixing buffer size, event grouping, etc (all of them can be found in the documentation). Here is an example were we create a mixer using user defined classes `YourEventClass`, `YourTrackClass`, and `YourPairClass`. We set the maximal mixing buffer size to 20 and pass user defined event grouping function `YourHashingFunction`:

```c++
#include "JJFemtoMixer.hxx"

Mixing::JJFemtoMixer<YourEventClass,YourTrackClass,YourPairClass> mixer;
mixer.SetMaxBufferSize(20); \\ set the size of the buffer to hold 20 events max in each group
mixer.SetEventHashingFunction(YourHashingFunction); \\ set your grouping of events
```

The next step is to loop over all events and all tracks in each event, perform all necessary rejections and corrections.
After each event loop you have to invoke the following methods:

```c++
auto signal = mixer.AddEvent(your_event_object,your_tracks_collection);
auto backgroud = mixer.GetSimilarPairs(your_event_object);
```

The returnied objects are of the type `std::map<std::string,std::vector<std::shared_ptr<YourPairClass>>>`. It is a collection of pairs where each map key (`std::string`) correspods to each gropup of pairs defined by you. The map value (a.k.a. the bucket) corresponds to all pairs wich belong to that group.

> [!NOTE]
> The mixer works internally on `std::shared_ptr`. In order for the mixer to work, you will also need to pass to it the correct data type, e.g. `std::shared_ptr<YourEventClass>`.

You can then use the `signal` and `backgound` objects to fill your histograms. Remember to fill histograms for each group seperately. Now what is left is to add the signal and background histograms based on the analysis you want to do, divide them and voilà! You have your own correlation function!

> [!IMPORTANT]
> The key value of `"bad"` is used internally for "bad" pairs, i.e. pairs which did not pass the rejection cut (if specified). It is useful to propagate this convention to your code.

## So... How Does This Work Exaclty?

The Koonin-Pratt equation is build on certain assumptions, which will not be discussed here. The consequences of those assumptions require us to crate particle pairs wich come from the same phase-space. We can loosen this term and say that they have to come from a *similar* phase-space for the sake of statistics.

### Event Grouping

The term *similar* is not well defined and will depend on a lot of things (detector, particles analysed, person analysing the data). Therefore, it needs to be easly modified. That is where the idea of letting the user define his grouping criteria came from.

It is a form of mapping of a chosen space onto a set of values (std::string). For example, we consider discrete 2D cartesian space, such that $x \in \{0,10\}$ and $y \in \{0,10\}$. We then map every point into a 1D space such that $\xi = x*100 + y$. This give us (if we consider writing out the zeros):

```text
(0,0) -> 0000
(0,1) -> 0001
(1,0) -> 0100
(1,1) -> 0101
etc...
```

This gives us the ability to map any descrete N-dimensional space into 1D space (kind of like hashing) and use it as a form of object grouping. Now we can mix tracks from events in same centrality, event plane angle, z vertex, event-average pt or whatever you can come up with!

If the above example was too abstract consider this one.
Imagine you analyse Au+Au data in a collider detector. Your have created your own class which represents an event and contains two public members: `int centrality` where each number corresponds to a given centrality range (e.g. 1 is 0-10%, 2 is 10-20% etc.) and `float Z` which is the Z component of the reconstructed event position (lets say it is in mm). The most common way of gruping events can be based on centrality and Z vertex of the collision. After discussion with your supervisor or reading some other analyses you decide to group events based on 10% centrality increments and 1cm intervals in Z vertex, so you define the following function:

```c++
std::string MyEventHashingFunction(const std::shared_ptr<MyEventClass> &event)
{
    return std::to_string(event->centrality) + std::to_string(static_cast<int>(event->Z/10));
}
```

> [!TIP]
> An easy way to define grouping for a floating point variable is to scale it by some value and the cast onto an integer. This works well for "nice" intervals, e.g. every 10mm or every 100 MeV/c. For more complex intervals (and the ones with varying interval width) it may be better to define the boundaries of each group in some array and use a `for` loop to find the corresponding index value.

Which may return values like:

```sh
"110"
"111"
"23"
"432"
```

> [!WARNING]
> Remeber that casting int to string and concatating them might not give you the result you want. Think of the notation you wish to use, e.g. if the hash should be 4 chars, where first two are reserved for centrality and the last two for Z vertex, then `std::to_string(10) + std::to_string(1)` will give you `101` instead of `1001` you would expect.

This means that every combination of the values is assigned to a specific `std::string`. E.g. `"110"` may contain all events with centrality 0-10% (this is `1` from our example) and $Z_{vertex} \in (110,119)$ mm (the `10` from our example). Each such `std::string` value in the above list represents a single group of events within wich the mixing will occur (assuming we have any tracks).

Each hash/group has its own mixing buffer, where N last events are stored (N is the max buffer size defined by the mixer or user) with a randomly chosen track. Only the tracks from the same group are mixed together.

In general, what the numbers mean is up to you. You could use letters for all I care. The key point is that the pairs will be created only from tracks wich come from events with the same hash (group value). This means the phase-space should also be similar and the theory behind the Koonin-Pratt equation holds!

### Pair Grouping

When performing femtoscopic analysis you will often find yourself performing so called differential analysis. The most common is kT-differential analysis. Lets say you have defined your own class representaing a pair of particles `MyPairClass` which has a public memeber `float kt` representing the average transverse momentum of the pair in units of MeV/c. You've decided to group your pairs in 100 MeV/c intervals. 

The mixer allows to group pairs in similar way as events, e.g.:

```c++
std::string MyPairHashingFunction(const std::shared_ptr<MyPairClass> &pair)
{
    return std::to_string(static_cast<int>(pair->kt/100));
}
```
> [!NOTE]
> By default JJFemtoMixer will put all pairs into one group with the hash (map key) of `"0"`.

As a return from `JJFemtoMixer::AddEvent` and `JJFemtoMixer::GetSimilarPairs` you will get pairs wich are already grouped by your function. Following the above example you may get values like `"1"`, `"2"`, `"3"`, etc. This makes it easier to fill the corresponding histograms later (maybe not in this simple example, but I hope you get the point of it in a multi-differential anaysis). 

### Pair Selection

The pair grouping function can also act as a pair selection cut. Howaver, I have made a decision to create a separate function in order to distinguish selection from grouping. Adding to the last example, lets say that `MyPairClass` also has a public field holding the value of the opening angle $\theta$ between the momentum vectors of the two tracks `float openingAngle` and you want to reject all pairs where $\theta < 5^\circ$. 

Then, the pair grouping function is defined in similar way (note, here we return a `bool`), e.g.:

```c++
bool MyPairRejectionFunction(const std::shared_ptr<MyPairClass> &pair)
{
    return pair->openingAngle < 5.;
}
```

It will still create pairs, but it will assign them to group `"bad"`. I made such a design choice to let the user "see" what is being rejected.

## Documentation & Examples

Some simple examples can be found in the `examples` directory.

I used the nice looking Doxygen Awesome by jothepro. To generate the HTML documentation just type `doxygen` in the base directory of this project. You can then open index.html file in any web browser to read the documentation.

## Important Notes

There are certain criteria which your event and pair class have to meet in order for the code to compile:

### Event Class:
- Has a `GetID()` function, which returns an `std::string` to know if we are not trying to mix two of the same events.

### Pair Class:
- Has a constructor of the following signature:
    ```c++
    YourPairClass(const std::shared_ptr<YourTrackClass>&,const std::shared_ptr<YourTrackClass>&)
    ```