#!/bin/bash
# @author       iota square <i2> 
# @date         30-09-2019
# @file         .travis.sh
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

echo "### Initiating Build Process"

echo "-- C compilers available"
ls -1 tools/ARM_GNU/Linux64/bin/arm-none-eabi-*
echo "----"

echo "=== Building Debug"
./make clean && ./make
mkdir ../DEBUG
cp -r build/ ../DEBUG
./make get_mem_map > ../DEBUG/mem_map.log
cat ../DEBUG/mem_map.log

echo "=== Building Release"
./make clean && ./make RELEASE=yes
mkdir ../RELEASE
cp -r build/ ../RELEASE
./make get_mem_map RELEASE=yes > ../RELEASE/mem_map.log
cat ../RELEASE/mem_map.log

# *********************** (C) COPYRIGHT iota2 ************END OF FILE**********

