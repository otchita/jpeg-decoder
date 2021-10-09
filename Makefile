# Repertoires du projet

BIN_DIR = bin
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
OBJPROF_DIR = obj-prof

# Options de compilation/édition des liens

CC = clang
LD = clang
INC = -I$(INC_DIR)

CFLAGS += $(INC) -Wall -std=c99 -O0 -g  -Wextra
LDFLAGS = -lm

# Liste des fichiers objet

# les notres...
OBJPROF_FILES =  $(OBJPROF_DIR)/huffman.o  $(OBJPROF_DIR)/jpeg_reader.o\
	$(OBJPROF_DIR)/bitstream.o


# et les votres!
OBJ_FILES = $(OBJ_DIR)/main.o $(OBJ_DIR)/debug.o $(OBJ_DIR)/decompress.o\
	$(OBJ_DIR)/iqzz.o $(OBJ_DIR)/idct.o $(OBJ_DIR)/image_maker.o\
	$(OBJ_DIR)/color.o $(OBJ_DIR)/upsampling.o $(OBJ_DIR)/structures.o\

#	$(OBJPROF_DIR)/bitstream.o

# fichiers pour tester les modules persos sur le main
OBJ_MAIN_BITSTREAM = $(OBJ_DIR)/bitstream.o $(OBJPROF_DIR)/huffman.o $(OBJPROF_DIR)/jpeg_reader.o
OBJ_MAIN_HUFFMAN = $(OBJ_DIR)/huffman.o $(OBJPROF_DIR)/bitstream.o $(OBJPROF_DIR)/jpeg_reader.o
OBJ_MAIN_JPEG_READER = $(OBJ_DIR)/jpeg_reader.o $(OBJPROF_DIR)/bitstream.o $(OBJPROF_DIR)/huffman.o
OBJ_MAIN_BITSTREAM_JPEG_READER = $(OBJ_DIR)/jpeg_reader.o $(OBJ_DIR)/bitstream.o $(OBJPROF_DIR)/huffman.o
OBJ_MAIN_FINAL = $(OBJ_DIR)/jpeg_reader.o $(OBJ_DIR)/bitstream.o $(OBJ_DIR)/huffman.o
# cibles
TARGET = $(BIN_DIR)/jpeg2ppm
TEST_BITSTREAM = $(BIN_DIR)/test_bitstream
TEST_HUFFMAN = $(BIN_DIR)/test_huffman
TEST_JPEG_READER = $(BIN_DIR)/test_jpeg_reader

# executable avec un module perso et deux modules des profs
MAIN_WITH_BITSTREAM = $(BIN_DIR)/main_bitstream
MAIN_WITH_HUFFMAN = $(BIN_DIR)/main_huffman
MAIN_WITH_JPEG_READER = $(BIN_DIR)/main_jpeg_reader
MAIN_WITH_BITSTREAM_JPEG_READER = $(BIN_DIR)/main_bitstream_jpeg_reader
MAIN_FINAL = $(BIN_DIR)/jpeg2ppm

all: $(TARGET) $(TEST_BITSTREAM)\
	$(MAIN_WITH_BITSTREAM) $(MAIN_WITH_JPEG_READER)\
	$(MAIN_WITH_HUFFMAN)\
	$(MAIN_WITH_BITSTREAM_JPEG_READER) $(MAIN_FINAL)
#$(MAIN_WITH_HUFFMAN)

$(TARGET): $(OBJPROF_FILES) $(OBJ_FILES)
	$(LD) $(LDFLAGS) $(OBJPROF_FILES) $(OBJ_FILES) -o $(TARGET)

# ----------- creation des executables pour les tests -----------
$(TEST_BITSTREAM): $(OBJ_DIR)/test_bitstream.o $(OBJ_DIR)/bitstream.o
	$(LD) $(LDFLAGS) $(OBJ_DIR)/test_bitstream.o $(OBJ_DIR)/bitstream.o -o $(TEST_BITSTREAM)

$(TEST_HUFFMAN): $(OBJ_DIR)/test_huffman.o $(OBJ_DIR)/huffman.o
	$(LD) $(LDFLAGS) $(OBJ_DIR)/test_huffman.o $(OBJ_DIR)/huffman.o -o $(TEST_HUFFMAN)

$(TEST_JPEG_READER): $(OBJ_DIR)/test_jpeg_reader.o $(OBJ_DIR)/jpeg_reader.o
	$(LD) $(LDFLAGS) $(OBJ_DIR)/test_jpeg_reader.o $(OBJ_DIR)/jpeg_reader.o -o $(TEST_JPEG_READER)


	# ------ creation des executables pour les tests sur le main ------
$(MAIN_WITH_BITSTREAM): $(OBJ_MAIN_BITSTREAM) $(OBJ_FILES)
	$(LD) $(LDFLAGS) $(OBJ_MAIN_BITSTREAM) $(OBJ_FILES) -o $(MAIN_WITH_BITSTREAM)

#
$(MAIN_WITH_HUFFMAN): $(OBJ_MAIN_HUFFMAN) $(OBJ_FILES)
	$(LD) $(LDFLAGS) $(OBJ_MAIN_HUFFMAN) $(OBJ_FILES)-o $(MAIN_WITH_HUFFMAN)

$(MAIN_WITH_JPEG_READER): $(OBJ_MAIN_JPEG_READER) $(OBJ_FILES)
	$(LD) $(LDFLAGS) $(OBJ_MAIN_JPEG_READER) $(OBJ_FILES) -o $(MAIN_WITH_JPEG_READER)

$(MAIN_WITH_BITSTREAM_JPEG_READER): $(OBJ_MAIN_BITSTREAM_JPEG_READER) $(OBJ_FILES)
	$(LD) $(LDFLAGS) $(OBJ_MAIN_BITSTREAM_JPEG_READER) $(OBJ_FILES) -o $(MAIN_WITH_BITSTREAM_JPEG_READER)

$(MAIN_FINAL): $(OBJ_MAIN_FINAL) $(OBJ_FILES)
	$(LD) $(LDFLAGS) $(OBJ_MAIN_FINAL) $(OBJ_FILES) -o $(MAIN_FINAL)

# ------------ règles de création des objets --------------------------

$(OBJ_DIR)/main%.o: $(INC_DIR)/jpeg_reader.h $(INC_DIR)/bitstream.h $(INC_DIR)/huffman.h\
	$(INC_DIR)/debug.h $(INC_DIR)/decompress.h $(INC_DIR)/iqzz.h\
	$(INC_DIR)/idct.h $(INC_DIR)/image_maker.h $(INC_DIR)/color.h\
	$(INC_DIR)/upsampling.h $(INC_DIR)/structures.h\

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/%.h
	$(CC) -o $@ -c $< $(CFLAGS)

$(OBJ_DIR)/test_%.o: $(SRC_DIR)/test_%.c $(SRC_DIR)/%.c $(INC_DIR)/%.h
	$(CC) -o $@ -c $< $(CFLAGS)


.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJ_FILES) $(TEST_BITSTREAM) $(TEST_HUFFMAN) $(TEST_JPEG_READER)\
		$(MAIN_WITH_BITSTREAM) $(MAIN_WITH_HUFFMAN) $(MAIN_WITH_JPEG_READER) $(MAIN_WITH_BITSTREAM_JPEG_READER)\
		$(MAIN_FINAL)
	rm -f $(OBJ_DIR)/*.o
	rm -f images/*.ppm images/*.pgm images/*.blabla
