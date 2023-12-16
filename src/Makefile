CC=					g++ 
CFLAGS=				-Wall -Werror -Wextra -std=c++17
CTESTFLAGS=			-std=c++17
GCOVFLAGS=			-fprofile-arcs -ftest-coverage
SOURSES=	./controller/controller.h \
	./model/graph_layer.cc \
	./model/graph_layer.h \
	./model/graph_neuron.cc \
	./model/graph_neuron.h \
	./model/graph_perceptron.cc \
	./model/graph_perceptron.h \
	./model/model.cc \
	./model/model.h \
	./model/neural_network.cc \
	./model/neural_network.h
OBJECT=			$(patsubst %*.cc, %*.o, $(SOURSES))
SOURSES_TEST=		test/test.cc
VALGRIND_FLAGS = 	--trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all --verbose
OS=					$(shell uname -s)

ifeq ($(OS), Linux)
  CHECKFLAGS=	-lgtest -lm -lpthread -lrt
endif
ifeq ($(OS), Darwin)
  CFLAGS+=		-D OS_MAC
  CHECKFLAGS=	-lgtest
endif

ifeq ($(OS), Linux)
  APP = build/perceptron
endif
ifeq ($(OS), Darwin)
  APP = build/perceptron.app
endif

all: build

check:
	cp ../materials/linters/.clang-format ./
	clang-format -n */*.cc
	clang-format -n */*.h
#	rm -rf .clang-format

cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem *.c *.h

build: clean
	mkdir build
	cd ./build/ && qmake ../view/ && make

open: build
	./build/perceptron

controller.a: $(OBJECT)
ifeq ($(OS), Darwin)
	$(CC) $(CTESTFLAGS) -c $(SOURSES)
endif
	ar -rcs controller.a $(OBJECT)
	@rm -f *.o

tests: clean test/test.cc controller.a
	$(CC) $(CTESTFLAGS) $(SOURSES_TEST) $(SOURSES) -o run $(CHECKFLAGS) -lgcov --coverage
	./run

gcov_report: tests
	lcov -t "test" -o report.info --no-external -c -d .
	genhtml -o report report.info
ifeq ($(OS), Linux)
	xdg-open ./report/index.html
else
	open ./report/index.html
endif


clean:
	@rm -rf ./a.out ./*.o ./*.a ./GcovReport ./report
	@rm -rf *.aux *.dvi *.log *.out *.pdf
	@rm -rf build/
	@rm -rf 	*.gcno \
			run \
			*.gcda \
			*.info \
			model/*.o \
			model/*.gch \
			*.txt \
			../perceptron \
			../perceptron.tar \
			./controller/*.gch

valgrind_test: tests
	CK_FORK=no valgrind $(VALGRIND_FLAGS) --log-file=RESULT_VALGRIND.txt ./run

leaks_test: tests
	leaks -atExit -- ./run

install: build
	@mkdir $(HOME)/perceptron
	@cp -rf $(APP) $(HOME)/perceptron/
	make clean

uninstall:
	@rm -rf $(HOME)/perceptron/

dvi:
	texi2dvi perceptron.tex

open_dvi: dvi
	evince -w perceptron.dvi

pdf:
	texi2pdf perceptron.tex

dist: build
ifeq ($(OS), Linux)
	tar -cf ../perceptron.tar build
endif
ifeq ($(OS), Darwin)
	rm -rf ./build/*.dmg
	hdiutil create -volname "Perceptron" -srcfolder ./build/Perceptron.app -ov -format UDRW perceptron.dmg
	mv perceptron.dmg $(HOME)/Desktop/
endif