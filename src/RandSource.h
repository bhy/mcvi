#ifndef __RANDSOURCE_H
#define __RANDSOURCE_H

#include <cstdlib>
#include <vector>

class RandSource;

class RandStream
{
public:
    RandStream(RandSource& randSource, long currStream, size_t currPos, bool sync=false):
        randSource(randSource), currStream(currStream), currPos(currPos), sync(sync)
    {}

    inline unsigned get();

    inline double getf()
    {
        return ((double)get() / RAND_MAX);
    }
    friend class RandSource;
private:
    RandSource& randSource;
    long currStream;
    size_t currPos;
    bool sync;
};



/**
   @class RandSource
   @brief Source of random numbers
   @details Generates streams of random numbers that are then reused.

   @author Wee Sun Lee
   @date 26 October 2009
*/
class RandSource
{
 public:
    RandSource(long numStream, long blockSize = 100):blockSize(blockSize), numStream(numStream), tempStream(*this, 0,0, true)
    {
        sources.resize(numStream);
        for (long j = 0; j < numStream; j++)
            for (long i=0; i< blockSize; i++){
                sources[j].push_back(rand());
            }
        currStream = 0;
        currNum = 0;
    };

    static void init(unsigned seed) { srand(seed); };

    inline unsigned get()
    {
        if (currNum >= sources[currStream].size()) {
            extend(currStream);
        }
        unsigned out = sources[currStream][currNum];
        currNum++;
        return out;
    };

    inline void extend(long iStream)
    {
        for (long i=0; i < blockSize; i++) sources[iStream].push_back(rand());
    }

    inline double getf()
    {
        return ((double)get() / RAND_MAX);
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

    inline RandStream getStream(long numStream, long numPos = 0)
    {
        return RandStream(*this, numStream, numPos);
    }

    inline operator RandStream&()
    {
        tempStream.currStream = currStream;
        tempStream.currPos = currNum;
        return tempStream;
    }

    inline void reset()
    {
        for (long i=0; i< numStream; i++)
            sources[i].resize(0);
        for (long j = 0; j < numStream; j++)
            for (long i=0; i< blockSize; i++){
                sources[j].push_back(rand());
            }
        currStream = 0;
        currNum = 0;
    };

 friend class RandStream;
 private:
    long blockSize;
    long numStream;
    long currStream;
    size_t currNum;
    std::vector<std::vector<unsigned> > sources;
    RandStream tempStream;
};

unsigned RandStream::get()
{
    if (currPos >= randSource.sources[currStream].size()) {
        #pragma omp critical
        randSource.extend(currStream);
    }

    unsigned out = randSource.sources[currStream][currPos];
    currPos++;
    if (sync) {
        randSource.setStreamPos(currStream, currPos);
    }
    return out;
};

#endif //  __RANDSOURCE_H
