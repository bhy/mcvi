#include <cmath>
#include <cstdlib>
#include "Gauss.h"
#include "Utils.h"
using namespace std;

Gauss::Gauss(double const mean, double const variance):
        mean(mean), variance(variance),
        deviation(sqrt(variance)),
        factor(sqrt(2 * M_PI * variance)),
        have_sample(false)
{
}

double Gauss::evaluate(double x)
{
    return exp(-(x - mean) * (x-mean) / (2 * variance)) / factor;
}

double Gauss::sample(RandStream rand_stream)
{
    if (have_sample) {
        have_sample = false;
        return saved_sample;
    }

    double x1, x2, w;
    do {
        x1 = 2 * rand_stream.randf() - 1;
        x2 = 2 * rand_stream.randf() - 1;
        w = x1 * x1 + x2 * x2;
    } while (w == 0 || w >= 1);

    w = sqrt( -2 * log(w) / w);
    double y1 = x1 * w, y2 = x2 * w;
    saved_sample = deviation * y2 + mean;
    have_sample = true;

    return deviation * y1 + mean;
}
