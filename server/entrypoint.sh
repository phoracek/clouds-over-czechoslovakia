#!/usr/bin/env bash

set -xe

SOURCE_DIR=$(dirname $0)
TARGET=$1

${SOURCE_DIR}/gis/fetch_and_process.sh /tmp/test.jpg
ditherer /tmp/test.jpg /tmp/test-dithered.png /tmp/test-dithered.bitmap
