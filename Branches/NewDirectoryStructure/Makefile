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

# Project and Test Directories
# Bin subdirectory of project target path defined in Code::Blocks project properties
TSTROOT =Bin
RELEASE =LinuxRelease
TESTDIR =$(TSTROOT)/Linux*
TESTDIRS=$(wildcard $(TESTDIR))
PRJDIR  =$(TSTROOT)/$(RELEASE)
PRJEXEC =$(PRJDIR)/bin/$(EXECUTABLE)


#############################################
##  SOURCE FILES
#############################################

# Executable source files
SRCDIR =src
RSCDIR =resources
SOURCES=$(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/*.h) \
        $(wildcard $(SRCDIR)/*/*.cpp) $(wildcard $(SRCDIR)/*/*.h) \
        $(wildcard $(RSCDIR)/*.xpm) $(wildcard $(RSCDIR)/*.cpp) $(wildcard $(RSCDIR)/*.h)

# Language list
LANGSRC =Lang
LANGUAGES=$(wildcard $(LANGSRC)/*/$(EXECUTABLE).po)
LANGUAGEDIRS=$(subst $(LANGSRC)/,,$(subst /$(EXECUTABLE).po,,$(LANGUAGES)))
MOBJECTS=$(LANGUAGES:.po=.mo)

# Help files
HELPDIR  =Web/Help
HELPEXT  =htm* png jp*g ico gif css map
HELP     =$(foreach EXT,$(HELPEXT),$(wildcard $(HELPDIR)/*.$(EXT)) $(wildcard $(HELPDIR)/*/*.$(EXT)))
HELPFILES=$(subst $(HELPDIR)/,,$(HELP))

# Doc files
DOCDIR   =doc
DOC      =$(wildcard $(DOCDIR)/*.txt)
DOCFILES =$(subst $(DOCDIR)/,,$(DOC))



#############################################
##  TARGET STRUCTURE
#############################################

# Target Structure below prefix or one of the test directories
DESTDIR   =
PREFIX    =$(DESTDIR)/usr

BINDIR    =bin
DATADIR   =share
LOCALEDIR =$(DATADIR)/locale
# DATA directory is different weither it is TEST or Target
TSTDATA   =$(DATADIR)/$(EXECUTABLE)
TGTDATA   =$(DATADIR)/$(TGT_EXECUTABLE)
# Data Structure below $(TSTDATA) or $(TGTDATA)
TGTHELP   =Help


#############################################
##  TOOLS
#############################################

MSGFMT     =msgfmt
INSTALLEXE =install -D -m 755
INSTALLDAT =install -D -m 644
BUILD      =codeblocks --build --target="Release Linux"
RM         =rm -f
RMDIR      =rm -f -R


#############################################
##  BUILDING RULES
#############################################

.PHONY: uninstall clean

all: $(PRJEXEC) test langs

$(PRJEXEC): $(SOURCES)
	$(BUILD) $(EXECUTABLE).cbp

langs: $(MOBJECTS)

%.mo : %.po
	$(MSGFMT) $< -o $@

install: $(PRJEXEC) $(MOBJECTS)
	$(INSTALLEXE) $(PRJEXEC) $(PREFIX)/$(BINDIR)/$(TGT_EXECUTABLE)
	$(INSTALLDAT) Install/devices.ini $(PREFIX)/$(TGTDATA)/devices.ini
	$(INSTALLDAT) Install/SampleInterfaceOnSerialPort.xml $(PREFIX)/$(TGTDATA)/SampleInterfaceOnSerialPort.xml
	$(INSTALLDAT) Install/SampleInterfaceOnLptPort.xml $(PREFIX)/$(TGTDATA)/SampleInterfaceOnLptPort.xml
	@for i in $(HELPFILES) ; do \
	   echo "$(INSTALLDAT) $(HELPDIR)/$$i $(PREFIX)/$(TGTDATA)/$(TGTHELP)/$$i"; \
	   $(INSTALLDAT) $(HELPDIR)/$$i $(PREFIX)/$(TGTDATA)/$(TGTHELP)/$$i ; done
	@for i in $(DOCFILES) ; do \
	   echo "$(INSTALLDAT) $(DOCDIR)/$$i $(PREFIX)/$(TGTDATA)/$$i"; \
	   $(INSTALLDAT) $(DOCDIR)/$$i $(PREFIX)/$(TGTDATA)/$$i ; done
	@for i in $(LANGUAGEDIRS); do \
	   echo "$(INSTALLDAT) $(LANGSRC)/$$i/$(EXECUTABLE).mo $(PREFIX)/$(LOCALEDIR)/$$i/LC_MESSAGES/$(TGT_EXECUTABLE).mo"; \
	   $(INSTALLDAT) $(LANGSRC)/$$i/$(EXECUTABLE).mo $(PREFIX)/$(LOCALEDIR)/$$i/LC_MESSAGES/$(TGT_EXECUTABLE).mo; done
#	$(INSTALLDAT) resources/$(EXECUTABLE).png $(DATADIR)/pixmaps/$(TGT_EXECUTABLE).png
#	$(INSTALLDAT) resources/$(EXECUTABLE).desktop $(DATADIR)/applications/$(TGT_EXECUTABLE).desktop

uninstall:
	$(RM)    $(PREFIX)/$(BINDIR)/$(TGT_EXECUTABLE)
	$(RMDIR) $(PREFIX)/$(TGTDATA)
	$(RM)    $(PREFIX)/$(LOCALEDIR)/*/LC_MESSAGES/$(TGT_EXECUTABLE).mo
#	$(RM)    $(DATADIR)/pixmaps/$(TGT_EXECUTABLE).png
#	$(RM)    $(DATADIR)/applications/$(TGT_EXECUTABLE).desktop

test: $(MOBJECTS)
	@for j in $(TESTDIR); do \
	    echo "$(INSTALLDAT) Install/devices.ini $$j/$(TSTDATA)/devices.ini"; \
	    $(INSTALLDAT) Install/devices.ini $$j/$(TSTDATA)/devices.ini; \
	    echo "$(INSTALLDAT) Install/SampleInterfaceOnSerialPort.xml $$j/$(TSTDATA)/SampleInterfaceOnSerialPort.xml" ; \
	    $(INSTALLDAT) Install/SampleInterfaceOnSerialPort.xml $$j/$(TSTDATA)/SampleInterfaceOnSerialPort.xml ; \
	    echo "$(INSTALLDAT) Install/SampleInterfaceOnLptPort.xml $$j/$(TSTDATA)/SampleInterfaceOnLptPort.xml" ; \
	    $(INSTALLDAT) Install/SampleInterfaceOnLptPort.xml $$j/$(TSTDATA)/SampleInterfaceOnLptPort.xml ; \
		for i in $(HELPFILES) ; do \
		   echo "$(INSTALLDAT) $(HELPDIR)/$$i $$j/$(TSTDATA)/$(TGTHELP)/$$i"; \
		   $(INSTALLDAT) $(HELPDIR)/$$i $$j/$(TSTDATA)/$(TGTHELP)/$$i ; done; \
		for i in $(DOCFILES) ; do \
		   echo "$(INSTALLDAT) $(DOCDIR)/$$i $$j/$(TSTDATA)/$$i"; \
		   $(INSTALLDAT) $(DOCDIR)/$$i $$j/$(TSTDATA)/$$i ; done; \
	    for i in $(LANGUAGEDIRS); do \
	        echo "$(INSTALLDAT) Lang/$$i/$(EXECUTABLE).mo $$j/$(LOCALEDIR)/$$i/LC_MESSAGES/$(TGT_EXECUTABLE).mo"; \
	        $(INSTALLDAT) Lang/$$i/$(EXECUTABLE).mo $$j/$(LOCALEDIR)/$$i/LC_MESSAGES/$(TGT_EXECUTABLE).mo; done; done
#		$(INSTALLDAT) resources/$(EXECUTABLE).png $(DATADIR)/pixmaps/$(TGT_EXECUTABLE).png
#		$(INSTALLDAT) resources/$(EXECUTABLE).desktop $(DATADIR)/applications/$(TGT_EXECUTABLE).desktop


clean:
	$(RM)    $(MOBJECTS)
	$(RM)    $(EXECUTABLE)
	$(RMDIR) $(TESTDIR)/$(TSTDATA)/devices.ini
	$(RM)    $(TESTDIR)/$(LOCALEDIR)/*/LC_MESSAGES/$(TGT_EXECUTABLE).mo
	@echo "To clean object files use Build > Clean command from Code::Blocks IDE"
#		$(RM) $(DATADIR)/pixmaps/$(TGT_EXECUTABLE).png
#		$(RM) $(DATADIR)/applications/$(TGT_EXECUTABLE).desktop

#distclean: ???
