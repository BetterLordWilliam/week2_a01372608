.RECIPEPREFIX = >
TARGET = ./build/rvloop
SRC = ./event-loop/my_event_loop.c

$(TARGET): $(SRC)
> @mkdir -p ./build
> gcc -o $(TARGET) $(SRC)

run: $(TARGET)
> $(TARGET)

clean:
> rm $(TARGET)

