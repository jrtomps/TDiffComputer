#    This software is Copyright by the Board of Trustees of Michigan State
#    University (c) Copyright 2014. 
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#     Author:
#      Jeromy Tompkins
#	     NSCL
#	     Michigan State University
#	     East Lansing, MI 48824-1321

INSTDIR=/usr/devopt/nscldaq/11.0

include $(INSTDIR)/filterkit/FilterIncludes




#
#  Add your own compilation/link flags here:

USERCXXFLAGS=$(shell root-config --cflags)
USERCCFLAGS=$(USERCCFLAGS)
USERLDFLAGS=$(shell root-config --libs)

#
#  Add the names of objects you need here if you modified the name of the driver file, 
#  this should also reflect thtat.
#
OBJECTS = FragmentIndex.o Analyzer.o CTemplateFilter.o SkeletonMain.o

#
#  Modify the line below to provide the name of the library you are trying to build
#  it must be of the form libsomethingorother.so
#

USERFILTER = UserFilter

$(USERFILTER): $(OBJECTS)
	   $(CXX) $(OBJECTS) -o $@ $(USERLDFLAGS) $(LDFLAGS)






