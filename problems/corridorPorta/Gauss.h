#ifndef __GAUSS_H
#define __GAUSS_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "RandSource.h"

class Gauss
{
  public:
    Gauss() {}
    Gauss(double const mean, double const variance);

    // Evaluate the Gaussian function at x
    double evaluate(double x);

    // Draw a sample from this distribution. We have 2 samples and
    // only get one, the other one is saved. So be consistent with the
    // choice of randStream
    double sample(RandStream* rand_stream)

    double mean, variance, deviation, factor;
    double saved_sample;
    bool have_sample;
};
#endif
