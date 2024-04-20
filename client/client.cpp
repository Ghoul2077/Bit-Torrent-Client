#include "client.h"
using namespace std;

bool isValidTorrentFile(const BEncodeToken& token) {
    if (token.getDatatype() != B_DICTIONARY) {
        cout << "Torrent file is malformed" << endl;
        return false;
    }

    dict_t const* torrentData = get<dict_t*>(token.val);
    if (torrentData->count("info") == 0) {
        cout << "Info section is not present" << endl;
        return false;
    }

    BEncodeToken const* infoData = &torrentData->at("info");
    if (infoData->getDatatype() != B_DICTIONARY) {
        cout << "Info section is malformed" << endl;
        return false;
    }

    dict_t const* infoStruct = get<dict_t*>(infoData->val);
    if ((infoStruct->count("name") == 0) ||
        (infoStruct->count("pieces") == 0) ||
        (infoStruct->count("piece length") == 0)) {
        cout << "Name, pieces or piece length info not present" << endl;
        return false;
    }

    bool isMultipleFiles = infoStruct->count("files");
    if (isMultipleFiles) {
        if (infoStruct->at("files").getDatatype() != B_LIST) {
            cout << "Files section is malformed" << endl;
            return false;
        }
        list_t const* files = get<list_t*>(infoStruct->at("files").val);
        for (auto i : *files) {
            if (i.getDatatype() != B_DICTIONARY) {
                cout << "File structure is malformed" << endl;
                return false;
            }
            dict_t const* subFile = get<dict_t*>(i.val);
            if ((subFile->count("path") == 0) ||
                (subFile->count("length") == 0)) {
                cout << "Path or length of file is not present" << endl;
                return false;
            }
        }
    } else {
        if (infoStruct->count("length") == 0) {
            cout << "Length of file is not specified" << endl;
            return false;
        }
    }

    return true;
}

int main(int argc, char const* argv[]) {
    for (int i = 1; i < argc; i++) {
        string input = "";

        ifstream inputFile(argv[i], ifstream::in | ifstream::binary);
        char ch;
        while (inputFile.get(ch)) {
            input += ch;
        }
        inputFile.close();

        try {
            Bdecode parsedData(input);

            BEncodeToken token;
            if (parsedData.getElementAtIndex(1, token)) {
                bool isValid = isValidTorrentFile(token);

                if (!isValid) {
                    return 0;
                }

                dict_t const* torrentData = get<dict_t*>(token.val);

                TorrentMetaInfo metaInfo;
                FILL_DATA_STR(metaInfo.announce, torrentData, "announce");
                FILL_DATA_STR(metaInfo.createdBy, torrentData, "created by");
                FILL_DATA_STR(metaInfo.createdBy, torrentData, "creation date");
                FILL_DATA_STR(metaInfo.createdBy, torrentData, "encoding");
                FILL_DATA_STR(metaInfo.createdBy, torrentData, "comment");

                BEncodeToken const* infoData = &torrentData->at("info");
                dict_t const* infoStruct = get<dict_t*>(infoData->val);

                TorrentFileInfo fileInfo;
                FILL_DATA_STR(fileInfo.name, infoStruct, "name");
                FILL_DATA_STR(fileInfo.pieces, infoStruct, "pieces");
                FILL_DATA_INT(fileInfo.pieceLength, infoStruct, "piece length");

                bool isMultipleFiles = infoStruct->count("files");
                if (!isMultipleFiles) {
                    FILL_DATA_INT(fileInfo.length, infoStruct, "length");
                    cout << fileInfo.name;
                    cout << " " << fileInfo.length << " bytes" << endl;
                } else {
                    cout << fileInfo.name << " : " << endl;
                    list_t const* files =
                        get<list_t*>(infoStruct->at("files").val);

                    for (auto i : *files) {
                        dict_t const* subFile = get<dict_t*>(i.val);
                        list_t const* path =
                            get<list_t*>(subFile->at("path").val);

                        string finalPath = "";
                        for (bigUInt i = 0; i < path->size(); i++) {
                            if (path->at(i).getDatatype() == B_STRING) {
                            }
                        }
                        cout << " " << subFile->at("length").toString()
                             << " bytes" << endl;
                    }
                }
            }
        } catch (bEncodeReturnTypes& err) {
            printError(err);
        }
    }
    return 0;
}
