CC = g++
CFLAGS = -g -Wall -std=c++17
TARGET = client.out
INCLUDE_DIRS = .
CLEAN_OBJECTS = -rm -f decoder/*.o client/*.o *.o
TEST_INPUTS=test_5.torrent

_client: client/client.cpp client/client.h _parser
	mkdir -p dist
	$(CC) $(CFLAGS) -I $(INCLUDE_DIRS) -o dist/$(TARGET) client/client.cpp torrent_parser.o Decoder.o BEncodeToken.o utils.o
	$(CLEAN_OBJECTS)

_parser: torrent_parser/torrent_parser.cpp torrent_parser/torrent_parser.h _decoder
	$(CC) $(CFLAGS) -I $(INCLUDE_DIRS) -c torrent_parser/torrent_parser.cpp

_decoder: decoder/Decoder.cpp decoder/Decoder.h decoder/BEncodeToken.cpp decoder/BEncodeToken.h utils/utils.cpp utils/utils.h
	$(CC) $(CFLAGS) -I $(INCLUDE_DIRS) -c decoder/Decoder.cpp decoder/BEncodeToken.cpp utils/utils.cpp

test_memory_leaks: client
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt dist/$(TARGET) tests/$(TEST_INPUTS)

clean:
	$(CLEAN_OBJECTS)
	-rm -rf dist
