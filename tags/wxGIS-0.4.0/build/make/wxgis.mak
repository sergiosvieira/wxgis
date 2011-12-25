DESTDIR =

version := 0.2.0
postfix := cat_app
program_name := wxgiscat-$(version)

include wxgiscommon.mak

CXXFLAGS += -I../../include -I../../include/wxgis -I../../include/wxgis$(postfix)
CXXFLAGS += -I/usr/include/wxgisgdal/alg -I/usr/include/wxgisgdal/gcore -I/usr/include/wxgisgdal/port -I/usr/include/wxgisgdal/ogr -I/usr/include/wxgisgdal/frmts

source_dirs  := ../../src/$(postfix)
search_wildcards := $(addsuffix /*.cpp,$(source_dirs))
obj_dir := ./obj/$(postfix)
dst_dir := ../../tmp/usr/bin

all: create_out $(program_name)

create_out:
	-mkdir -p $(obj_dir)
	-mkdir -p $(dst_dir)

$(program_name): $(notdir $(patsubst %.cpp,%.o,$(wildcard $(search_wildcards)))) 
	$(CXX) $(addprefix $(obj_dir)/,$^) -o $(dst_dir)/$@ $(LDFLAGS) `wx-config --libs` /usr/lib/libcurl.so /usr/lib/libgeos_c.so /usr/lib/libproj.so /usr/lib/wxgis/wxgiscpl.so /usr/lib/wxgis/wxgisogr.so /usr/lib/wxgis/wxgisgdal.so /usr/lib/wxgis/wxgiscore.so /usr/lib/wxgis/wxgiscarto.so /usr/lib/wxgis/wxgiscartoui.so /usr/lib/wxgis/wxgiscatalog.so /usr/lib/wxgis/wxgiscatalogui.so /usr/lib/wxgis/wxgisdatasource.so /usr/lib/wxgis/wxgisdisplay.so /usr/lib/wxgis/wxgisframework.so /usr/lib/wxgis/wxgisgeometry.so  /usr/lib/wxgis/wxgisgeoprocessing.so  /usr/lib/wxgis/wxgisgeoprocessingui.so

VPATH := $(source_dirs)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) `wx-config --cxxflags` $(addprefix -I,$(source_dirs)) -o $(obj_dir)/$@ -c $<

include $(wildcard *.d)

clean:
	-rm -f ./$(obj_dir)/*.o ./$(obj_dir)/*.d
	-rmdir -p --ignore-fail-on-non-empty ./$(obj_dir)
	-rm -f $(dst_dir)/$(program_name)

.PHONY: clean


