
FLAGS = -Wall -Wextra -fsanitize=address,undefined -g
LIBS = -lSDL2

SOURCE_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
SRC = $(wildcard $(SOURCE_DIR)/*.c)
OBJ = $(SRC:$(SOURCE_DIR)/%.c=$(OBJ_DIR)/%.o)

git_show : $(OBJ)
	mkdir -p build
	gcc $(FLAGS) -o $(BUILD_DIR)/$@ $^ $(LIBS)

$(OBJ_DIR)/%.o : $(SOURCE_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	gcc $(FLAGS) -c -o $@ $< $(LIBS)

clean :
	rm -rfv ./build

run :
	./$(BUILD_DIR)/git_show
