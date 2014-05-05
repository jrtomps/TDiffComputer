/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Jeromy Tompkins
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/




#ifndef CTEMPLATEFILTER_CPP
#define CTEMPLATEFILTER_CPP

#include <CFilter.h>
#include <stdint.h>

class TH1;
class CPhysicsEventItem;

extern class Analyzer gAnalyzer;

/**! \class CTemplateFilter
    Here is a sample implementation of a filter to append a reversed copy of the
    data in physics event to its body. This is for illustration purposes.

    See the documentation for the CFilter base class for the virtually declared 
    methods available for dealing with non-physics events. The user has access 
    to all of the different ring item types. In fact, it is not necessary for 
    the user to return the same type of ring item from method as it received. 
*/
class CTemplateFilter : public CFilter
{
  public:
    CTemplateFilter();

    virtual CTemplateFilter* clone() const { return new CTemplateFilter(*this);}

    // handle the state changes
    virtual CRingItem* handlePhysicsEventItem(CPhysicsEventItem* pItem);

};

#endif
