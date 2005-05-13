run: runtests
	./runtests

runtests: runtests.cpp
	g++ -Wall -I.. -I../cxxtest -o runtests runtests.cpp

runtests.cpp: *.h
	../cxxtest/cxxtestgen.pl --error-printer -o runtests.cpp *.h
