#!/bin/bash
# create multiresolution windows icon
ICON_SRC=../../src/qt/res/icons/bitcoin.png
ICON_DST=../../src/qt/res/icons/bitcoin.ico
convert ${ICON_SRC} -resize 16x16 feathercoin-16.pn
convert ${ICON_SRC} -resize 32x32 feathercoin-32.png
convert ${ICON_SRC} -resize 48x48 feathercoin-48.png
convert feathercoin-16.png feathercoin-32.png feathercoin-48.png ${ICON_DST}

