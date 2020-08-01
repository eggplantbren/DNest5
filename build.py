#!/usr/bin/env python
import os
import subprocess

BUILD_DIR = ".build"
HPP_DIR = "DNest5"

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
for filename in os.listdir(HPP_DIR):
    if filename[-4:] == ".hpp":
        source_files.append(dict(name=filename,
                              path=HPP_DIR + "/" + filename,
                              output=".build/" + filename + ".gch",
                              deps=[]))

# Add cpp files
for filename in os.listdir("."):
    if filename[-4:] == ".cpp":
        source_files.append(dict(name=filename, path=filename,
                                 output=".build/" + filename[0:-4] + ".o",
                                 deps=[]))

# Find dependencies
for source_file in source_files:
    find_dependencies(source_file, source_files)

# Open Makefile
makefile = open("Makefile", "w")
makefile.write(f"""CXX = g++
FLAGS = -std=c++17
INCLUDE = -I {HPP_DIR} -I .
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
