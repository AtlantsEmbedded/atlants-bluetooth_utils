#############################################################################
# Makefile for building: BLuetooth_dev
# Project:  intellipi
# Written by: Alex Joseph, josephvalex@gmail.com
#############################################################################
CC            = gcc
LINK          = $(CC)
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT -fPIE $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT -fPIE $(DEFINES)
LIBS          = -lbluetooth
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = cp -f
COPY_DIR      = cp -f -Rd
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

#########

####### Files

SOURCES       = simplescan.c 
OBJECTS       = simplescan.o
TARGET        = simplescan


first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS)    -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) -o $(TARGET) $(OBJECTS) $(LIBS)


####### Compile

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

########
# remove object files and executable when user executes "make clean" 

clean: 
	rm *.o $(TARGET)

