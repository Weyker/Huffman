CPP=g++
CPPFLAGS=	-O0 \
			-g3 \
			-Wall \
			-std=c++11

SRC = source/

BUILD_DIR= build/

OBJ=	compressor \
		decompressor

INC=-Iinclude/

%: $(SRC)%.cpp
	@echo "[C++] Compiling $<"
	@$(CPP) $(CPPFLAGS) $(INC) $< -o $(BUILD_DIR)$@

all: $(OBJ)

clean: 
	rm -f -r $(BUILD_DIR)*