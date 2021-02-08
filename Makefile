CXX = g++
FLAGS = -O3 -march=native -std=c++2a -Wall -Wextra -pedantic
INCLUDE = -I . -I ./include

default:
	$(CXX) $(FLAGS) $(INCLUDE) -c src/CommandLineOptions.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c src/Database.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c src/Particle.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c src/Options.cpp
	ar rcs libdnest5.a *.o
	rm -f *.o

