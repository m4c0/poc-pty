all: test.exe
	./$<

test.exe: test.cpp
	clang++ -std=c++20 -o $@ $<

.PHONY: all
