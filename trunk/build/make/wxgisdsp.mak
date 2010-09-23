DESTDIR =

version := 0.2.0
postfix := display
name := wxgis$(postfix)
program_name := $(name).so.$(version)

include wxgiscommon.mak

CXXFLAGS += -fPIC -s -O2 -c -MD
CXXFLAGS += -I../../include -I../../include/wxgis -I../../include/wxgis$(postfix)
CXXFLAGS += -I/usr/include/wxgisgdal/alg -I/usr/include/wxgisgdal/gcore -I/usr/include/wxgisgdal/port -I/usr/include/wxgisgdal/ogr -I/usr/include/wxgisgdal/frmts
CXXFLAGS += -DWXMAKINGDLL_GIS_DSP

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

