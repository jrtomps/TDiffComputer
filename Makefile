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






