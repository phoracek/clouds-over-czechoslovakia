#!/usr/bin/env bash

set -xe

SOURCE_DIR=$(dirname $0)
TARGET=${1:-/tmp}

${SOURCE_DIR}/gis/fetch_and_process.sh ${TARGET}/view.jpg
ditherer ${TARGET}/view.jpg ${TARGET}/view-dithered.png ${TARGET}/view-dithered.bitmap
date +%s > ${TARGET}/timestamp.txt
