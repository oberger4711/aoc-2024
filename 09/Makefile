TARGET ?= solve
SRC_DIRS ?= ./

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)

$(TARGET): $(SRCS)
	g++ -std=c++17 $(SRCS) -o $@

.PHONY: clean
clean:
	$(RM) $(TARGET)