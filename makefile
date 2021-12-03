
CXX = g++
CXXFLAG = -std=c++11 -g
DEBUG = NODEBUG
CPPFLAG = -D $(DEBUG)




Lab3 : main.cpp src/SP.o
	$(CXX) $(CPPFLAG) $(CXXFLAG)  -o $@ $^ 


src/SP.o : src/SP.cpp header/SP.hpp


clean:
	rm -f Lab3 src/SP.o

.phony : clean
