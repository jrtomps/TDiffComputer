

CC = gcc
CPP = g++

PKGNAME := filter 
INSTALLDIR := .
BUILDDIR := . 
SOURCEDIR := .

DAQROOT := /usr/opt/nscldaq/10.2-104

LIBS := $(shell cppunit-config --libs)

LIBS += $(shell root-config --libs)

LIBDIR := $(DAQROOT)/lib
INCLUDES += -I$(SOURCEDIR) $(shell cppunit-config --cflags)
INCLUDES += -I$(DAQROOT)/include
CPPFLAGS += $(INCLUDES)

LIBS += -L$(DAQROOT)/lib -ldataformat -ldaqshm -lurl
LIBS += -L/usr/lib -lException

CPPFLAGS += $(shell root-config --cflags) -fPIC

CPPFLAGS += -g

headers := CFileDataSink.h \
			CRingDataSink.h \
			CDataSinkFactory.h \
			CMediator.h  \
			CFilterMain.h \
			StringsToIntegers.h \
  		CCompositeFilter.h \
			FragmentIndex.h \
			CTemplateFilter.h \
			filterargs.h \
			CStdinDataSource.h

objs    := $(patsubst %.h, %.o, $(headers))

OBJS := $(objs)


.PHONEY : all setup build

build : libfilter.so UserFilter

all : build check
	
setup : 
	@if [ ! -d $(BUILDDIR) ] ; then mkdir $(BUILDDIR) ; fi

%.o : %.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

%.o : %.c
	$(CC) $(CPPFLAGS) -c -o $@ $<

libfilter.so : $(OBJS)
	$(CPP) -shared -Wl,"-soname=libfilter.so" -Wl,"-rpath=$(INSTALLPATH)" $(CPPFLAGS) -o $@ $^ $(LIBS) 

clean :
	rm -f $(OBJS) 


Analyzer.o : Analyzer.cpp Analyzer.cpp
	$(CPP) -c $(shell root-config --cflags) -o $@ $< $(shell root-config --libs) 

LDFLAGS = $(LIBDIR)/libdaqshm.so	\
	$(LIBDIR)/libdataformat.so	\
	$(LIBDIR)/liburl.so	\
	-L/usr/lib -lException \
	$(TCL_LDFLAGS) -lpthread -lrt \
	-Wl,-rpath=$(LIBDIR) \
	$(LIBS)

UserFilter : $(OBJS) SkeletonMain.o CTemplateFilter.o Analyzer.o
	$(CPP) $(OBJECTS) -o $@ $(LDFLAGS) -Wl,-rpath=$(INSTALLPATH) SkeletonMain.o CTemplateFilter.o Analyzer.o libfilter.so
