//    This software is Copyright by the Board of Trustees of Michigan State
//    University (c) Copyright 2014. 
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//     Author:
//       Jeromy Tompkins
//	     NSCL
//	     Michigan State University
//	     East Lansing, MI 48824-1321


#ifndef ANALYZER_H
#define ANALYZER_H

#include <stdint.h>
#include <string>

class TH1;
class TH2;
class TGraph;
class TAxis;
class FragmentIndex;

class Analyzer 
{
  private:
    TH1* htdiff;
    TH1* hmult;
    TH1* hmult2d;
    TH1* hs800evolve;
    TH1* hsecevolve;
    TH2* htdiffmult;
    std::vector<TH2*> htdiffevolve;
    TGraph* grtstamp;
    int npoint;

    int nevent;  

  public:
   Analyzer();
   ~Analyzer();

//   Analyzer(const Analyzer& rhs);
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

    void fillEvolving2D(std::vector<TH2*>& hists, double diff, double tstamp);
    TH2* createNewerHist(TH2* oldhist);
    void setBinContents(TH1* h, double val);
    std::string formNewName(std::string hname);
    bool valueInRange(TAxis* axis, double value);
};

#endif
