program_name := wxGISGeometry.so

include wxgis.mak
CXXFLAGS += -DWXMAKINGDLL_GIS_CORE 

source_dirs  := ../../src/geometry
search_wildcards := $(addsuffix /*.cpp,$(source_dirs))
obj_dir := ../obj/debug/geometry
dst_dir := ../debug

all: create_obj_out $(program_name)

create_obj_out:
	-mkdir -p $(obj_dir)
	-mkdir -p $(dst_dir)
test:
	@echo $(notdir $(patsubst %.cpp,%.o,$(wildcard $(search_wildcards))))

$(program_name): $(obj_dir)/$(notdir $(patsubst %.cpp,%.o,$(wildcard $(search_wildcards))))
	$(CXX) -shared $^ -o $(dst_dir)/$@ `wx-config --libs`

VPATH := $(source_dirs)
     
$(obj_dir)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) `wx-config --cflags` -c -MD $(addprefix -I,$(source_dirs)) $< -o $@

include $(wildcard *.d) 

clean:
	rm -f $(obj_dir)/*.o $(obj_dir)/*.d
	rmdir -p --ignore-fail-on-non-empty $(obj_dir)

.PHONY: clean

