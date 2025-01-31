compiler=g++
cflags=-O3  -Wpedantic -fsanitize=address -g -fstack-protector -Wextra -Wall -Wextra
testcflags=-ftest-coverage ${cflags}
builddir=bin
C_version=c++23
file=main
ofile=pidchain
lintfilter=-whitespace/comments,-whitespace/blank_line,-readability/todo,-build/header_guard,-whitespace/indent
gcoverops=--exclude include/test_expect_equals.h --exclude test_parser.cpp --exclude-noncode-lines --exclude-unreachable-branches
gcoverdir=--gcov-object-directory gcovrdir
includes=-I include/ -I ConfigParser/include/
testinclude= -I /usr/src/googletest/googletest/include/
make: 		
	${compiler} ${cflags} ${file}.cpp -std=${C_version} ${includes} -o ${builddir}/${ofile}


lint:
	cppcheck main.cpp; 
	cpplint --filter=${lintfilter} main.cpp 

format:
	clang-format -i main.cpp


test:
	${compiler} ${cflags} ${testcflags} -std=${C_version} ${includes} ${testinclude} test_${ofile}.cpp -o build/${ofile}_test -lgtest -lgtest_main

coverage:
	rm documentation/html/coverage.html
	gcovr ${gcoverops} ${gcoverdir} --html documentation/html/coverage.html
	gcovr ${gcoverops} ${gcoverdir}


documentation:
	doxygen ConfigParser_doxygen.cfg

clean:
	rm ${builddir}/* -rf

all: clean format lint test make
