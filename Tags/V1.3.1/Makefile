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
OBJ_DIR =obj/$(RELEASE)

#############################################
##  SOURCE FILES
#############################################

# Graphic resources
RSCDIR    =resources
SRCPIXMAP =$(RSCDIR)/$(TGT_EXECUTABLE).png
SRCDESKTOP=$(RSCDIR)/$(TGT_EXECUTABLE).desktop

# Version
SRC_VERSION=src/version.autogen
MNF_VERSION=manifest.autogen
TAR_VERSION=TarVersion.autogen
ALL_VERSION=$(SRC_VERSION) $(MNF_VERSION) $(TAR_VERSION)
SVN_STATUS =SVN.status.depend
SVN_INFO   =SVN.info.depend
MNF_SOURCE =resources/model.manifest

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
OBJECTS   =$(foreach SOURCE,$(SOURCES),$(OBJ_DIR)/$(subst .cpp,.o,$(SOURCE)))
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
TGTMAPS   =$(subst $(HELPDIR),$(PREFIX)/$(TGTDATA)/$(TGTHELP),$(MAPFILES))
TSTMAPS   =$(subst $(HELPDIR),$(PRJDIR)/$(TSTDATA)/$(TGTHELP),$(MAPFILES))
TGDOCFILES=$(subst $(DOCDIR),$(PREFIX)/$(TGTDATA),$(DOC))
TSDOCFILES=$(subst $(DOCDIR),$(PRJDIR)/$(TSTDATA),$(DOC))
# Tar Version
KNOWN_OS  =centos Ubuntu
OS_VERSION=$(shell uname -rv)
OS_NAME   =$(strip $(foreach OS, $(KNOWN_OS), $(findstring $(OS), $(OS_VERSION))))
TAR_MAKEFILE=TarMakefile.depend


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
BUILD      =$(CXX)
LIBS       =`$(WXCONFIG) --libs` -shared-libgcc
else
BUILD      =codeblocks --build --target="Release Linux"
endif
CPPC       =$(CXX) `$(WXCONFIG) --cxxflags` -O2 -Isrc -DUSE_SVN_VERSION
RM         =rm -f
RMDIR      =rm -f -R
SAXON_JAR  =$(firstword $(strip $(wildcard /usr/share/java/saxonb.jar) $(wildcard /usr/share/java/saxon9he.jar)))
XSLT_PROC  =java -classpath $(SAXON_JAR) net.sf.saxon.Transform  -ext:on
XSLT_IN    =-s:
XSLT_SHEET =-xsl:
XSLT_MAP   =$(XSLT_PROC) $(XSLT_SHEET)Map2Redirect.xslt


#############################################
##  BUILDING RULES
#############################################

.PHONY: uninstall clean


all: $(PRJEXEC) langs

ifeq ($(CODEBLOCKS),NO)

$(OBJ_DIR):
	$(MKDIR) $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.cpp $(SRC_VERSION) $(OBJ_DIR)
	$(MKDIR) $(dir $@)
	$(CPPC) -c $< -o $@

$(PRJEXEC): $(OBJECTS)
	$(MKDIR) $(PRJXEDIR)
	$(BUILD) $(OBJECTS) -o $@ $(LIBS)
	
.SECONDARY: $(OBJECTS)

else

$(PRJEXEC): $(SOURCES) $(SRC_VERSION)
	$(BUILD) $(EXECUTABLE).cbp
	
endif


langs: $(MOBJECTS)

%.mo: %.po
	$(MSGFMT) $< -o $@

## Version files
##################

$(SVN_INFO): $(SOURCES) $(LANSRC) $(SRCPIXMAP) $(SRCDESKTOP) $(HELPFILES) $(MAPFILES) $(DOC)
	svn info --xml . > $@
	
$(SVN_STATUS): $(SOURCES) $(LANSRC) $(SRCPIXMAP) $(SRCDESKTOP) $(HELPFILES) $(MAPFILES) $(DOC)
	svn status --xml -v . > $@

$(ALL_VERSION): $(SVN_STATUS) $(SVN_INFO)
	$(XSLT_PROC) $(XSLT_SHEET)SVN2Version.xslt $(XSLT_IN)$(SVN_INFO) \
					STATUS_FILE=$(CURDIR)/$(SVN_STATUS) \
					MANIFEST_FILE=$(CURDIR)/$(MNF_SOURCE) \
					MANIFEST_OUTPUT=$(CURDIR)/$(MNF_VERSION) \
					VERSION_SCRIPT=$(CURDIR)/$(TAR_VERSION) > $(SRC_VERSION)


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


uninstall:
	$(RM)    $(PREFIX)/$(BINDIR)/$(TGT_EXECUTABLE)
	$(RMDIR) $(PREFIX)/$(TGTDATA)
	$(RM)    $(TGLANGFILE)
	$(RM)    $(TGTPIXMAP) $(TGTDESKTOP)

	
## TAR rules
##################

include $(TAR_MAKEFILE)

$(TAR_MAKEFILE): $(TAR_VERSION) Makefile
	@echo "Creating $(TAR_MAKEFILE) with TAR version=`cat $(TAR_VERSION)`"
	@echo "TAR_DATE=\$$(shell cat $(TAR_VERSION))" > $(TAR_MAKEFILE)
	@echo "TAR_FILENAME=$(EXECUTABLE)-$(OS_NAME)-\$$(TAR_DATE)-bin" >> $(TAR_MAKEFILE)
	@echo "tar: \$$(VERSION_SCRIPT) \$$(TAR_FILENAME).tar.gz" >> $(TAR_MAKEFILE)
	@echo " " >> $(TAR_MAKEFILE)
	@echo "\$$(TAR_FILENAME).tar.gz: $(TAR_MAKEFILE) \$$(PREFIX)/\$$(BINDIR)/\$$(TGT_EXECUTABLE) \$$(TGTFILES) \$$(TGTHFILES) \$$(TGTMAPS) \$$(TGDOCFILES) \$$(TGLANGFILE) \$$(TGTPIXMAP) \$$(TGTDESKTOP) \$$(TAR_VERSION)" >> $(TAR_MAKEFILE)
	@printf "\t tar -cf \$$(TAR_FILENAME).tar --directory=$(PREFIX) $(BINDIR)/$(TGT_EXECUTABLE) $(TGTDATA) \\" >> $(TAR_MAKEFILE)
	@printf "\n\t \$$(subst \$$(PREFIX)/,,\$$(TGLANGFILE)) \\" >> $(TAR_MAKEFILE)
	@printf "\n\t $(DATADIR)/pixmaps/$(TGT_EXECUTABLE).png \\" >> $(TAR_MAKEFILE)
	@printf "\n\t $(DATADIR)/applications/$(TGT_EXECUTABLE).desktop" >> $(TAR_MAKEFILE)
	@printf "\n\t $(RM) \$$(TAR_FILENAME).tar.gz" >> $(TAR_MAKEFILE)
	@printf "\n\t gzip \$$(TAR_FILENAME).tar\n" >> $(TAR_MAKEFILE)

	
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

