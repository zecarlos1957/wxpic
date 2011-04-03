# Copyright (c) 2010 Erdem U. Altinyurt
# This file and all modifications and additions to the wxpic
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments to wxpic.free.fr

# norootforbuild

%define _prefix	/usr

Name:			wxpic
Summary:		Programmer of PIC microcontrollers driving serial or parallel port adapters
Version:		1.2.1_r186
Release:		1
License:		GPL
Group:			Development
URL:			http://wxpic.free.fr
Source0:		%{name}-%{version}-src.tar.bz2
BuildRoot:		%{_tmppath}/%{name}-%{version}-build
#BuildRequires:		dos2unix
BuildRequires:		gcc-c++
BuildRequires:		pkgconfig
BuildRequires:		wxGTK-devel

%description
This program is wx port of well known WinPic program for programming PIC microcontrollers.

wxpic supports CLI for make it work with other programs.

wxpic is writen with cross-platform API, wxWidgets.

%prep
%setup -q -n wxpic

#dos2unix     docs/*
#%__chmod 644 docs/*

%build
%__make %{?jobs:-j%{jobs}} \
	WXCONFIG=wx-config

%install
%makeinstall

%find_lang %{name}

%clean
[ -d "%{buildroot}" -a "%{buildroot}" != "" ] && %__rm -rf "%{buildroot}"

%files -f %{name}.lang
%defattr(-,root,root)
#%doc docs/*
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/%{name}/Author.txt
%{_datadir}/%{name}/DocWriter.txt
%{_datadir}/%{name}/Help/Adapter.htm
%{_datadir}/%{name}/Help/ChipRead.png
%{_datadir}/%{name}/Help/ChipReload.png
%{_datadir}/%{name}/Help/ChipVerify.png
%{_datadir}/%{name}/Help/ChipWrite.png
%{_datadir}/%{name}/Help/Code.png
%{_datadir}/%{name}/Help/ConfigMem.png
%{_datadir}/%{name}/Help/ConfigPic.png
%{_datadir}/%{name}/Help/CustomInterface.htm
%{_datadir}/%{name}/Help/Data.png
%{_datadir}/%{name}/Help/Faq.htm
%{_datadir}/%{name}/Help/Interface.png
%{_datadir}/%{name}/Help/LargeLogo.png
%{_datadir}/%{name}/Help/LptPort.htm
%{_datadir}/%{name}/Help/Menu.htm
%{_datadir}/%{name}/Help/Messages.png
%{_datadir}/%{name}/Help/Open.png
%{_datadir}/%{name}/Help/Options.png
%{_datadir}/%{name}/Help/Panel.htm
%{_datadir}/%{name}/Help/Save.png
%{_datadir}/%{name}/Help/TaitSchema.png
%{_datadir}/%{name}/Help/Top.png
%{_datadir}/%{name}/Help/WxPic.css
%{_datadir}/%{name}/Help/WxPic.ico
%{_datadir}/%{name}/Help/WxPic.png
%{_datadir}/%{name}/Help/com84.gif
%{_datadir}/%{name}/Help/devlist_format.htm
%{_datadir}/%{name}/Help/fr/ConfigMem.png
%{_datadir}/%{name}/Help/fr/ConfigPic.png
%{_datadir}/%{name}/Help/fr/Faq.htm
%{_datadir}/%{name}/Help/fr/Interface.png
%{_datadir}/%{name}/Help/fr/Menu.htm
%{_datadir}/%{name}/Help/fr/Messages.png
%{_datadir}/%{name}/Help/fr/Options.png
%{_datadir}/%{name}/Help/fr/Panel.htm
%{_datadir}/%{name}/Help/fr/WxPic.png
%{_datadir}/%{name}/Help/fr/index.htm
%{_datadir}/%{name}/Help/fr/wxHelp.map
%{_datadir}/%{name}/Help/index.htm
%{_datadir}/%{name}/Help/noppp-x.gif
%{_datadir}/%{name}/Help/noppp.gif
%{_datadir}/%{name}/Help/pip84.gif
%{_datadir}/%{name}/Help/wxHelp.map
%{_datadir}/%{name}/License.txt
%{_datadir}/%{name}/SampleInterfaceOnLptPort.xml
%{_datadir}/%{name}/SampleInterfaceOnSerialPort.xml
%{_datadir}/%{name}/Translator.txt
%{_datadir}/%{name}/devices.ini

%changelog
* Wed Aug 8 2010 Erdem U. Altinyurt <spamjunkeater@gmail.com> - 1.2-0
- initial release of rpm
