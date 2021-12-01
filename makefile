
CXX = g++
CXXFLAG = -std=c++11





Lab3 : main.cpp src/SP.o
	$(CXX) $(CXXFLAG) -o $@ $^ 


src/SP.o : src/SP.cpp header/SP.hpp


clean:
	rm -f Lab3 src/SP.o

.phony : clean
