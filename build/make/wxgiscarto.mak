DESTDIR =

version := 0.2.0
postfix := carto
name := wxgis$(postfix)
program_name := $(name).so.$(version)

include wxgiscommon.mak

CXXFLAGS += -fPIC -s -O2 -c -MD
CXXFLAGS += -I../../include -I../../include/wxgis -I../../include/wxgis$(postfix)
CXXFLAGS += -I/usr/include/wxgisgdal/alg -I/usr/include/wxgisgdal/gcore -I/usr/include/wxgisgdal/port -I/usr/include/wxgisgdal/ogr -I/usr/include/wxgisgdal/frmts
CXXFLAGS += -DWXMAKINGDLL_GIS_CRT

source_dirs  := ../../src/$(postfix)
search_wildcards := $(addsuffix /*.cpp,$(source_dirs))
obj_dir := ./obj/$(postfix)
dst_dir := ../../tmp/usr/lib/wxgis

all: create_out $(program_name)

create_out:
	-mkdir -p $(obj_dir)
	-mkdir -p $(dst_dir)

$(program_name): $(notdir $(patsubst %.cpp,%.o,$(wildcard $(search_wildcards))))	
	$(CXX) -shared $(addprefix $(obj_dir)/,$^) -o $(dst_dir)/$@ `wx-config --libs`

VPATH := $(source_dirs)
     
%.o: %.cpp
	$(CXX) $(CXXFLAGS) `wx-config --cxxflags` $(addprefix -I,$(source_dirs)) -o $(obj_dir)/$@ $<

include $(wildcard *.d)

clean:
	-rm -f ./$(obj_dir)/*.o ./$(obj_dir)/*.d
	-rmdir -p --ignore-fail-on-non-empty ./$(obj_dir)
	-rm -f $(dst_dir)/$(program_name)

.PHONY: clean

#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CRT  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/alg -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts  -c /home/bishop/projects/wxGIS/src/carto/cartocmd.cpp -o ../obj/src/carto/cartocmd.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CRT  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/alg -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts  -c /home/bishop/projects/wxGIS/src/carto/featurelayer.cpp -o ../obj/src/carto/featurelayer.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CRT  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/alg -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts  -c /home/bishop/projects/wxGIS/src/carto/map.cpp -o ../obj/src/carto/map.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CRT  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/alg -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts  -c /home/bishop/projects/wxGIS/src/carto/mapview.cpp -o ../obj/src/carto/mapview.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CRT  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/alg -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts  -c /home/bishop/projects/wxGIS/src/carto/rasterlayer.cpp -o ../obj/src/carto/rasterlayer.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CRT  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/alg -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts  -c /home/bishop/projects/wxGIS/src/carto/rasterrenderer.cpp -o ../obj/src/carto/rasterrenderer.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CRT  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/alg -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts  -c /home/bishop/projects/wxGIS/src/carto/simplerenderer.cpp -o ../obj/src/carto/simplerenderer.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CRT  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/alg -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts  -c /home/bishop/projects/wxGIS/src/carto/tableview.cpp -o ../obj/src/carto/tableview.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CRT  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/alg -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts  -c /home/bishop/projects/wxGIS/src/carto/transformthreads.cpp -o ../obj/src/carto/transformthreads.o
#g++ -shared  ../obj/src/carto/cartocmd.o ../obj/src/carto/featurelayer.o ../obj/src/carto/map.o ../obj/src/carto/mapview.o ../obj/src/carto/rasterlayer.o ../obj/src/carto/rasterrenderer.o ../obj/src/carto/simplerenderer.o ../obj/src/carto/tableview.o ../obj/src/carto/transformthreads.o   -o ../debug/wxGISCartod.so -pthread -Wl,-Bsymbolic-functions  -lwx_gtk2u_richtext-2.8 -lwx_gtk2u_aui-2.8 -lwx_gtk2u_xrc-2.8 -lwx_gtk2u_qa-2.8 -lwx_gtk2u_html-2.8 -lwx_gtk2u_adv-2.8 -lwx_gtk2u_core-2.8 -lwx_baseu_xml-2.8 -lwx_baseu_net-2.8 -lwx_baseu-2.8   -lcurl 

