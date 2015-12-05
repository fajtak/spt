# Makefile na soubor pro cteni dat pro Medipix
# Lukas Fajt
# 12.5.2014

CC=g++
CPPFLAGS=`root-config --cflags` -std=c++11
EXTRALIBS=`root-config --libs`

LIBS+= -lSpectrum
LIBS+= -lMinuit

NAME=SPT
PROGRAM=sptAnalyzer.cpp
SOURCES=frameHolder.cpp frame.cpp cluster.cpp noisyMask.cpp calibration.cpp
OBJECTS=$(patsubst %.cpp, %.o, $(SOURCES))
HEADERS=$(patsubst %.cpp, %.h, $(SOURCES))

.PHONY: build
.PHONY: clean

build: $(NAME)

clean:
	rm -f *.o $(NAME)

$(NAME): $(PROGRAM) $(OBJECTS)
	$(CC) $(CPPFLAGS) -o $(NAME) $(PROGRAM) $(OBJECTS) $(EXTRALIBS) $(LIBS)

# hromadné nastavení závislostí, žádná akce
${OBJECTS}: ${HEADERS}

