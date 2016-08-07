CC=gcc
BIN=bin/
SRC=src/
COMMON_OPTS=-Iinc/ -o $@ $(DEBUG) -Wall
BIN_OPTS=$(COMMON_OPTS) -c $^
PROG_OPTS=$(COMMON_OPTS) $^
ALL_DEPENDS=$(BIN)main.o $(BIN)lex.o $(BIN)tok.o $(BIN)resmap.o $(BIN)string_t.o

interp: $(ALL_DEPENDS)
	$(CC) $(PROG_OPTS)

$(BIN)main.o: $(SRC)main.c
	$(CC) $(BIN_OPTS)

$(BIN)lex.o: $(SRC)lex.c
	$(CC) $(BIN_OPTS)

$(BIN)tok.o: $(SRC)tok.c
	$(CC) $(BIN_OPTS)


$(BIN)resmap.o: $(SRC)resmap.c
	$(CC) $(BIN_OPTS)

$(BIN)string_t.o: $(SRC)string_t.c
	$(CC) $(BIN_OPTS)
