#!/bin/bash
# @date         07-09-2019
# @author       iota square <i2> 
#  _       _        ___  
# (_)     | |      |__ \.
#  _  ___ | |_ __ _   ) |
# | |/ _ \| __/ _` | / / 
# | | (_) | || (_| |/ /_ 
# |_|\___/ \__\__,_|____|
#
# @License      GNU GPU v3
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Free Software, Hell Yeah!
#

scriptname=${0##*/}
scriptdir=${0%*$scriptname}

# Rerun the script with environment cleared out
if [ "$1" != "ENVCLEARED" ]; then
    exec -c "$scriptdir$scriptname" ENVCLEARED "$PATH" "$HOME" "$@"
    exit
else
    shift
    export SAVED_PATH=$1
    shift
    export SAVED_HOME=$1
    shift
fi
# Delete some automatic bash environment variables
unset PATH
unset TERM
unset PWD
unset OLDPWD
unset SHLVL


linux32unamestr=`"${scriptdir}tools/common/Linux32/uname" 2> /dev/null`
linux64unamestr=`"${scriptdir}tools/common/Linux64/uname" 2> /dev/null`
osxunamestr=`"${scriptdir}tools/common/OSX/uname"  2> /dev/null`
win32unamestr=`"${scriptdir}tools/common/Win32/uname.exe" -o 2> /dev/null`

if [ "$linux32unamestr" == "Linux" ] || [ "$linux64unamestr" == "Linux" ]; then
# Linux
linuxuname64str=`${scriptdir}tools/common/Linux64/uname -m 2> /dev/null`
if [ "$linuxuname64str" == "x86_64" ]; then

# Linux64
#echo Host is Linux64
"${scriptdir}tools/common/Linux64/make" "$@" HOST_OS=Linux64

else

# Linux32
#echo Host is Linux32
"${scriptdir}tools/common/Linux32/make" "$@" HOST_OS=Linux32

fi
elif [[ "$osxunamestr" == *Darwin* ]]; then

#OSX
#echo Host is OSX
"${scriptdir}tools/common/OSX/make" "$@" HOST_OS=OSX

elif [ "${win32unamestr}" == "MinGW" ]; then

#MinGW / Cygwin
#echo Host is MinGW
"${scriptdir}tools/common/Win32/make.exe" "$@" HOST_OS=Win32

else
echo Unknown host
echo Linux32 uname: \"${linux32unamestr}\"
echo Linux64 uname: \"${linux32unamestr}\"
echo OSX uname: \"${osxunamestr}\"
echo Win32 uname: \"${win32unamestr}\"
fi

# *********************** (C) COPYRIGHT iota2 ************END OF FILE**********

