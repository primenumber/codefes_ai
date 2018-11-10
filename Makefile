OBJS=main.o gamestate.o board.o line.o eval.o search.o utils.o
CXXFLAGS=-std=c++17 -O3 -march=native -mtune=native
LDFLAGS=-lboost_timer -lboost_system

ai:$(OBJS)
	g++ -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

clean:
	-rm *.o
	-rm ai

%.o: %.cpp
	g++ -c -o $@ $(CXXFLAGS) $<
