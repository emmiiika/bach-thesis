#!/bin/bash
# checks if every .hpp and .h file can be included and compiled on its own
# (i.e. if we have not missed some #includes in our own code)

for f in $(find ../include -name '*.hpp' -or -name '*.h'); do
	echo "Processing '$f':"
	echo "#include <impl/basic/include.hpp>" > tmp.cpp
	echo "#include <$f>" >> tmp.cpp
	echo "int main() {}" >> tmp.cpp
	g++ -MM -std=c++17 -fconcepts -fopenmp -I../include -O0 tmp.cpp -o tmp.out -lcryptominisat5
	cat tmp.out
	rm tmp.cpp tmp.out
	echo
done
