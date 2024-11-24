
SRC = Method.hpp Method.cpp main.cpp
APP = app.exe

all:
	g++ $(SRC) -o $(APP) -std=gnu++20 -g
