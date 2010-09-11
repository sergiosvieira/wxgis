#wxgis makefile
#PROJ = wxgis
#CC = g++
#CXX = c++
#EXECUTABLE = wxcat

CXX ?= g++
CXXFLAGS += -D__UNIX__ -DWXUSINGDLL -D_USRDLL
CXXFLAGS += -I../../include/
CXXFLAGS += -Wall

#include wxgiscore.mak
#include 

#clean:
#  rm *.o *~
  
  

#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -DNOPCH  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/alg -I../../include/wxgis  -c /home/bishop/projects/wxGIS/src/cat_app/catalogapp.cpp -o ../obj/src/cat_app/catalogapp.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -DNOPCH  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/alg -I../../include/wxgis  -c /home/bishop/projects/wxGIS/src/cat_app/catalogframe.cpp -o ../obj/src/cat_app/catalogframe.o
#g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -DNOPCH  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include -I/home/bishop/projects/gdal-1.7.2/ogr -I/home/bishop/projects/gdal-1.7.2/ogr/ogrsf_frmts -I/home/bishop/projects/gdal-1.7.2/port -I/home/bishop/projects/gdal-1.7.2/gcore -I/home/bishop/projects/gdal-1.7.2/alg -I../../include/wxgis  -c /home/bishop/projects/wxGIS/src/cat_app/gisaboutdlg.cpp -o ../obj/src/cat_app/gisaboutdlg.o
#g++  -o ../debug/wxGISCatd ../obj/src/cat_app/catalogapp.o ../obj/src/cat_app/catalogframe.o ../obj/src/cat_app/gisaboutdlg.o   -pthread -Wl,-Bsymbolic-functions  -lwx_gtk2u_richtext-2.8 -lwx_gtk2u_aui-2.8 -lwx_gtk2u_xrc-2.8 -lwx_gtk2u_qa-2.8 -lwx_gtk2u_html-2.8 -lwx_gtk2u_adv-2.8 -lwx_gtk2u_core-2.8 -lwx_baseu_xml-2.8 -lwx_baseu_net-2.8 -lwx_baseu-2.8   -lcurl   "/home/bishop/projects/gdal-1.7.2/wxgis/bin/cpld.so" "/home/bishop/projects/gdal-1.7.2/wxgis/bin/ogrd.so" "/home/bishop/projects/gdal-1.7.2/wxgis/bin/gdald.so" ../debug/wxGISCored.so /usr/lib/libcurl.so /usr/lib/libgeos_c.so /usr/lib/libproj.so ../debug/wxGISCartod.so ../debug/wxGISCartoUId.so ../debug/wxGISCatalogd.so ../debug/wxGISCatalogUId.so ../debug/wxGISDataSourced.so ../debug/wxGISDisplayd.so ../debug/wxGISFrameworkd.so ../debug/wxGISGeometryd.so 


