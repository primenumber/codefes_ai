OBJS=main.o gamestate.o board.o line.o eval.o search.o
CXXFLAGS=-std=c++17 -Og -g -march=native -mtune=native
LDFLAGS=-lboost_timer -lboost_system

ai:$(OBJS)
	g++ -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

%.o: %.cpp
	g++ -c -o $@ $(CXXFLAGS) $<
