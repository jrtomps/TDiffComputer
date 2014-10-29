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
//
#include "FragmentIndex.h"
#include "Analyzer.h"

#include <iostream>
#include <iomanip>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TString.h>

// Instantiate the beast
Analyzer gAnalyzer;

namespace MergedInfo {
  uint32_t Secondary = 0;
  uint32_t S800 = 5;
}

Analyzer::Analyzer()
  : htdiff(new TH1D("htdiff","S800 - Secondary Timestamp ;Time Difference (ticks) ;Counts", 2000,-1000,1000)),
  hmult(new TH1D("hmult","Multiplicity ;Multiplicity ;Counts", 5, -0.5, 4.5)),
  hs800evolve(new TH1D("hs800evolve","S800 Counts per Time ;Time (s) ;Counts", 7200, 0, 7200)),
  hsecevolve(new TH1D("hsecevolve","Secondary Counts per Time ;Time (s) ;Counts", 7200, 0, 7200)),
  hmult2d(new TH2D("hmult2d","Decomposed Multiplicity ;S800 Multiplicity ;Secondary Multiplicity ;Counts", 24,-0.5,24.5, 25,-0.5,24.5)),
  htdiffmult(new TH2D("htdiffmult","Multiplicity vs. S800-Secondary Tstamp ;Time Difference (ticks) ;Multiplicity ;Counts", 2000,-1000,1000, 25,-0.5,24.5)),
  htdiffevolve(),
  grtstamp(new TGraph(10000)),
  npoint(0),
  nevent(0)
{
  htdiffevolve.push_back(new TH2D("htdiffevolve_0",
                                  "Evolution of S800-DDAS Tstamp ;Time Difference (ticks) ;Time (sec) ;Counts", 
                                  3000,-3000,3000, 1800, 0, 7200 )
                        );

  htdiff->SetDirectory(0);
  hmult->SetDirectory(0);
  hs800evolve->SetDirectory(0);
  hsecevolve->SetDirectory(0);
  hmult2d->SetDirectory(0);
  htdiffmult->SetDirectory(0);
  htdiffevolve.back()->SetDirectory(0);
}

template<class T> void Analyzer::clone(T h1, T h2)
{
  std::string hname = h1->GetName();
  h2 = dynamic_cast<T>(h1->Clone((hname + "c").c_str()));
  h2->SetDirectory(0);
}

Analyzer::~Analyzer() 
{
  TFile f("analyzer.root","RECREATE");
  htdiff->Write();
  hmult->Write();
  hs800evolve->Write();
  hsecevolve->Write();
  hmult2d->Write();
  htdiffmult->Write();

  unsigned int size = htdiffevolve.size();
  for (unsigned int i=0; i<size; ++i) {
    std::cout << "\nWriting " << htdiffevolve.at(i)->GetName() << std::flush;
    htdiffevolve.at(i)->Write();
  }
  std::cout << std::endl;

  grtstamp->Write("grtstamp");
  f.Close();

  delete htdiff;
  delete hmult;
  delete hs800evolve;
  delete hsecevolve;
  delete hmult2d;
  delete htdiffmult;

  for (unsigned int i=0; i<htdiffevolve.size(); ++i) {
    if (htdiffevolve.at(i)!=0) {
      delete htdiffevolve.at(i);
    }
  }

  delete grtstamp;
}

void Analyzer::operator()(FragmentIndex& index)
{

  ++nevent;

  bool foundS800 = false;
  bool foundSecondary = false;
  double s800tstamp = 0;
  double sectstamp = 0;
  int s800count=0;
  int seccount=0;

  FragmentIndex::iterator it=index.begin();
  FragmentIndex::iterator end=index.end();
  while ( it != end) {
    if (it->s_sourceId == MergedInfo::Secondary) {
      if (!foundSecondary )  {
        sectstamp = it->s_timestamp;
        foundSecondary = true;
      }
      hsecevolve->Fill(sectstamp*8.0e-9);
      ++seccount;
    } else if (it->s_sourceId == MergedInfo::S800) {
      uint32_t ritem_type = computeType(it->s_itemhdr+2);
      if (ritem_type == 30) {
        if (!foundS800) { 
          s800tstamp = it->s_timestamp;
          foundS800 = true;
        }
        ++s800count;
        hs800evolve->Fill(s800tstamp*8.0e-9);
      } 
    } 
    ++it;
  }

  int nfrags=index.getNumberFragments();
  hmult->Fill(nfrags);
  hmult2d->Fill(s800count,seccount);

  if (foundS800 && foundSecondary) {
    double diff = s800tstamp - sectstamp;
    htdiff->Fill(diff);
    grtstamp->SetPoint(npoint, s800tstamp, sectstamp);
    htdiffmult->Fill(diff, nfrags);
    fillEvolving2D(htdiffevolve, diff, sectstamp*8.0e-9);
    ++npoint;
  } else {
    std::cout.flags(std::ios::fixed);
    std::cout.precision(0);
    std::cout << "\nFound an unbuilt event! (evt# " << nevent << ")";
    std::cout << " s800tstamp=" << std::setw(10) << s800tstamp;
    std::cout << " sectstamp=" << std::setw(10) << sectstamp;
  }

}

void Analyzer::fillEvolving2D(std::vector<TH2*>& hists, double diff, double tstamp)
{
    TH2* hcurrent = hists.back();
  
    if (!valueInRange(hcurrent->GetYaxis(),tstamp)) {
      hcurrent = createNewerHist(hcurrent);
      hists.push_back(hcurrent);
    }

    hcurrent->Fill(diff,tstamp);
}

bool Analyzer::valueInRange(TAxis* axis, double value)
{
   return ( value > axis->GetXmin() && value < axis->GetXmax() );
}

void Analyzer::setBinContents(TH1* h, double val)
{
  TAxis* ax = h->GetXaxis();
  TAxis* ay = h->GetYaxis();
  TAxis* az = h->GetZaxis();

  for (int xbin=0; xbin<=ax->GetNbins(); ++xbin) {
    for (int ybin=0; ybin<=ay->GetNbins(); ++ybin) {
      for (int zbin=0; zbin<=az->GetNbins(); ++zbin) {
        h->SetBinContent(xbin,ybin,zbin,0);
      }
    }
  }
}

std::string Analyzer::formNewName(std::string hname)
{
  TObjArray* decomp = TString(hname).Tokenize("_");
  TString basename = hname;
  int index=0;
  if (decomp->GetEntries()==2) {
    basename = (dynamic_cast<TObjString*>(decomp->At(0)))->String();
    index = (dynamic_cast<TObjString*>(decomp->At(1)))->String().Atoi();
  } else {
    std::cout << "Unable to intelligently form new histo name from ";
    std::cout << hname << std::endl;
  }
  return std::string(TString::Format("%s_%d",basename.Data(), index+1).Data());
}

TH2* Analyzer::createNewerHist(TH2* oldhist)
{
  TAxis* ax = oldhist->GetXaxis();
  TAxis* ay = oldhist->GetYaxis();
  double yhi = ay->GetXmax();
  double yrange = yhi - ay->GetXmin();
  int nbins = ay->GetNbins();

  // Create the new hist and clear all of its bin contents and info
  TH2* hnew = new TH2D(formNewName(oldhist->GetName()).c_str(),
                      "Evolution of S800-DDAS Tstamp ;Time Difference (ticks) ;Time (sec) ;Counts", 
                      ax->GetNbins(), ax->GetXmin(), ax->GetXmax(),
                      nbins, yhi, yhi+yrange);
  setBinContents(hnew,0); 
  hnew->Reset("ICESM");
  hnew->SetDirectory(0);
  

  return hnew;
}
