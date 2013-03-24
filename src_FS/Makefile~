CC = g++
FLAGS = -Wall
LIB = -lpthread 
BIN = ../bin/FS/
SOURCE = ../src/
INCLUDE = ../include/
FS_OBJECTS = FS.o $(SOURCE)comunication.o $(SOURCE)utility.o 

FS : $(FS_OBJECTS)
	$(CC) $(FLAGS) $(LIB) $(FS_OBJECTS) -o $(BIN)FS
	rm -f *.o $(SOURCE)*.o


FS.o : FS0-1.cpp $(SOURCE)utility.o $(SOURCE)comunication.o 

utility.o : $(SOURCE)utility.cpp $(INCLUDE)utility.h $(INCLUDE)message_CL_DFR.h $(INCLUDE)message_FS_DFR.h

comunication.o : $(SOURCE).comunication.cpp $(INCLUDE)comunication.h

clean:
	rm -f *.o $(SOURCE)*.o 

