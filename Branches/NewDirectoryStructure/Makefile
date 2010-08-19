WXCONFIG = wx-config
CPP = `$(WXCONFIG) --cxx`
CXXFLAGS= `$(WXCONFIG) --cxxflags` -O2
LDFLAGS = `$(WXCONFIG) --libs`
RC = `$(WXCONFIG) --rescomp`
#RC = x86_64-w64-mingw32-windres --define WX_CPU_AMD64
RCFLAGS = `$(WXCONFIG) --cxxflags`
MSGFMT = msgfmt

SOURCES= $(wildcard src/*.cpp)\
			$(wildcard src/*/*.cpp)\
			$(wildcard resources/*.cpp)

RESOURCES= resources/resource.rc
RESOURCE_OBJ=$(RESOURCES:.rc=.o)
OBJECTS=$(SOURCES:.cpp=.o)
MOBJECTS=$(LANGUAGES:.po=.mo)

LANGUAGEDIRS=fr
LANGUAGES=$(wildcard Lang/*/WxPic.po)
EXECUTABLE=WxPic
EXECUTABLE_WIN=WxPic.exe

DESTDIR		=
PREFIX		= $(DESTDIR)/usr
BINDIR	    = $(PREFIX)/bin
DATADIR	    = $(PREFIX)/share
LOCALEDIR   = $(DATADIR)/locale

all: $(SOURCES) $(EXECUTABLE) langs

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(OBJECTS) $(LDFLAGS) -o $@

win: $(SOURCES) $(RESOURCES) $(EXECUTABLE_WIN) langs

$(EXECUTABLE_WIN): $(OBJECTS) $(RESOURCE_OBJ)
	$(CPP) $(OBJECTS) $(RESOURCE_OBJ) $(LDFLAGS) -static-libgcc -o $@

langs: $(MOBJECTS)

%.mo : %.po
	$(MSGFMT) $< -o $@

%.o : %.rc
	$(RC) $(RCFLAGS) $(CXXFLAGS)  $< -o $@

.cpp.o:
	$(CPP) $(CXXFLAGS) -Isrc -c $< -o $@

install:
	install -D -m 755 $(EXECUTABLE) $(BINDIR)/$(EXECUTABLE)
	install -D -m 644 resources/$(EXECUTABLE).png $(DATADIR)/pixmaps/$(EXECUTABLE).png
	install -D -m 644 resources/$(EXECUTABLE).desktop $(DATADIR)/applications/$(EXECUTABLE).desktop
	@for i in $(LANGUAGEDIRS); do \
	   echo "install -D -m 644 Lang/$$i/$(EXECUTABLE).mo $(LOCALEDIR)/$$i/LC_MESSAGES/$(EXECUTABLE).mo"; \
	   install -D -m 644 Lang/$$i/$(EXECUTABLE).mo $(LOCALEDIR)/$$i/LC_MESSAGES/$(EXECUTABLE).mo; done

uninstall:
	rm $(BINDIR)/$(EXECUTABLE)
	rm $(DATADIR)/pixmaps/$(EXECUTABLE).png
	rm $(DATADIR)/applications/$(EXECUTABLE).desktop
	rm $(LOCALEDIR)/*/LC_MESSAGES/$(EXECUTABLE).mo

test:
	cat $(LANGUAGEDIRS)

clean:
	rm -f src/*.o
	rm -f src/*/*.o
	rm -f resources/*.o
	rm -f Lang/*/$(EXECUTABLE).mo
	rm -f $(EXECUTABLE)
	rm -f $(EXECUTABLE_WIN)
	rm -rf $(EXECUTABLE).app

distclean: clean
