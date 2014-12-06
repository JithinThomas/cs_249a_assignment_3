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

clean:
	rm -f dense_nm_* manual_*txt sparse_nm_*txt client-auto-network-sim client-manual-network-sim *.o *~

always:
