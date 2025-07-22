#ifndef __SAMPLETYPES_H__
#define __SAMPLETYPES_H__

#include <complex>
#include <vector>
//#include <valarray>

using sdrreal = double;
using vsdrreal = std::vector<sdrreal>;
using sdrcomplex = std::complex<sdrreal>;
using vsdrcomplex = std::vector<sdrcomplex>;
//using vsdrcomplex = std::valarray<sdrcomplex>;
using vsdrcomplex_iter = vsdrcomplex::iterator;

#endif // __SAMPLETYPES_H__
