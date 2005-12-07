Summary: SNOBOL4 - StriNg Oriented and symBOlic Language
Name: snobol4
Version: 1.1
Release: 1
Copyright: distributable
Group: Development/Languages
Source: ftp://ftp.snobol4.org/snobol/snobol4-1.1.tar.gz
URL: http://www.snobol4.org/csnobol4/

%description
A port of the original Macro implementation of SNOBOL4
with SPITBOL and SNOBOL4+ enhancements.

Powerful patterns are first class objects.

%prep
%setup
./configure

%build
make

%install
make install

%files
/usr/local/bin/snobol4
/usr/local/lib/snobol4
/usr/local/man/man1/snobol4.1
/usr/local/man/man3/snolib.3
/usr/local/man/man3/snobol4dbm.3
/usr/local/man/man3/snobol4tcl.3
