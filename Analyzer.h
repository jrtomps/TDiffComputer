
#ifndef ANALYZER_H
#define ANALYZER_H

#include <stdint.h>

class TH1;
class TH2;
class TGraph;
class FragmentIndex;

class Analyzer 
{
  private:
    TH1* htdiff;
    TH1* hmult;
    TH1* hmult2d;
    TH1* hs800evolve;
    TH1* hddasevolve;
    TH2* htdiffmult;
    TH2* htdiffevolve;
    TGraph* grtstamp;
    int npoint;
  
  public:
   Analyzer();
   ~Analyzer();

   Analyzer(const Analyzer& rhs);
   void operator() (FragmentIndex& index);

  private:
    uint32_t computeType(uint16_t* ptr) 
    {
      uint32_t low=0, hi=0;
      low = *ptr; ++ptr;
      hi = *ptr; 
    
      return ((hi<<16)|low);
    }

    template<class T> void clone(T h1, T h2);

};

#endif
