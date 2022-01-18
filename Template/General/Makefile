DNEST5_DIR = /home/brewer/Projects/DNest5
FLAGS = -O3 -march=native -std=c++2a -Wall -Wextra -pedantic
INCLUDE = -I $(DNEST5_DIR) -I $(DNEST5_DIR)/include

default:
	$(CXX) $(FLAGS) $(INCLUDE) -c main.cpp
	$(CXX) $(FLAGS) $(INCLUDE) -c postprocess.cpp
	$(CXX) -pthread -L $(DNEST5_DIR) -o main main.o -lpthread -lsqlite3 -lyaml-cpp -ldnest5
	$(CXX) -pthread -L $(DNEST5_DIR) -o postprocess postprocess.o -lpthread -lsqlite3 -lyaml-cpp -ldnest5
	rm -f *.o

