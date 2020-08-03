#!/usr/bin/env python
import os
import subprocess

BUILD_DIR = ".build"
HPP_DIRS = ["DNest5", "Examples"]

def find_dependencies(source_file, source_files):
    """
    Find the dependencies and put them in the source_file dict.
    """
    f = open(source_file["path"])
    lines = f.readlines()
    f.close()

    includes =  [line for line in lines if "#include" in line]
    for include in includes:
        for other in source_files:
            if other["name"] in include:
                if other["name"][-4:] == ".hpp":
                    source_file["deps"].append(other["output"])

# Get the hpp files
source_files = []
hpps = [d + "/" + f for d in HPP_DIRS for f in os.listdir(d)]
hpps = [hpp for hpp in hpps if hpp[-4:] == ".hpp"]
for hpp in hpps:
    name = hpp.split("/")[1]
    source_file = dict(name=name, path=hpp)
    source_file["output"] = ".build/" + name + ".gch"
    source_file["deps"] = []
    source_files.append(source_file)

# Add cpp files from current directory
cpps = [f for f in os.listdir(".") if f[-4:] == ".cpp"]
for cpp in cpps:
    source_files.append(dict(name=cpp, path=cpp,
                             output=".build/" + cpp[0:-4] + ".o",
                             deps=[]))

# Find dependencies
for source_file in source_files:
    find_dependencies(source_file, source_files)

## String for include part of Makefile
#INCLUDES = "".join(["-I " + s + " " for s in HPP_DIRS + ["."]])

# Open Makefile
makefile = open("Makefile", "w")
makefile.write(f"""CXX = g++
FLAGS = -std=c++17
INCLUDE = -I .
OPTIM = -O2 -g
WARN = -Wall -Wextra -pedantic
LINK = -lpthread -lsqlite3 -lyaml-cpp
ALL = $(FLAGS) $(INCLUDE) $(WARN) $(OPTIM)\n\n""")

makefile.write("default: main\n\n")
makefile.write("main: .build/main.o")
makefile.write("\n\t$(CXX) $(ALL) -pthread -o main .build/main.o $(LINK)\n\n")

# Create Make targets
for source_file in source_files:
    line = source_file["output"] + ": "
    line += source_file["path"]
    for dep in source_file["deps"]:
        line += " " + dep
    makefile.write(line + "\n")
    makefile.write(f"\t$(CXX) $(ALL) -o {source_file['output']} -c {source_file['path']}")
    makefile.write("\n\n")

makefile.write("clean:\n")
makefile.write("\t rm main .build/*\n")

makefile.close()

subprocess.run("make")
