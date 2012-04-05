CXX = clang++

palette: palette.cpp
	$(CXX) -Wall -ansi -pedantic -std=c++0x -L/usr/X11R6/lib -lm -lpthread -lX11 -o palette palette.cpp

clean:
	rm palette
