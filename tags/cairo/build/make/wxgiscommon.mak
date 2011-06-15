CXX ?= g++
CC ?= gcc
CXXFLAGS += -D__UNIX__ -D__WXGTK__ -DWXUSINGDLL -DNDEBUG -DSTRICT -D_USRDLL 
CXXFLAGS += -Wall -g -O2 -fPIC -MD 
LDFLAGS += -s 
#   
