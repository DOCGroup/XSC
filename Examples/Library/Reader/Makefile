CXX := g++ -Wall -O3
XSC := ../../../xsc
XSCRT := ../../../XSCRT

vpath %.cpp ..
vpath %.hpp ..
vpath %.xsd ..

driver: library.o driver.o dom.o
	$(CXX) -o $@ $^ -lxerces-c

library.o: library.cpp
driver.o: driver.cpp library.hpp
dom.o: dom.cpp dom.hpp

%.cpp %.hpp %.ipp: %.xsd $(XSC)
	$(XSC) --cxx-generate-traversal-types -- $<

%.o: %.cpp
	$(CXX) -I$(XSCRT) -c $< -o $@
