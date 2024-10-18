#!/usr/bin/env bash

set -xe

SOURCE_DIR=$(dirname $0)
BUILD_DIR=/tmp/eumetsat_mtg_geocolour_rgb_build
TARGET=$1

mkdir -p ${BUILD_DIR}

rm -f ${BUILD_DIR}/eumetsat.tif
gdal_translate -of GTiff -outsize 1000 0  "WMS:https://view.eumetsat.int/geoserver/ows?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&LAYERS=mtg_fd%3Argb_geocolour&CRS=CRS:84&BBOX=11.080,46.900,23.549,51.850&FORMAT=image/jpeg&TILESIZE=256&OVERVIEWCOUNT=17&MINRESOLUTION=0.0000053644180298&TILED=true" ${BUILD_DIR}/eumetsat.tif

rm -f ${BUILD_DIR}/eumetsat_boundaries.tif
cp ${BUILD_DIR}/eumetsat.tif ${BUILD_DIR}/eumetsat_boundaries.tif
gdal_rasterize -b 1 -b 2 -b 3 -burn 255 -burn 255 -burn 255 -l ne_10m_admin_0_boundary_lines_land_central_europe ${SOURCE_DIR}/naturalearthdata_boundary_lines_central_europe/ne_10m_admin_0_boundary_lines_land_central_europe.shp ${BUILD_DIR}/eumetsat_boundaries.tif

rm -f ${BUILD_DIR}/eumetsat_boundaries_mercator.tif
gdalwarp -t_srs ESRI:53004 -r lanczos -wo SOURCE_EXTRA=1000 -co COMPRESS=LZW ${BUILD_DIR}/eumetsat_boundaries.tif ${BUILD_DIR}/eumetsat_boundaries_mercator.tif

rm -f ${BUILD_DIR}/eumetsat_boundaries_mercator.jpg
gdal_translate -of JPEG -scale ${BUILD_DIR}/eumetsat_boundaries_mercator.tif ${BUILD_DIR}/eumetsat_boundaries_mercator.jpg

cp ${BUILD_DIR}/eumetsat_boundaries_mercator.jpg ${TARGET}
