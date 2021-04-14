OBJ = main.o
all: main
pipe: $(OBJ)
	gcc $(OBJ) -o main
.PHONY: clean
clean:
	rm -f *.o main
