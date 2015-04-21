#!/bin/bash


export TARGET_OSM_BIN=/home/sk/work/osm/target
OSM_PKG_CONFIG_PATH=${TARGET_OSM_BIN}/lib/pkgconfig

echo "Target dir: $TARGET_OSM_BIN"
echo "Target pkg dir: $OSM_PKG_CONFIG_PATH"

mkdir -p $TARGET_OSM_BIN

#build illumination
if [ ! -f ${OSM_PKG_CONFIG_PATH}/illumination.pc ]; then
	echo "Build illumination"
	#svn co https://illumination.svn.sourceforge.net/svnroot/illumination/trunk/Illumination illumination
	cd 3rdparty/illumination
	./autogen.sh && ./configure --prefix=$TARGET_OSM_BIN
	make -j8 && make install
	cd ../..
fi

#build libosmscout
if [ ! -f ${OSM_PKG_CONFIG_PATH}/libosmscout.pc ]; then
	echo "Build libosmscout"
	cd libosmscout
	./autogen.sh && PKG_CONFIG_PATH=${OSM_PKG_CONFIG_PATH} ./configure --prefix=$TARGET_OSM_BIN
	make -j8 && make install
	cd ..
fi

#build libosmscout-import
if [ ! -f ${OSM_PKG_CONFIG_PATH}/libosmscout-import.pc ]; then
	echo "Build libosmscout-import"
	cd libosmscout-import
	./autogen.sh && PKG_CONFIG_PATH=${OSM_PKG_CONFIG_PATH} ./configure --prefix=$TARGET_OSM_BIN
	make -j8 && make install
	cd ..
fi

#build libosmscout-map
if [ ! -f ${OSM_PKG_CONFIG_PATH}/libosmscout-map.pc ]; then
	echo "Build libosmscout-map"
	cd libosmscout-map
	./autogen.sh && PKG_CONFIG_PATH=${OSM_PKG_CONFIG_PATH} ./configure --prefix=$TARGET_OSM_BIN
	make -j8 && make install
	cd ..
fi

#build Import
#if [ ! -f ${OSM_PKG_CONFIG_PATH}/libosmscout-map.pc ]; then
	echo "Build Import"
	cd Import
	./autogen.sh && PKG_CONFIG_PATH=${OSM_PKG_CONFIG_PATH} ./configure --prefix=$TARGET_OSM_BIN
	make -j8 && make install
	cd ..
#fi
#build libosmscout-map-qt
if [ ! -f ${OSM_PKG_CONFIG_PATH}/libosmscout-map-qt.pc ]; then
	echo "Build libosmscout-map-qt"
	cd libosmscout-map-qt
	./autogen.sh && PKG_CONFIG_PATH=${OSM_PKG_CONFIG_PATH} ./configure --prefix=$TARGET_OSM_BIN
	make -j8 && make install
	cd ..
fi
