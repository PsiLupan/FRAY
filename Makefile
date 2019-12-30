#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
  $(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/gamecube_rules

export CC := powerpc-eabi-clang
MACHDEP =  -DGEKKO -mcpu=750 \
	   -D__gamecube__ -DHW_DOL -ffunction-sections -fdata-sections

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:= main
BUILD		:= build
SOURCES		:= src/asm src/ogcext/ src/hsd/ src
INCLUDES	:= 

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

#NODEBUG = -DNDEBUG
CFLAGS	:= -O1 -std=gnu18 -fstack-protector-strong -Wall -Wno-implicit-function-declaration -Wno-missing-braces -Wno-switch $(MACHDEP) $(INCLUDE) $(NODEBUG)
CXXFLAGS	:= $(CFLAGS)

LDFLAGS	:= -g $(MACHDEP) -Wl,--unresolved-symbols=ignore-in-object-files,-Map,$(notdir $@).map -T$(PWD)/ogc.ld
ASFLAGS :=	$(MACHDEP) -D_LANGUAGE_ASSEMBLY $(INCLUDE)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -logc -lm

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= 

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC) -Wl,--gc-sections -nostartfiles \
		$(DEVKITPPC)/lib/gcc/powerpc-eabi/*/crtend.o \
		$(DEVKITPPC)/lib/gcc/powerpc-eabi/*/ecrtn.o \
		$(DEVKITPPC)/lib/gcc/powerpc-eabi/*/ecrti.o \
		$(DEVKITPPC)/lib/gcc/powerpc-eabi/*/crtbegin.o \
		$(DEVKITPPC)/powerpc-eabi/lib/crtmain.o
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-isystem /d/LLVM/lib/clang/9.0.0/include \
					-isystem $(DEVKITPPC)/powerpc-eabi/include \
					-isystem $(LIBOGC_INC)
					
#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					-L$(LIBOGC_LIB)
					
export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean
					
#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)
	
#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------