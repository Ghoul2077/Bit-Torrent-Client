#include "torrent_parser.h"
using namespace std;

bool insertDirectoryToStruct(Directory* rootDir, vector<string>& pathToInsert,
                             bigInt const& fileLength, bigUInt index = 0) {
    if ((rootDir == NULL) || (pathToInsert.size() == 0) ||
        (pathToInsert.size() <= index)) {
        return true;
    }

    if (index == (pathToInsert.size() - 1)) {
        TorrentFileInfo fileInfo;
        fileInfo.name = pathToInsert[index];
        fileInfo.length = fileLength;
        rootDir->files.insert(fileInfo);
        return true;
    }

    Directory* nextDir = NULL;

    string currDir = pathToInsert[index];
    set<Directory*, DirectoryComparator>* subDirs = &(rootDir->subDir);
    bool dirAlreadyPresent = false;
    for (auto i : *subDirs) {
        if (i->directoryName == currDir) {
            nextDir = i;
            dirAlreadyPresent = true;
        }
    }

    if (dirAlreadyPresent == false) {
        Directory* newDir = new Directory();
        if (newDir == NULL) {
            return false;
        }
        newDir->directoryName = currDir;
        rootDir->subDir.insert(newDir);
        nextDir = newDir;
    }

    return insertDirectoryToStruct(nextDir, pathToInsert, fileLength,
                                   index + 1);
}

void prettyPrintDirectoryStruct(Directory* rootDir, int indentationLevel) {
    if (rootDir == NULL) {
        return;
    }

    if (rootDir->directoryName != "") {
        cout << (indentationLevel == 0 ? "" : "|");
        cout << string(indentationLevel, '-') << rootDir->directoryName
             << " \\ " << endl;
    }

    for (auto i : rootDir->files) {
        cout << "|";
        cout << string(indentationLevel + 1, '-') << i.name << "\t" << i.length
             << " bytes" << endl;
    }

    for (auto i : rootDir->subDir) {
        prettyPrintDirectoryStruct(i, indentationLevel + 1);
    }
}

bool TorrentParser::isValidTorrentFile(const BEncodeToken& token) {
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

TorrentParser* TorrentParser::parseFile(
    const string& input /*Pass contents of file as string here*/,
    bool& isSuccess) {
    TorrentParser* torrentInfo = new TorrentParser();
    if (torrentInfo == NULL) {
        isSuccess = false;
        return NULL;
    }

    try {
        Bdecode parsedData(input);

        // We extract first token after parsing, as a valid torrent file will
        // always have 1 token which is always a dictionary
        BEncodeToken token;
        if (parsedData.getElementAtIndex(1, token)) {
            bool isValid = isValidTorrentFile(token);

            if (!isValid) {
                cout << "Invalid torrent file, stopping the parser" << endl;
                isSuccess = false;
                return NULL;
            }

            dict_t const* torrentData = get<dict_t*>(token.val);

            FILL_DATA_STR(torrentInfo->metaInfo.announce, torrentData,
                          "announce");
            FILL_DATA_STR(torrentInfo->metaInfo.createdBy, torrentData,
                          "created by");
            FILL_DATA_STR(torrentInfo->metaInfo.creationDate, torrentData,
                          "creation date");
            FILL_DATA_STR(torrentInfo->metaInfo.encoding, torrentData,
                          "encoding");
            FILL_DATA_STR(torrentInfo->metaInfo.comment, torrentData,
                          "comment");

            BEncodeToken const* infoData = &torrentData->at("info");
            dict_t const* infoStruct = get<dict_t*>(infoData->val);

            bool isMultipleFiles = infoStruct->count("files");
            if (!isMultipleFiles) {
                Directory* rootDir = new Directory();
                if (rootDir == NULL) {
                    isSuccess = false;
                    return NULL;
                }

                rootDir->directoryName = "";
                TorrentFileInfo fileInfo;
                FILL_DATA_STR(fileInfo.pieces, infoStruct, "pieces");
                FILL_DATA_INT(fileInfo.pieceLength, infoStruct, "piece length");
                FILL_DATA_STR(fileInfo.name, infoStruct, "name");
                FILL_DATA_INT(fileInfo.length, infoStruct, "length");
                rootDir->files.insert(fileInfo);

                torrentInfo->rootDir = rootDir;
            } else {
                Directory* rootDir = new Directory();
                if (rootDir == NULL) {
                    isSuccess = false;
                    return NULL;
                }

                FILL_DATA_STR(rootDir->directoryName, infoStruct, "name");
                list_t const* files = get<list_t*>(infoStruct->at("files").val);

                for (auto i : *files) {
                    dict_t const* subFile = get<dict_t*>(i.val);
                    list_t const* path = get<list_t*>(subFile->at("path").val);
                    bigInt const length =
                        get<bigInt>(subFile->at("length").val);

                    if (path->size() == 1) {
                        TorrentFileInfo fileInfo;
                        fileInfo.name = path->at(0).toString();
                        fileInfo.length = length;
                        rootDir->files.insert(fileInfo);
                    } else {
                        vector<string> pathAsVector;
                        for (bigUInt i = (path->size() - 1); i > 0; i--) {
                            if (path->at(i).getDatatype() == B_STRING) {
                                pathAsVector.push_back(path->at(i).toString());
                            }
                        }
                        if (path->at(0).getDatatype() == B_STRING) {
                            pathAsVector.push_back(path->at(0).toString());
                        }
                        insertDirectoryToStruct(rootDir, pathAsVector, length);
                    }
                }

                torrentInfo->rootDir = rootDir;
            }
        }

        isSuccess = true;
    } catch (bEncodeReturnTypes& err) {
        printError(err);
        isSuccess = false;
    }

    return torrentInfo;
}
