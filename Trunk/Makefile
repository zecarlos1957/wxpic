#############################################
#  Purpose:
#    Makefile for Linux only
#
#  Authors: 
#    Philippe Chevrier  pch@laposte.net
#    Erdem U. Altinyurt http://erdem_ua.users.sourceforge.net/
#
#  License: see Doc/License.txt
#    
#############################################
# targets:
#    - all: build the binaries and the test environment
#    - clean: remove the files generated or copied by all
#    - install: install the application in the target directories (requires sudo)
#    - uninstall: clean files copied by install (requires sudo)


#############################################
##  PROJECT DATA
#############################################

# Windows name
EXECUTABLE=WxPic
# Linux name (lower case if uppercase not supported)
TGT_EXECUTABLE=wxpic

# Use YES or NO to indicate if Code::Blocks is available and can be used
# Without Code::Block the obsolescence of Object files ignores the .h dates (should be used only for full rebuild)
CODEBLOCKS=NO

# Project and Test Directories
# Bin subdirectory of project target path defined in Code::Blocks project properties
TSTROOT =bin
RELEASE =LinuxRelease
TESTDIR =$(TSTROOT)/Linux*
TESTDIRS=$(wildcard $(TESTDIR))
PRJDIR  =$(TSTROOT)/$(RELEASE)
PRJXEDIR=$(PRJDIR)/bin
PRJEXEC =$(PRJXEDIR)/$(EXECUTABLE)
TMP_DIR =obj/$$REDIRECT_TMP$$


#############################################
##  SOURCE FILES
#############################################

# Graphic resources
RSCDIR    =resources
SRCPIXMAP =$(RSCDIR)/$(TGT_EXECUTABLE).png
SRCDESKTOP=$(RSCDIR)/$(TGT_EXECUTABLE).desktop

# Executable source files
SRCDIR =src
ifeq ($(CODEBLOCKS),NO)
SOURCES=$(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp) $(wildcard $(RSCDIR)/*.cpp)
else
SOURCES=$(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*/*.cpp) \
        $(wildcard $(RSCDIR)/*.cpp) $(wildcard $(RSCDIR)/*.h) $(wildcard $(RSCDIR)/*.xpm) \
        $(wildcard $(SRCDIR)/*.h) $(wildcard $(SRCDIR)/*/*.h)
endif

# Language list
LANGSRCDIR=Lang
LANSRC    =$(wildcard $(LANGSRCDIR)/*/$(EXECUTABLE).po)

# Help files
HELPDIR     =Web/Help
HELPEXT     =htm* png jp*g ico gif css
HELPFILES   =$(foreach EXT,$(HELPEXT),$(wildcard $(HELPDIR)/*.$(EXT)) $(wildcard $(HELPDIR)/*/*.$(EXT)))

MAPFILENAME =wxHelp.map
MAPFILES    =$(HELPDIR)/$(MAPFILENAME) $(subst $(LANGSRCDIR)/,$(HELPDIR)/,$(subst $(EXECUTABLE).po,$(MAPFILENAME),$(LANSRC)))

# Doc files
DOCDIR   =doc
DOC      =$(wildcard $(DOCDIR)/*.txt)

# Non-graphic resources
INSTDIR=Install
INSTRES=$(INSTDIR)/devices.ini \
        $(INSTDIR)/SampleInterfaceOnSerialPort.xml \
        $(INSTDIR)/SampleInterfaceOnLptPort.xml
		

#############################################
##  OBJECT FILES
#############################################
ifeq ($(CODEBLOCKS),NO)
OBJECTS   =$(SOURCES:.cpp=.o)
endif
MOBJECTS=$(LANSRC:.po=.mo)


#############################################
##  TARGET STRUCTURE
#############################################

# Target Structure below prefix or one of the test directories
DESTDIR   =
PREFIX    =$(DESTDIR)/usr

BINDIR    =bin
DATADIR   =share
LOCALEDIR =$(DATADIR)/locale
LCMSGDIR  =LC_MESSAGES
TSLANGFILE=$(subst $(LANGSRCDIR),$(PRJDIR)/$(LOCALEDIR),$(subst $(EXECUTABLE).mo,$(LCMSGDIR)/$(EXECUTABLE).mo,$(MOBJECTS)))
TGLANGFILE=$(subst $(LANGSRCDIR),$(PREFIX)/$(LOCALEDIR),$(subst $(EXECUTABLE).mo,$(LCMSGDIR)/$(TGT_EXECUTABLE).mo,$(MOBJECTS)))
TGTPIXMAP =$(PREFIX)/$(DATADIR)/pixmaps/$(TGT_EXECUTABLE).png
TGTDESKTOP=$(PREFIX)/$(DATADIR)/applications/$(TGT_EXECUTABLE).desktop
# DATA directory is different weither it is TEST or Target
TSTDATA   =$(DATADIR)/$(EXECUTABLE)
TGTDATA   =$(DATADIR)/$(TGT_EXECUTABLE)
TGTFILES  =$(subst $(INSTDIR),$(PREFIX)/$(TGTDATA),$(INSTRES))
TSTFILES  =$(subst $(INSTDIR),$(PRJDIR)/$(TSTDATA),$(INSTRES))
# Data Structure below $(TSTDATA) or $(TGTDATA)
TGTHELP   =Help
TGTHFILES =$(subst $(HELPDIR),$(PREFIX)/$(TGTDATA)/$(TGTHELP),$(HELPFILES))
TSTHFILES =$(subst $(HELPDIR),$(PRJDIR)/$(TSTDATA)/$(TGTHELP),$(HELPFILES))
TGTMAPS   =$(subst $(HELPDIR),$(PREFIX)/$(TSTDATA)/$(TGTHELP),$(MAPFILES))
TSTMAPS   =$(subst $(HELPDIR),$(PRJDIR)/$(TSTDATA)/$(TGTHELP),$(MAPFILES))
TGDOCFILES=$(subst $(DOCDIR),$(PREFIX)/$(TGTDATA),$(DOC))
TSDOCFILES=$(subst $(DOCDIR),$(PRJDIR)/$(TSTDATA),$(DOC))


#############################################
##  TOOLS
#############################################

MSGFMT     =msgfmt
INSTALLEXE =install -D -m 755
INSTALLDAT =install -D -m 644
ifeq ($(CODEBLOCKS),NO)
WXCONFIG   =wx-config
CXX        =`$(WXCONFIG) --cxx`
MKDIR      =mkdir -p 
BUILD      =$(CXX) `$(WXCONFIG) --libs` -static-libgcc
else
BUILD      =codeblocks --build --target="Release Linux"
endif
CPPC       =$(CXX) `$(WXCONFIG) --cxxflags` -O2 -Isrc
RM         =rm -f
RMDIR      =rm -f -R
XSLT_MAP   =saxonb-xslt -ext:on -xsl:Map2Redirect.xslt
XSLT_IN    =-s:


#############################################
##  BUILDING RULES
#############################################

.PHONY: uninstall clean


all: $(PRJEXEC) langs

ifeq ($(CODEBLOCKS),NO)
.cpp.o:
	$(CPPC) -c $< -o $@

$(PRJEXEC): $(OBJECTS)
	$(MKDIR) $(PRJXEDIR)
	$(BUILD) $(OBJECTS) -o $@
else
$(PRJEXEC): $(SOURCES)
	$(BUILD) $(EXECUTABLE).cbp
endif


langs: $(MOBJECTS)

%.mo: %.po
	$(MSGFMT) $< -o $@
 
## Install rules
##################
$(PREFIX)/$(BINDIR)/$(TGT_EXECUTABLE): $(PRJEXEC)
	$(INSTALLEXE) $< $@

$(TGTFILES): $(PREFIX)/$(TGTDATA)%: $(INSTDIR)%
	$(INSTALLDAT) $< $@

$(TGTHFILES): $(PREFIX)/$(TGTDATA)/$(TGTHELP)%: $(HELPDIR)%
	$(INSTALLDAT) $< $@

$(TGTMAPS): $(PREFIX)/$(TGTDATA)/$(TGTHELP)%: $(HELPDIR)%
	$(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@)
	$(MKDIR) $(TMP_DIR)
	$(XSLT_MAP) $(XSL_IN)$< TARGET_DIR=$(CURDIR)/$(TMP_DIR)
	$(INSTALLDAT) $(TMP_DIR)/* $(subst /$(MAPFILENAME),,$@)
	$(RMDIR) $(TMP_DIR)

$(TGDOCFILES): $(PREFIX)/$(TGTDATA)%: $(DOCDIR)%
	$(INSTALLDAT) $< $@

$(TGLANGFILE): $(PREFIX)/$(LOCALEDIR)%$(LCMSGDIR)/$(TGT_EXECUTABLE).mo: $(LANGSRCDIR)%$(EXECUTABLE).mo
	$(INSTALLDAT) $< $@

$(TGTPIXMAP): $(SRCPIXMAP)
	$(INSTALLDAT) $< $@

$(TGTDESKTOP): $(SRCDESKTOP)
	$(INSTALLDAT) $< $@
	
install: $(PREFIX)/$(BINDIR)/$(TGT_EXECUTABLE) $(TGTFILES) $(TGTHFILES) $(TGTMAPS) $(TGDOCFILES) $(TGLANGFILE) $(TGTPIXMAP) $(TGTDESKTOP)

tar: install
	tar -cf $(EXECUTABLE).tar --directory=$(PREFIX) $(BINDIR)/$(TGT_EXECUTABLE) $(TGTDATA) \
					$(subst $(PREFIX)/,,$(TGLANGFILE)) \
					$(DATADIR)/pixmaps/$(TGT_EXECUTABLE).png \
					$(DATADIR)/applications/$(TGT_EXECUTABLE).desktop
	gzip $(EXECUTABLE).tar

uninstall:
	$(RM)    $(PREFIX)/$(BINDIR)/$(TGT_EXECUTABLE)
	$(RMDIR) $(PREFIX)/$(TGTDATA)
	$(RM)    $(TGLANGFILE)
	$(RM)    $(TGTPIXMAP) $(TGTDESKTOP)

# TEST rules
##############

$(TSTFILES): $(PRJDIR)/$(TSTDATA)%: $(INSTDIR)%
	@for i in $(TESTDIR); do \
	    echo "$(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@)"; \
	    $(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@); done

$(TSTHFILES): $(PRJDIR)/$(TSTDATA)/$(TGTHELP)%: $(HELPDIR)%
	@for i in $(TESTDIR); do \
	    echo "$(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@)"; \
	    $(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@); done

$(TSTMAPS): $(PRJDIR)/$(TSTDATA)/$(TGTHELP)%: $(HELPDIR)%
	@for i in $(TESTDIR); do \
	    echo "$(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@)"; \
	    $(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@); \
	    echo "$(XSLT_MAP) $(XSL_OUT)$(DUMMY) $(XSL_IN)$< TARGET_DIR=$(CURDIR)/$$i/$(subst /$(MAPFILENAME),,$(subst $(PRJDIR)/,,$@))"; \
	    $(XSLT_MAP) $(XSL_IN)$< TARGET_DIR=$(CURDIR)/$$i/$(subst $(MAPFILENAME),,$(subst $(PRJDIR)/,,$@)); done

$(TSDOCFILES): $(PRJDIR)/$(TSTDATA)%: $(DOCDIR)%
	@for i in $(TESTDIR); do \
	    echo "$(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@)"; \
	    $(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@); done

$(TSLANGFILE): $(PRJDIR)/$(LOCALEDIR)%$(LCMSGDIR)/$(EXECUTABLE).mo: $(LANGSRCDIR)%$(EXECUTABLE).mo
	@for i in $(TESTDIR); do \
	    echo "$(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@)"; \
	    $(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@); done


test: $(TSTFILES) $(TSTHFILES) $(TSTMAPS) $(TSDOCFILES) $(TSLANGFILE)


clean:
	$(RM)    $(MOBJECTS)
	$(RM)    $(TESTDIR)/$(BINDIR)/$(EXECUTABLE)
	$(RM)    $(TESTDIR)/$(LOCALEDIR)/*/$(LCMSGDIR)/$(TGT_EXECUTABLE).mo
	$(RMDIR) $(TESTDIR)/$(TSTDATA)
ifeq ($(CODEBLOCKS),NO)
	$(RM)    $(OBJECTS)
else
	@echo "To clean object files use Build > Clean command from Code::Blocks IDE"
endif

