CFLAGS=-Wall -Wextra -std=c99 -O0 -g $(shell pkg-config --cflags check)
CHECK_LIBS=$(shell pkg-config --libs check)

.PHONEY : all
.PHONEY : test
.PHONEY : clean
.PHONEY : check

all: rpn test

rpn: main.o rpnlib.o
	@gcc $(CFLAGS) -o rpn main.o rpnlib.o

main.o: main.c rpnlib.h
	@gcc $(CFLAGS) -c main.c

rpnlib.o: rpnlib.c rpnlib.h
	@gcc $(CFLAGS) -c rpnlib.c

test: rpn-test
	@CK_VERBOSITY="normal" ./rpn-test
	@echo "--------------"

check: rpn-test
	@CK_FORK=no  CK_VERBOSITY="normal" valgrind --leak-check=full --show-leak-kinds=all -v ./rpn-test
	-@echo "------------"

rpn-test: rpnlib-test.o rpnlib.o
	@gcc -o rpn-test rpnlib-test.o rpnlib.o $(CHECK_LIBS)

rpnlib-test.o: rpnlib.o rpnlib-test.c
	@gcc $(CFLAGS) -Wno-sequence-point -c rpnlib-test.c

clean:
	-@rm rpn rpnlib.o main.o rpn-test rpnlib-test.o
