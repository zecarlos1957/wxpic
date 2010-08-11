# Copyright (c) 2010 Erdem U. Altinyurt
# This file and all modifications and additions to the wxpic
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments to spamjunkeater gmail.com

# norootforbuild

%define _prefix	/usr

Name:			wxpic
Summary:		A program to repair broken AVI file streams by rebuilding index part of file
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

%changelog
* Wed Aug 8 2010 Erdem U. Altinyurt <spamjunkeater@gmail.com> - 1.2-0
- initial release of rpm
