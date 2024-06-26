CXX = g++
FLAGS = -O3 -march=native -std=c++2a -Wall -Wextra -pedantic
INCLUDE = -I . -I ./include

default:
	$(CXX) $(FLAGS) $(INCLUDE) -c src/CommandLineOptions.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c src/Database.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c src/Levels.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c src/Misc.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c src/ParameterNames.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c src/Particle.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c src/Options.cpp
	ar rcs libdnest5.a *.o
	$(CXX) $(FLAGS) $(INCLUDE) -c main.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c postprocess.cpp
	$(CXX) -pthread -L . -o main main.o -lpthread -lsqlite3 -ldnest5 -lyaml-cpp
	$(CXX) -pthread -L . -o postprocess postprocess.o -lpthread -lsqlite3 -ldnest5 -lyaml-cpp
	rm -f *.o

