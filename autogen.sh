#!/bin/sh
autoheader
aclocal -I m4
automake -ac
autoconf
