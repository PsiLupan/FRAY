#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
  $(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

export	LIBOGC_INC	:=	$(DEVKITPRO)/libogc/include
export	LIBOGC_LIB	:=	$(DEVKITPRO)/libogc/lib/cube

include $(DEVKITPPC)/gamecube_rules

BUILD				:= build

export BASEDIR		:= $(CURDIR)
export HSDDIR		:= $(BASEDIR)/hsd
export LIBS			:=	$(BASEDIR)/lib

export INCDIR		:=	../include/hsd/

MACHDEP		:= -DBIGENDIAN -DGEKKO -mcpu=750 -meabi -msdata=eabi -mhard-float -ffunction-sections -fdata-sections
MACHDEP		+=	-DHW_DOL

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDES	:=	-I$(BASEDIR) -I$(BASEDIR)/hsd \
					-I$(LIBOGC_INC)

CFLAGS		:= -DHSD_INTERNAL -g -O2 -fno-strict-aliasing -Wall $(MACHDEP) $(INCLUDES)


HSDLIB		:= $(LIBS)/hsd

#---------------------------------------------------------------------------------
VPATH :=	$(HSDDIR)
					
#---------------------------------------------------------------------------------
HSDOBJ		:=	\
				hsd_aobj.o hsd_cobj.o hsd_debug.o hsd_display.o hsd_dobj.o hsd_fobj.o \
				hsd_fog.o hsd_init.o hsd_jobj.o hsd_lobj.o hsd_memory.o hsd_mobj.o \
				hsd_object.o hsd_pad.o hsd_pobj.o hsd_robj.o hsd_state.o hsd_texp.o \
				hsd_tobj.o hsd_util.o hsd_video.o hsd_wobj.o

all: dist				

#---------------------------------------------------------------------------------
$(HSDLIB).a: $(HSDOBJ)
#---------------------------------------------------------------------------------

.PHONY: libs build dist

#---------------------------------------------------------------------------------
install-headers:
#---------------------------------------------------------------------------------
	@mkdir -p $(INCDIR)
	@cp ./hsd/*.h ../include/hsd/
	@cp ./build/hsdlib.a ../lib/hsd/

#---------------------------------------------------------------------------------
build:
#---------------------------------------------------------------------------------
	@[ -d $(INCDIR) ] || mkdir -p $(INCDIR)
	@[ -d $@ ] || mkdir -p $@
	@make libs -C hsd -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
dist: build install-headers
#---------------------------------------------------------------------------------
LIBRARIES	:=	$(HSDLIB).a
	
#---------------------------------------------------------------------------------	
libs: $(LIBRARIES)
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
clean:
#---------------------------------------------------------------------------------
	rm -fr ./hsd/*.o