#ifndef __RANDSOURCE_H
#define __RANDSOURCE_H

#include <cstdlib>
#include <vector>
#include <iostream>

class RandSource;

class RandStream
{
  public:
    RandStream() :
            seed(0)
    {}

    void initseed(unsigned int num) {
        seed=num;
    }

    unsigned int get()
    {
        return rand_r(&seed);
    }

    inline double getf()
    {
        return ((double)get() / RAND_MAX);
    }
    friend class RandSource;
  private:
    unsigned int seed;
};

/**
   @class RandSource
   @brief Source of random numbers
   @details Streams of random numbers.

   @author Haoyu Bai
   @date 1 January 2011
*/
class RandSource
{
  public:
    RandSource(long numStream, long blockSize = 100):
            numStream(numStream)
    {
        for(int i=0; i < numStream; ++i)
            sources.push_back(RandStream());
        currStream = 0;
        currNum = 0;
        initseed(std::rand());
    };

    void initseed(unsigned int seed)
    {
        for(unsigned int i=0; i<numStream; ++i) {
            sources[i].initseed(seed ^ i);
        }
    };

    void sizeStat()
    {
    }

    inline unsigned get()
    {
        return sources[currStream].get();
    };

    inline double getf()
    {
        return (double(get()) / RAND_MAX);
    }

    inline void startStream(long streamNum)
    {
        currNum = 0;
        currStream = streamNum;
    };

    inline void setStreamPos(long streamNum, long pos)
    {
        currNum = pos;
        currStream = streamNum;
    };

    inline long getStreamNum() { return currStream; };
    inline long getPosInStream() { return currNum; };

    inline RandStream& getStream(long numStream, long numPos=0)
    {
        return sources[numStream];
    }

    inline operator RandStream&()
    {
        return sources[currStream];
    }

    inline void reset()
    {
        currStream = 0;
        currNum = 0;
    };
    friend class RandStream;
  private:
    long numStream;
    long currStream;
    size_t currNum;
    std::vector<RandStream> sources;
};
#endif //  __RANDSOURCE_H
