
#include "FragmentIndex.h"
#include "Analyzer.h"

#include <iostream>
#include <string>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TObjString.h>
#include <TObjArray.h>

// Instantiate the beast
Analyzer gAnalyzer;

namespace MergedInfo {
  uint32_t DDAS = 1;
  uint32_t S800 = 2;
}

Analyzer::Analyzer()
  : htdiff(new TH1D("htdiff","S800 - DDAS Timestamp ;Time Difference (ticks) ;Counts", 2000,-1000,1000)),
  hmult(new TH1D("hmult","Multiplicity ;Multiplicity ;Counts", 5, -0.5, 4.5)),
  hs800evolve(new TH1D("hs800evolve","S800 Counts per Time ;Time (s) ;Counts", 7200, 0, 7200)),
  hddasevolve(new TH1D("hddasevolve","DDAS Counts per Time ;Time (s) ;Counts", 7200, 0, 7200)),
  hmult2d(new TH2D("hmult2d","Decomposed Multiplicity ;S800 Multiplicity ;DDAS Multiplicity ;Counts", 5,-0.5,4.5, 5,-0.5,4.5)),
  htdiffmult(new TH2D("htdiffmult","Multiplicity vs. S800-DDAS Tstamp ;Time Difference (ticks) ;Multiplicity ;Counts", 2000,-1000,1000, 5,-0.5,4.5)),
  htdiffevolve(),
  grtstamp(new TGraph(10000)),
  npoint(0)
{
  htdiffevolve.push_back(new TH2D("htdiffevolve_0",
                                  "Evolution of S800-DDAS Tstamp ;Time Difference (ticks) ;Time (sec) ;Counts", 
                                  2000,-1000,1000, 1800, 0, 7200 )
                        );

  htdiff->SetDirectory(0);
  hmult->SetDirectory(0);
  hs800evolve->SetDirectory(0);
  hddasevolve->SetDirectory(0);
  hmult2d->SetDirectory(0);
  htdiffmult->SetDirectory(0);
  htdiffevolve.back()->SetDirectory(0);
}

//Analyzer::Analyzer(const Analyzer& rhs) 
//{
////  std::string hname = rhs.htdiff->GetName();
////  htdiff = dynamic_cast<TH1*>(rhs.htdiff->Clone((hname + "c").c_str()));
////  htdiff->SetDirectory(0);
////
////  hname = rhs.hmult->GetName();
////  hmult = dynamic_cast<TH1*>(rhs.hmult->Clone((hname + "c").c_str()));
////  hmult->SetDirectory(0);
////
////  hname = rhs.hmult2d->GetName();
////  hmult2d = dynamic_cast<TH1*>(rhs.hmult2d->Clone((hname + "c").c_str()));
////  hmult2d->SetDirectory(0);
////
////  hname = rhs.htdiffmult->GetName();
////  htdiffmult = dynamic_cast<TH2*>(rhs.htdiffmult->Clone((hname + "c").c_str()));
////  htdiffmult->SetDirectory(0);
////
////  hname = rhs.htdiffevolve->GetName();
////  htdiffevolve = dynamic_cast<TH2*>(rhs.htdiffevolve->Clone((hname + "c").c_str()));
////  htdiffevolve->SetDirectory(0);
//  clone (rhs.htdiff, htdiff);
//  clone (rhs.hmult, hmult);
//  clone (rhs.hs800evolve, hs800evolve);
//  clone (rhs.hmult2d, hmult2d);
//  clone (rhs.htdiffmult, htdiffmult);
//  clone (rhs.htdiffevolve, htdiffevolve);
//}

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
  hddasevolve->Write();
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
  delete hddasevolve;
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
  bool foundS800 = false;
  bool foundDDAS = false;
  double s800tstamp = 0;
  double ddaststamp = 0;
  int s800count=0;
  int ddascount=0;

  FragmentIndex::iterator it=index.begin();
  FragmentIndex::iterator end=index.end();
  while ( it != end) {
    if (it->s_sourceId == MergedInfo::DDAS) {
      if (!foundDDAS )  {
        ddaststamp = it->s_timestamp;
        foundDDAS = true;
      }
      hddasevolve->Fill(ddaststamp*8.0e-9);
      ++ddascount;
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
  hmult2d->Fill(s800count,ddascount);

  if (foundS800 && foundDDAS) {
    double diff = s800tstamp - ddaststamp;
    htdiff->Fill(diff);
    grtstamp->SetPoint(npoint, s800tstamp, ddaststamp);
    htdiffmult->Fill(diff, nfrags);
    fillEvolving2D(htdiffevolve, diff, ddaststamp*8.0e-9);
    ++npoint;
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

TString Analyzer::formNewName(TString hname)
{
  TObjArray* decomp = hname.Tokenize("_");
  TString basename = hname;
  int index=0;
  if (decomp->GetEntries()==2) {
    basename = (dynamic_cast<TObjString*>(decomp->At(0)))->String();
    index = (dynamic_cast<TObjString*>(decomp->At(1)))->String().Atoi();
  } else {
    std::cout << "Unable to intelligently form new histo name from ";
    std::cout << hname.Data() << std::endl;
  }
  return TString::Format("%s_%d",basename.Data(), index+1);
}

TH2* Analyzer::createNewerHist(TH2* oldhist)
{
  TAxis* ax = oldhist->GetXaxis();
  TAxis* ay = oldhist->GetYaxis();
  double yhi = ay->GetXmax();
  double yrange = yhi - ay->GetXmin();
  int nbins = ay->GetNbins();

  // Create the new hist and clear all of its bin contents and info
  TH2* hnew = new TH2D(formNewName(oldhist->GetName()).Data(),
                      "Evolution of S800-DDAS Tstamp ;Time Difference (ticks) ;Time (sec) ;Counts", 
                      ax->GetNbins(), ax->GetXmin(), ax->GetXmax(),
                      nbins, yhi, yhi+yrange);
  setBinContents(hnew,0); 
  hnew->Reset("ICESM");
  hnew->SetDirectory(0);
  

  return hnew;
}
