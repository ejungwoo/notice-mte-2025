# === Paths ===
LIBUSB_INC = /usr/include/libusb-1.0
LIBUSB_LIB = /usr/lib/x86_64-linux-gnu
NKHOME     = $(PWD)/..

# === Compiler & Flags ===
CC        = gcc
COPTS     = -fPIC -Wall
LDFLAGS   = -O2 -Wall
LIBS      = -L$(LIBUSB_LIB) -lusb-1.0 -L$(NKHOME)/lib -lNoticeCENS_SCALER -lm
INCLUDES  = -I$(NKHOME)/include -I$(LIBUSB_INC)
OutOpt    = -o

# === Find all *.c and generate corresponding names
MAINS     := $(wildcard *.c)
EXES      := $(patsubst %.c,%.exe,$(MAINS))
OBJS      := $(patsubst %.c,%.o,$(MAINS))

INSTALLDIR = $(NKHOME)/bin

# === Targets ===
all: $(EXES)

# Rule to compile foo.o → foo.exe
%.exe: %.o
	$(CC) $^ $(LIBS) $(OutOpt)$@ $(LDFLAGS)
	@echo "$@ done"

# Compile .o from *.c
%.o: %.c
	$(CC) $(COPTS) $(INCLUDES) -c -o $@ $<

# Install
install: all
	install $(EXES) $(INSTALLDIR)

# Clean
clean:
	rm -f $(OBJS) $(EXES)

