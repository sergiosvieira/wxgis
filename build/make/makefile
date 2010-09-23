DESTDIR =

bin_dir := ../../tmp

all: core geom ds frw dsp carto cat cartoui catui gp gpui
	make -f wxgis.mak

core:
	make -f wxgiscore.mak

geom:
	make -f wxgisgeom.mak

ds:
	make -f wxgisds.mak

dsp:
	make -f wxgisdsp.mak

frw:
	make -f wxgisfrw.mak

cat:
	make -f wxgiscat.mak

catui:
	make -f wxgiscatui.mak

carto:
	make -f wxgiscarto.mak

cartoui:
	make -f wxgiscartoui.mak

gp:
	make -f wxgisgp.mak

gpui:
	make -f wxgisgpui.mak

clean:
	make -f wxgis.mak clean
	make -f wxgiscore.mak clean
	make -f wxgisgeom.mak clean
	make -f wxgisds.mak clean
	make -f wxgisdsp.mak clean
	make -f wxgisfrw.mak clean
	make -f wxgiscat.mak clean
	make -f wxgiscatui.mak clean
	make -f wxgiscarto.mak clean
	make -f wxgiscartoui.mak clean
	make -f wxgisgp.mak clean
	make -f wxgisgpui.mak clean
	-rmdir -p --ignore-fail-on-non-empty $(bin_dir)

install_head:
	-mkdir -p $(bin_dir)/usr/include
	cp -R ../../include/wxgis $(bin_dir)/usr/include/

.PHONY: clean
