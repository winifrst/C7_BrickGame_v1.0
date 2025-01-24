CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror
LDFLAGS = -lncurses
CHECKFLAGS = -pthread -lcheck -lrt -lm -lsubunit

BACKEND_SRC = $(wildcard brick_game/tetris/*.c)
TEST_SRC = $(wildcard test/*.c)

all: clean tetris

tetris: tetris_lib.a
	mkdir -p build
	$(CC) $(CFLAGS) -o build/tetris $(wildcard gui/cli/*.c) tetris_lib.a $(LDFLAGS)
	rm -f tetris_lib.a

tetris_lib.a:
	$(CC) -c $(CFLAGS) $(BACKEND_SRC)
	ar rcs tetris_lib.a *.o
	rm -f *.o

test: tetris_test.a
	$(CC) --coverage $(TEST_SRC) -o test_runner tetris_test.a $(CHECKFLAGS)
	./test_runner

tetris_test.a:
	$(CC) -c $(CFLAGS) --coverage $(BACKEND_SRC)
	ar rcs tetris_test.a *.o
	rm -f *.o

gcov_report: test
	rm -rf gcov_report
	mkdir -p gcov_report
	gcov -r backend.gcda
	geninfo --no-external --output-file coverage.info .
	genhtml coverage.info --output-directory gcov_report
	rm -f tetris test_runner tetris_lib.a tetris_test.a *.o *.gcno *.gcda *.gcov coverage.info

install: clean tetris
	rm -rf install
	mkdir -p install
	@mv build/tetris install
	make clean

uninstall:
	-rm -f install/tetris score.txt

# sudo apt install texlive-full
# sudo apt-get install doxygen
dvi:
	@doxygen Doxyfile > /dev/null 2>&1
	@cd dvi/latex && latex -silent refman.tex > /dev/null 2>&1
	mv dvi/latex/refman.dvi dvi/
	rm -rf dvi/latex

run_dvi: dvi
	xdvi dvi/refman.dvi

dist:
	rm -rf dist/
	mkdir -p dist
	tar cvzf dist/BrickGame_1.0.tgz brick_game gui test FSM_tetris.jpeg Doxyfile Makefile mainpage.dox

play: tetris
	./build/tetris

clean:
	rm -f build/tetris test_runner tetris_lib.a tetris_test.a *.o *.gcno *.gcda *.gcov coverage.info
	rm -rf gcov_report valgrind-out.txt dvi/* q.log
	rm -f test/.tetris_tests.c.swp

cppcheck: 
	cppcheck --enable=all --suppress=missingIncludeSystem brick_game/tetris/*.c brick_game/*.c gui/cli/*.c tests/*.c