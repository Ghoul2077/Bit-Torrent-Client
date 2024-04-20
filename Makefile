CC = g++
CFLAGS = -g -Wall -std=c++17
TARGET = client.out
INCLUDE_DIRS = .
CLEAN_OBJECTS = -rm -f decoder/*.o client/*.o *.o
TEST_INPUTS=test_1.torrent

client: client/client.cpp client/client.h encoder
	mkdir -p dist
	$(CC) $(CFLAGS) -I $(INCLUDE_DIRS) -o dist/$(TARGET) client/client.cpp Decoder.o BEncodeToken.o utils.o
	$(CLEAN_OBJECTS)

encoder: decoder/Decoder.cpp decoder/Decoder.h decoder/BEncodeToken.cpp decoder/BEncodeToken.h utils/utils.cpp utils/utils.h
	$(CC) $(CFLAGS) -I $(INCLUDE_DIRS) -c decoder/Decoder.cpp decoder/BEncodeToken.cpp utils/utils.cpp

test_memory_leaks: client
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt dist/$(TARGET) tests/$(TEST_INPUTS)

clean:
	$(CLEAN_OBJECTS)
	-rm -rf dist
