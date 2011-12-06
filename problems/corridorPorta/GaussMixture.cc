#include "GaussMixture.h"
using namespace std;

double GaussMixture::evaluate(double x)
{
    double result = 0.0;
    for (int i = 0; i < gauss.size(); ++i)
        result += gauss[i].evaluate(x) * weights[i];

    return result;
}
