PROJECT=demo01

# hint from C:\gcc-linaro\share\doc\gcc-linaro-arm-linux-gnueabihf\README.txt 
#------------------------------------------------------------------------------
# The linaro compiler wants WINDOWS Pathes
# The make variables us UNIX style pathes and
# CYGWIN uses /cygdrive/c for the absolute drive
# The CYGWIN cygpath can recode the UNIX absolute path to Windows, but
# the MAKEFILE dependencies don't want to see any colons (c:\)
# SO we'll use Windows Absolute pathes to the COMPILERs
# AND we'll only use RELATIVE pathes to the source files
# AS SUCH, we won't use $(CURDIR).  Instead will just use "." to indicate
# the current directory where the Makefile resides and require Make to be
# executed from that folder.
#------------------------------------------------------------------------------
CURFOLD := .
  
# Directory for C-Source
vpath %.c $(CURFOLD)/src

# Directory for includes
CINCLUDE = $(CURFOLD)/include  

# Directory for object files
OBJDIR = $(CURFOLD)/object

# Other dependencies
DEPS = \
 Makefile \
 include/$(PROJECT).h \
 include/lookup.h \
 include/thermistor.h
 

# Compiler object files 
COBJ = \
 $(OBJDIR)/$(PROJECT).o \
 $(OBJDIR)/lookup.o     \
 $(OBJDIR)/thermistor.o

# gcc binaries to use
CC = "C:\gcc-linaro\bin\arm-linux-gnueabihf-gcc.exe"
LD = "C:\gcc-linaro\bin\arm-linux-gnueabihf-gcc.exe"

# rm is part of yagarto-tools
SHELL = cmd
REMOVE = rm -f

# Compiler options
# Two additional flags neccessary for Angstrom Linux. 
# Don't use them with Ubuntu or Debian  
CFLAGS = -marm
ifeq ($(TARGET),angstrom)
CFLAGS += -march=armv4t
CFLAGS += -mfloat-abi=soft
endif

CFLAGS += -O0 
CFLAGS += -g 
CFLAGS += -I.
CFLAGS += -I$(CINCLUDE)
CFLAGS += $(CDEFINE)

# for a better output
MSG_EMPTYLINE = . 
MSG_COMPILING = ---COMPILE--- 
MSG_LINKING = ---LINK--- 
MSG_SUCCESS = ---SUCCESS--- 

# Our favourite
all: $(PROJECT)

# Linker call
$(PROJECT): $(COBJ)
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_LINKING)
	$(LD) -o $@ $^ $(CFLAGS)
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_SUCCESS) $(PROJECT)

# Compiler call
$(COBJ): $(OBJDIR)/%.o: %.c $(DEPS)
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_COMPILING) $<
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(REMOVE) $(OBJDIR)/*.o
	$(REMOVE) $(PROJECT)

