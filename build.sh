#!/bin/bash
# spmenu build script
PREFIX="${PREFIX:-/usr}"
DESTDIR="${DESTDIR:-}"
INCDIR="${INCDIR:-/usr/include}"
makebin="${makebin:-$(command -v make)}"
cc="${cc:-${CC:-gcc}}"
opt="${opt:-${OPT:--O2}}"
warn="${warn:-true}"

check_dist() {
    [ -f "/etc/pacman.conf" ] && [ "$warn" != "false" ] && printf "hint: detected Pacman. if you want you can run 'makepkg' with proper arguments to install it using pacman.\n" && pacman=true
    [ -f "/System/Library/CoreServices/SystemVersion.plist" ] && [ "$warn" != "false" ] && printf "hint: detected a Mac. Please report any issues you find as it is untested.\n" && mac=true
    uname -a | grep -q OpenBSD && printf "hint: detected OpenBSD. Please report any issues you find as it is untested.\n" && openbsd=true
}

check() {
    if [ "$mac" != "true" ]; then
    [ ! -x "$(command -v ldconfig)" ] && printf "ldconfig not found in %s. Please make sure your system is set up properly." "\$PATH" && exit 1
    [ ! -x "$(command -v make)" ] && printf "make not found in %s. Please make sure your system is set up properly." "\$PATH" && exit 1
    [ ! -x "$(command -v "$cc")" ] && printf "%s not found in %s. Please make sure your system is set up properly." "$cc" "\$PATH"
    [ -n "$(ldconfig -p | grep Imlib2)" ] && printf "Imlib2 found\n" && imlib2=true || imlib2=false
    [ -n "$(ldconfig -p | grep libXft)" ] && printf "libXft found\n" && xft=true || xft=false
    [ -n "$(ldconfig -p | grep libX11)" ] && printf "libX11 found\n" && x11=true || x11=false
    [ -n "$(ldconfig -p | grep libXrender)" ] && printf "libXrender found\n" && xrender=true || xrender=false
    [ -n "$(ldconfig -p | grep libpango)" ] && printf "libpango found\n" && pango=true || pango=false
    [ -n "$(ldconfig -p | grep libpangoxft)" ] && printf "libpangoxft found\n" && pangoxft=true || pangoxft=false
    [ -n "$(ldconfig -p | grep libxcb-xinerama)" ] && printf "libxcb-xinerama found\n" && xinerama=true || xinerama=false
    [ -n "$(ldconfig -p | grep libgnutls-openssl)" ] && printf "openssl found\n" && openssl=true || openssl=false
    [ -n "$(ldconfig -p | grep fribidi)" ] && printf "fribidi found\n" && fribidi=true || fribidi=false
    [ -n "$(ldconfig -p | grep freetype)" ] && printf "freetype found\n" && freetype=true || freetype=false
    [ -n "$(ldconfig -p | grep libconfig)" ] && printf "libconfig found\n" && libconfig=true || libconfig=false
    else
    makebin="gnumake"
    GEN_MANUAL="false"
    PREFIX="/usr/local"
    fi
}

loadconf() {
    [ -x "buildconf" ] && source buildconf

    # mandatory deps
    [ "$freetype" = "false" ] && printf "Freetype not found. Install it.\n" && exit 1
    [ "$openssl" = "false" ] && [ "$imlib2" = "true" ] && printf "OpenSSL not found. Install it.\n" && exit 1
    [ "$xrender" = "false" ] && printf "libXrender not found. Install it.\n" && exit 1
    [ "$x11" = "false" ] && printf "libX11 not found. Install it.\n" && exit 1
    [ "$xft" = "false" ] && printf "libXft not found. Install it.\n" && exit 1
}

build() {
    # to be overriden
    FREETYPEINC="$INCDIR/freetype2"
    X11INC="/usr/X11R6/include"
    X11LIB="/usr/X11R6/lib"

    # macOS
    if [ "$mac" = "true" ]; then
        INCDIR="/usr/local/include"
        FREETYPEINC="$INCDIR/freetype2"
        X11INC="/opt/X11/include"
        X11LIB="/opt/X11/lib"
    fi

    # openbsd
    if [ "$openbsd" = "true" ]; then
        INCDIR="$X11INC"
        FREETYPEINC="$INCDIR/freetype2"
    fi

    # xinerama
    if [ "$xinerama" = "true" ]; then
        xineramalib="-lXinerama"
        xineramatoggle="-DXINERAMA"
    fi

    # imlib2
    if [ "$imlib2" = "true" ]; then
        imlib2libs="-lImlib2"
        imlib2toggle="-DIMAGE"
    fi

    # openssl
    if [ "$openssl" = "true" ]; then
        opensslconf="openssl"
    fi

    # pango
    if [ "$pango" = "true" ] && [ "$pangoxft" = "true" ]; then
        pangoconf="pango"
        pangoxftconf="pangoxft"
        pangotoggle="-DPANGO"
    fi

    # libconfig
    if [ "$libconfig" = "true" ]; then
        libconfigtoggle="-DCONFIG"
        libconfigconf="libconfig"
    fi

    # fribidi
    if [ "$fribidi" = "true" ]; then
        bdlibs="-lfribidi"
        bdinc="$INCDIR/fribidi"
        bdtoggle="-DRTL"
    else
        bdlibs=""
        bdinc=""
        bdtoggle=""
    fi

    $makebin clean
    [ "$GEN_MANUAL" != "false" ] && [ -x "$(command -v pandoc)" ] && $makebin man

    $makebin \
        CC="$cc" \
        PREFIX="$PREFIX" \
        DISTDIR="$DISTDIR" \
        OPT="$opt" \
        XINERAMALIBS="$xineramalib" \
        XINERAMATOGGLE="$xineramatoggle" \
        IMLIB2LIBS="$imlib2libs" \
        IMLIB2TOGGLE="$imlib2toggle" \
        PANGOCONF="$pangoconf" \
        PANGOXFTCONF="$pangoxftconf" \
        PANGOTOGGLE="$pangotoggle" \
        BDLIBS="$bdlibs" \
        BDINC="$bdinc" \
        BDTOGGLE="$bdtoggle" \
        FREETYPEINC="$FREETYPEINC" \
        OPENSSLCONF="$opensslconf" \
        LIBCONFIGTOGGLE="$libconfigtoggle" \
        LIBCONFIGCONF="$libconfigconf" \
        X11LIB="$X11LIB" \
        X11INC="$X11INC"
}

install() {
    if [ "$mac" = "true" ]; then
        RULE="install_mac"
    else
        RULE="install"
    fi

    $makebin $RULE \
        CC="$cc" \
        PREFIX="$PREFIX" \
        OPT="$opt" \
        XINERAMALIBS="$xineramalib" \
        XINERAMATOGGLE="$xineramatoggle" \
        IMLIB2LIBS="$imlib2libs" \
        IMLIB2TOGGLE="$imlib2toggle" \
        PANGOCONF="$pangoconf" \
        PANGOXFTCONF="$pangoxftconf" \
        PANGOTOGGLE="$pangotoggle" \
        BDLIBS="$bdlibs" \
        BDINC="$bdinc" \
        BDTOGGLE="$bdtoggle" \
        FREETYPEINC="$FREETYPEINC" \
        OPENSSLCONF="$opensslconf" \
        LIBCONFIGTOGGLE="$libconfigtoggle" \
        LIBCONFIGCONF="$libconfigconf" \
        X11LIB="$X11LIB" \
        X11INC="$X11INC"
}

[ "$1" = "--no-install" ] && install=false

check_dist
check
loadconf
build
[ "$install" != "false" ] && install
