CC = g++ -std=c++17
CFLAGS  = -g -Wall
RM = rm

OPENCV = `pkg-config --cflags --libs opencv4`

LIBS = $(OPENCV)

all: main run

main: myGMM_2.o
	$(CC) $(CFLAGS) -o myGMM_2 -lX11 myGMM_2.o $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)

run:
	./myGMM_2
	
clean:
	rm *.o myGMM_2