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





#include <CFilter.h>
#include <iostream>
#include <CPhysicsEventItem.h>
#include "CTemplateFilter.h"
#include "Analyzer.h"
#include "FragmentIndex.h"


/**! \class CTemplateFilter
    Here is a sample implementation of a filter to append a reversed copy of the
    data in physics event to its body. This is for illustration purposes.

    See the documentation for the CFilter base class for the virtually declared 
    methods available for dealing with non-physics events. The user has access 
    to all of the different ring item types. In fact, it is not necessary for 
    the user to return the same type of ring item from method as it received. 
*/
CTemplateFilter::CTemplateFilter() 
{
}

    // handle the state changes
CRingItem* CTemplateFilter::handlePhysicsEventItem(CPhysicsEventItem* pItem) 
{
  uint16_t* pBody = reinterpret_cast<uint16_t*>(pItem->getBodyPointer());
  FragmentIndex frags(pBody);

  gAnalyzer(frags);

  // return the original item
  return pItem; 
}

