I will assume that if you came here, you already know about CERN ROOT

The examples here can be compiled using the root interpreter, e.g. `root -q -b exampleABC.cxx++g`

## Example 1

A showcase of the use of event hashing function. Only every second event the number of pairs in background increases for each event group

## Example 2

A showcase of the pair hashing function. Each group has its own collection of pairs. Moreover, some groups are present in the signal and not in the background and vice versa. This may occur in real life and should be taken into consideration when analysing data.