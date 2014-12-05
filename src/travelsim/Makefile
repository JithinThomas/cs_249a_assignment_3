SRC=../../src
CPPFLAGS = -I$(SRC)
CXX = g++
CXXFLAGS = \
    -g -std=c++11 \
    -Wall \
    -Wno-unused-function

all: client-auto-network-sim client-manual-network-sim

client-auto-network-sim: always
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o client-auto-network-sim $(SRC)/travelsim/client-auto-network-sim.cxx

client-manual-network-sim: always
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o client-manual-network-sim $(SRC)/travelsim/client-manual-network-sim.cxx

client-new-1: always
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o client-new-1 $(SRC)/travelsim/client-new-1.cxx

client1: always
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o client1 $(SRC)/travelsim/client1.cxx

client2: always
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o client2 $(SRC)/travelsim/client2.cxx

client3: always
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o client3 $(SRC)/travelsim/client3.cxx

whacksim:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o whacksim $(SRC)/travelsim/whacksim.cxx

whacksim-queued:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o whacksim-queued $(SRC)/travelsim/whacksim-queued.cxx

clean:
	rm -f client1 client2 client3 *.o *~

always:
