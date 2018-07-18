#!/bin/bash


if [ "x$1" == "x" ];
then
    echo "usage:$0 logo_file"
    exit 0
fi

if [ -f "$1" ]
then
    echo $1
else
    echo "no find file [$1]"
    exit 0
fi

name=${1%%.*}

source_bmp=$1
target_ppm=logo.ppm

echo $name
echo "$1"

echo "create $name.pnm"
bmptopnm $1 > $name.pnm

echo "create $name.clut224.pnm"
pnmquant 224 $name.pnm > $name.clut224.pnm

echo "create $name.ppm"
pnmtoplainpnm $name.clut224.pnm > $name.ppm

cp $name.ppm ../kernel/drivers/video/logo/logo_rota_clut224.ppm

echo "create logo.ppm success,please rebuild kernel."



