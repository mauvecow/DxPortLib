#!/bin/sh

libtoolize
aclocal -I autotools
automake --foreign --add-missing
autoconf
