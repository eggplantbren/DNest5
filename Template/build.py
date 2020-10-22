#!/usr/bin/env python
import subprocess
import yaml

MAKEFILE = """DNEST5_DIR = {dnest5_dir}

default:
	g++ -std=c++17 -O3 -march=native -I $(DNEST5_DIR) -c *.cpp
	g++ -pthread -o main main.o -lpthread -lsqlite3 -lyaml-cpp
	g++ -pthread -o postprocess postprocess.o -lsqlite3 -lyaml-cpp
	rm -f *.o
"""

f = open("config.yaml")
dnest5_dir = yaml.load(f, Loader=yaml.SafeLoader)["dnest5_dir"]
f.close()

f = open("Makefile", "w")
f.write(MAKEFILE.format(dnest5_dir=dnest5_dir))
f.close()

subprocess.run("make")

