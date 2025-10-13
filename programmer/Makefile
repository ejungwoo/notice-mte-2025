MODULE = update_scaler

SrcSuf = cpp
HdrSuf = h
ObjSuf = o
OutPutOpt = -o #

CXXFLAGS = -I. -I$(LIBUSB_INC) -Wall -Wundef -Wunused -Wshadow -O2 -fPIC -m64
LDFLAGS = -L$(LIBUSB_LIB) -lusb-1.0

PROGRAMMERS = $(MODULE).cpp 
PROGRAMMERH = $(MODULE).h
PROGRAMMERO = $(MODULE).o

PROGRAM = $(MODULE).exe

all: $(PROGRAM)

clean:
	@rm -rf $(PROGRAMMERO) $(PROGRAM)

$(PROGRAM): $(PROGRAMMERO)
	$(CXX) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS) $(LDFLAGS)


