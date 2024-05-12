#include "client.h"

#include "torrent_parser/torrent_parser.h"
using namespace std;

int main(int argc, char const* argv[]) {
    for (int i = 1; i < argc; i++) {
        string input = "";

        ifstream inputFile(argv[i], ifstream::in | ifstream::binary);
        char ch;
        while (inputFile.get(ch)) {
            input += ch;
        }
        inputFile.close();

        bool parsingSuccess = false;
        TorrentParser* parsedData =
            TorrentParser::parseFile(input, parsingSuccess);

        if ((parsingSuccess == true) && (parsedData != NULL)) {
            parsedData->printDir();
        } else {
            cout << "Error while parsing the torrent file" << endl;
        }

        delete parsedData;
    }
    return 0;
}
