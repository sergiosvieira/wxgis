CPPFLAGS += -DWXMAKINGDLL_GIS_CORE 

g++ -Wall  -g -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -D_USRDLL -DWXMAKINGDLL_GIS_CORE  -g -I/usr/lib/wx/include/gtk2-unicode-release-2.8 -I/usr/include/wx-2.8 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D__WXGTK__ -pthread      -I../../include  -c /home/bishop/projects/wxGIS/src/core/config.cpp -o ../obj/src/core/config.o


