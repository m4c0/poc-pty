test.exe: test.cpp
	clang++ -std=c++20 -o $@ $<

