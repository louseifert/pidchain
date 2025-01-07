compiler=g++
cflags=-O3  -Wpedantic -fsanitize=address -g -fstack-protector -Wextra -Wall -Wextra
builddir=bin
C_version=c++23
file=main
ofile='pidchain'

make: 		
	${compiler} ${cflags} ${file}.cpp -std=${C_version} -I ./include/ -o ${builddir}/${ofile}


lint:
	cppcheck main.cpp; cpplint main.cpp 

format:
	clang-format -i main.cpp


test:
	g++ -O3 -fprofile-arcs -ftest-coverage -Wpedantic -fsanitize=address -g -fstack-protector -Wextra -Wall -Wextra -std=c++23 -I ./include/ -I /usr/src/googletest/googletest/include/ test_parser.cpp -o build/test_parser -lgtest -lgtest_main

coverage:
	rm documentation/html/coverage.html
	gcovr --html documentation/html/coverage.html
	gcovr


documentation:
	doxygen ConfigParser_doxygen.cfg

clean:
	rm ${builddir}/* -rf

all: clean format lint test make
