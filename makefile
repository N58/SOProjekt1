OBJ = main.o linkedList.o copy.o dirChecker.o utils.o
all: projekt
projekt: $(OBJ)
	gcc $(OBJ) -o projekt
$(OBJ): linkedList.h copy.h dirChecker.h utils.h
.PHONY: clean
clean:
	rm -f *.o projekt
