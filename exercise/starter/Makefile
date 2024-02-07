CFLAGS = -ggdb -Wall -Wextra -std=c++17 -O0
# You can use any standard version between C++11 or C++20 you like

all: test 

test: Base.cc ConsolListUpdaters.cc InputOutput.cc Test.cc
	g++ $(CFLAGS) -o $@ $^

clean:
	rm -rf MatchingEngine.o ConsolListUpdaters.o InputOutput.o Test.o test test.dSYM
