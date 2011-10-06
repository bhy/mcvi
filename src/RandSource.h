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
            seed(0), pos(0)
    {}

    void initseed(unsigned int num) {
        #pragma omp critical
        {
            seed=num;
            pos = 0;
        }
    }

    unsigned int get()
    {
        unsigned int result;
        #pragma omp critical
        {
            result = rand_r(&seed);
            ++pos;
        }
        return result;
    }

    inline double getf()
    {
        return ((double)get() / RAND_MAX);
    }
    friend class RandSource;

    long pos;
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
        seeds.resize(numStream);
        for(int i=0; i < numStream; ++i)
            sources.push_back(RandStream());
        currStream = 0;
        currNum = 0;
        initseed(std::rand());
    };

    void initseed(unsigned int seed)
    {
        #pragma omp parallel for
        for(unsigned int i=0; i<numStream; ++i) {
            seeds[i] = (seed ^ i);
            sources[i].initseed(seeds[i]);
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
        #pragma omp critical
        {
            currNum = 0;
            currStream = streamNum;
        }
    };

    inline void setStreamPos(long streamNum, long pos)
    {
        #pragma omp critical
        {
            currNum = pos;
            currStream = streamNum;
        }
    };

    inline long getStreamNum() { return currStream; };
    inline long getPosInStream() { return currNum; };

    inline RandStream& getStream(long numStream, long numPos)
    {
        sources[numStream].initseed(seeds[numStream]);
        for (long i = 0; i < numPos; i++)
            sources[numStream].get();
        return sources[numStream];
    }

    inline RandStream& getStream(long numStream)
    {
        return sources[numStream];
    }


    inline operator RandStream&()
    {
        return sources[currStream];
    }

    inline void reset()
    {
        #pragma omp critical
        {
            currStream = 0;
            currNum = 0;
        }
    };
    friend class RandStream;
  private:
    long numStream;
    long currStream;
    size_t currNum;
    std::vector<RandStream> sources;
    std::vector<unsigned> seeds;
};
#endif //  __RANDSOURCE_H
