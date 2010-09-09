#wxgis makefile
PROJ = wxgis
CC = g++
CXX = c++
#EXECUTABLE = wxcat

include wxgiscore.mak
include 

clean:
  rm *.o *~
  
  