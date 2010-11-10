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


#############################################
##  SOURCE FILES
#############################################

# Graphic resources
RSCDIR    =resources
SRCPIXMAP =$(RSCDIR)/wxpic.png
SRCDESKTOP=$(RSCDIR)/wxpic.desktop

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
#LANGUAGEDIRS=$(subst $(LANGSRC)/,,$(subst /$(EXECUTABLE).po,,$(LANGUAGES)))

# Help files
HELPDIR  =Web/Help
HELPEXT  =htm* png jp*g ico gif css map
HELPFILES=$(foreach EXT,$(HELPEXT),$(wildcard $(HELPDIR)/*.$(EXT)) $(wildcard $(HELPDIR)/*/*.$(EXT)))

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

$(TGDOCFILES): $(PREFIX)/$(TGTDATA)%: $(DOCDIR)%
	$(INSTALLDAT) $< $@

$(TGLANGFILE): $(PREFIX)/$(LOCALEDIR)%$(LCMSGDIR)/$(TGT_EXECUTABLE).mo: $(LANGSRCDIR)%$(EXECUTABLE).mo
	$(INSTALLDAT) $< $@

$(TGTPIXMAP): $(SRCPIXMAP)
	$(INSTALLDAT) $< $@

$(TGTDESKTOP): $(SRCDESKTOP)
	$(INSTALLDAT) $< $@
	
install: $(PREFIX)/$(BINDIR)/$(TGT_EXECUTABLE) $(TGTFILES) $(TGTHFILES) $(TGDOCFILES) $(TGLANGFILE) $(TGTPIXMAP) $(TGTDESKTOP)
#	$(INSTALLEXE) $(PRJEXEC) $(PREFIX)/$(BINDIR)/$(TGT_EXECUTABLE)
#	$(INSTALLDAT) Install/devices.ini $(PREFIX)/$(TGTDATA)/devices.ini
#	$(INSTALLDAT) Install/SampleInterfaceOnSerialPort.xml $(PREFIX)/$(TGTDATA)/SampleInterfaceOnSerialPort.xml
#	$(INSTALLDAT) Install/SampleInterfaceOnLptPort.xml $(PREFIX)/$(TGTDATA)/SampleInterfaceOnLptPort.xml
#	@for i in $(HELPFILES) ; do \
#	   echo "$(INSTALLDAT) $(HELPDIR)/$$i $(PREFIX)/$(TGTDATA)/$(TGTHELP)/$$i"; \
#	   $(INSTALLDAT) $(HELPDIR)/$$i $(PREFIX)/$(TGTDATA)/$(TGTHELP)/$$i ; done
#	@for i in $(DOCFILES) ; do \
#	   echo "$(INSTALLDAT) $(DOCDIR)/$$i $(PREFIX)/$(TGTDATA)/$$i"; \
#	   $(INSTALLDAT) $(DOCDIR)/$$i $(PREFIX)/$(TGTDATA)/$$i ; done
#	@for i in $(LANGUAGEDIRS); do \
#	   echo "$(INSTALLDAT) $(LANGSRC)/$$i/$(EXECUTABLE).mo $(PREFIX)/$(LOCALEDIR)/$$i/LC_MESSAGES/$(TGT_EXECUTABLE).mo"; \
#	   $(INSTALLDAT) $(LANGSRC)/$$i/$(EXECUTABLE).mo $(PREFIX)/$(LOCALEDIR)/$$i/LC_MESSAGES/$(TGT_EXECUTABLE).mo; done
#	$(INSTALLDAT) resources/$(EXECUTABLE).png $(DATADIR)/pixmaps/$(TGT_EXECUTABLE).png
#	$(INSTALLDAT) resources/$(EXECUTABLE).desktop $(DATADIR)/applications/$(TGT_EXECUTABLE).desktop

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

$(TSDOCFILES): $(PRJDIR)/$(TSTDATA)%: $(DOCDIR)%
	@for i in $(TESTDIR); do \
	    echo "$(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@)"; \
	    $(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@); done

$(TSLANGFILE): $(PRJDIR)/$(LOCALEDIR)%$(LCMSGDIR)/$(EXECUTABLE).mo: $(LANGSRCDIR)%$(EXECUTABLE).mo
	@for i in $(TESTDIR); do \
	    echo "$(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@)"; \
	    $(INSTALLDAT) $< $$i/$(subst $(PRJDIR)/,,$@); done


test: $(TSTFILES) $(TSTHFILES) $(TSDOCFILES) $(TSLANGFILE)
#	@for j in $(TESTDIR); do \
#	    echo "$(INSTALLDAT) Install/devices.ini $$j/$(TSTDATA)/devices.ini"; \
#	    $(INSTALLDAT) Install/devices.ini $$j/$(TSTDATA)/devices.ini; \
#	    echo "$(INSTALLDAT) Install/SampleInterfaceOnSerialPort.xml $$j/$(TSTDATA)/SampleInterfaceOnSerialPort.xml" ; \
#	    $(INSTALLDAT) Install/SampleInterfaceOnSerialPort.xml $$j/$(TSTDATA)/SampleInterfaceOnSerialPort.xml ; \
#	    echo "$(INSTALLDAT) Install/SampleInterfaceOnLptPort.xml $$j/$(TSTDATA)/SampleInterfaceOnLptPort.xml" ; \
#	    $(INSTALLDAT) Install/SampleInterfaceOnLptPort.xml $$j/$(TSTDATA)/SampleInterfaceOnLptPort.xml ; \
#		for i in $(HELPFILES) ; do \
#		   echo "$(INSTALLDAT) $(HELPDIR)/$$i $$j/$(TSTDATA)/$(TGTHELP)/$$i"; \
#		   $(INSTALLDAT) $(HELPDIR)/$$i $$j/$(TSTDATA)/$(TGTHELP)/$$i ; done; \
#		for i in $(DOCFILES) ; do \
#		   echo "$(INSTALLDAT) $(DOCDIR)/$$i $$j/$(TSTDATA)/$$i"; \
#		   $(INSTALLDAT) $(DOCDIR)/$$i $$j/$(TSTDATA)/$$i ; done; \
#	    for i in $(LANGUAGEDIRS); do \
#	        echo "$(INSTALLDAT) Lang/$$i/$(EXECUTABLE).mo $$j/$(LOCALEDIR)/$$i/LC_MESSAGES/$(TGT_EXECUTABLE).mo"; \
#	        $(INSTALLDAT) Lang/$$i/$(EXECUTABLE).mo $$j/$(LOCALEDIR)/$$i/LC_MESSAGES/$(TGT_EXECUTABLE).mo; done; done


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

#distclean: ???
