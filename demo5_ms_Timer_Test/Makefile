PROJECT=demo05

# hint from C:\gcc-linaro\share\doc\gcc-linaro-arm-linux-gnueabihf\README.txt 

CURFOLD := .

# Directory for C-Source
vpath %.c $(CURFOLD)/src

# Directory for includes
CINCLUDE = $(CURFOLD)/include  

# Directory for object files
OBJDIR = $(CURFOLD)/object

# Other dependencies
DEPS = \
 Makefile  \
 include/mstime.h 

# include/$(PROJECT).h 
 

# Compiler object files 
COBJ = \
 $(OBJDIR)/$(PROJECT).o \
 $(OBJDIR)/mstime.o 



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
CFLAGS += -pthread

# for a better output
MSG_EMPTYLINE = . 
MSG_COMPILING = ---COMPILE--- 
MSG_LINKING = ---LINK--- 
MSG_SUCCESS = ---SUCCESS--- 

# Our favourite
all: $(PROJECT)

# Linker call                -lrt for timer_create()
$(PROJECT): $(COBJ)
	@echo $(MSG_EMPTYLINE)
	@echo $(MSG_LINKING)
	$(LD) -o $@ $^ $(CFLAGS) -lrt
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

