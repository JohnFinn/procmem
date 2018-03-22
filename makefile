default: changer target

changer: main.cpp
	g++ $^ -o $@
target: process.c
	gcc $^ -o $@
