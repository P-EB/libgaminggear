%define vmajor 0
%define vminor 15
%define vmicro 1

Name: libgaminggear
Version: %{vmajor}.%{vminor}.%{vmicro}
Release: 1
License: GPL
Source: %{name}-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-build
BuildRequires: cmake >= 2.6.4
BuildRequires: gtk2-devel >= 2.20
BuildRequires: sqlite >= 3.7
BuildRequires: libcanberra-devel
BuildRequires: libnotify-devel
BuildRequires: doxygen
BuildRequires: gettext >= 0.15
ExclusiveOS: linux
URL: http://sourceforge.net/projects/libgaminggear/
Packager: Stefan Achatz <erazor_de@users.sourceforge.net>

Summary: Libraries for gaming input devices
Group: System Environment/Libraries

%package devel
Requires: libgaminggear
Summary: Development headers for libgaminggear.
Group: Development/Libraries

%description
Libgaminggear provides functions and widgets common for gaming input devices.

%description devel
Libgaminggear provides functions and widgets common for gaming input devices.
Here are the development headers needed to compile software that uses libgaminggear.

%prep
%setup -q -n %{name}-%{version}
%{__mkdir} build
cd build
cmake -DCMAKE_INSTALL_PREFIX="%{_prefix}" -DCMAKE_BUILD_TYPE=Release -DINSTALL_LIBDIR="%{_libdir}" -DWITH_DOC=TRUE ..

%build
cd build
%{__make} -r %{?_smp_mflags}

%install
cd build
%{__rm} -rf "%{buildroot}"
%{__make} -r %{?_smp_mflags} DESTDIR="%{buildroot}" install

%clean
%{__rm} -rf "%{buildroot}"

%post
/sbin/ldconfig
touch --no-create %{prefix}/share/icons/hicolor &>/dev/null || :

%postun
/sbin/ldconfig
if [ $1 -eq 0 ] ; then
  touch --no-create %{prefix}/share/icons/hicolor &>/dev/null
  gtk-update-icon-cache %{prefix}/share/icons/hicolor &>dev/null || :
fi

%posttrans
gtk-update-icon-cache %{prefix}/share/icons/hicolor &>dev/null || :

%files
%defattr(-,root,root)
%doc INSTALL README COPYING Changelog
%{_libdir}/libgaminggear*
%{_datadir}/libgaminggear/icons
%{_bindir}/gaminggearfxinfo
%{_bindir}/gaminggearfxcontrol
%{_datadir}/locale/*/LC_MESSAGES/libgaminggear.mo

%files devel
%defattr(-,root,root)
%{_includedir}/gaminggear-%{vmajor}
%{_datadir}/doc/libgaminggear/html
%{_datadir}/pkgconfig/gaminggear-%{vmajor}.pc
%{_datadir}/cmake/Modules/FindGAMINGGEAR%{vmajor}.cmake

%changelog
* Fri Jun 09 2016 Stefan Achatz <erazor_de@users.sourceforge.net> 0.15.1-1
- Initial version
