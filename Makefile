CXX = g++
CXXFLAGS = -Ofast -pthread -std=c++11 -g -w
FastHCADecoder: *.cpp *.h
	$(CXX) -o clHCA $(CXXFLAGS) *.cpp

clean:
	rm clHCA
