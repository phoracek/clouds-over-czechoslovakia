# Current state of the skies in black and white

# License

Boundary lines in this project are taken from <https://www.naturalearthdata.com> where they were kindly rpesented as public domain.

Satelite images are taken from [EUMETSAT](https://www.eumetsat.int/) [MTG GeoColour RGB](https://view.eumetsat.int/productviewer?v=mtg_fd:rgb_geocolour#), credits should go to NASA.

All the code under this repository is distributed under the terms of the MIT license. See [LICENSE](LICENSE) for details.

# GIS notes

Satelite images are taken from <https://view.eumetsat.int/productviewer?v=default#>, using it's WMS API and 'GeoColour RGB - MTG - 0 degree' visualization.

Download the needed tooling:

```sh
sudo dnf install gdal
```

Get info about the source:

```sh
gdalinfo https://view.eumetsat.int/geoserver/wms\?service\=WMS\&version\=1.3.0 | grep 71
```

Download specific part of the map as GeoTiff (can be opened in QGis):

```sh
gdal_translate -of GTiff -outsize 1000 0  "WMS:https://view.eumetsat.int/geoserver/ows?SERVICE=WMS&VERSION=1.3.0&REQUEST=GetMap&LAYERS=mtg_fd%3Argb_geocolour&CRS=CRS:84&BBOX=10.2777938842773,48.3506393432617,20.2807235717773,52.3563919067383&FORMAT=image/jpeg&TILESIZE=256&OVERVIEWCOUNT=17&MINRESOLUTION=0.0000053644180298&TILED=true" out.tif
```

Download boundary lines from <https://www.naturalearthdata.com/downloads/110m-cultural-vectors/>.

Open the boundary lines in QGIS. Create a new scratch layer for polygons. Draw a square over central Europe. Create a new scratch layer from intersection of the polygon and boundaries. Export it as a SHP file.

Add the boundaries to the downloaded GeoTiff:

```sh
gdal_rasterize -b 1 -b 2 -b 3 -burn 255 -burn 255 -burn 255 -l ne_50m_admin_0_boundary_lines_land ne_50m_admin_0_boundary_lines_land.shp out.tif
```

Transform to the spherical mercator projection:

```sh
gdalwarp -t_srs ESRI:53004 -r lanczos -wo SOURCE_EXTRA=1000 -co COMPRESS=LZW out.tif out_mercator.tif
```

Convert it to a JPG:

```sh
gdal_translate -of JPEG -scale out_mercator.tif out_mercator.jpg
```
