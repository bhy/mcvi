#ifndef __GAUSSMIXTURE_H
#define __GAUSSMIXTURE_H

#include <vector>
#include "Gauss.h"

#include <iostream>

class GaussMixture
{
  public:
    GaussMixture() {}
    GaussMixture(std::vector<Gauss> const& gauss, std::vector<double> const& weights): gauss(gauss), weights(weights) {}

    double evaluate(double x);

    std::vector<Gauss> gauss;
    std::vector<double> weights;
};
#endif
