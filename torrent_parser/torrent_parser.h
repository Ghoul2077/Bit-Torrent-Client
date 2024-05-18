#include <bits/stdc++.h>

#include "decoder/Decoder.h"
using namespace std;

#define FILL_DATA_STR(x, obj, attr) \
    if (obj->count(attr) > 0) x = obj->at(attr).toString();

#define FILL_DATA_INT(x, obj, attr)                    \
    if (!(obj->count(attr) > 0) ||                     \
        !(stringToInt(obj->at(attr).toString(), x))) { \
        x = 0;                                         \
    }

struct TorrentMetaInfo {
    string announce;
    string createdBy;
    string creationDate;
    string encoding;
    string comment;

    TorrentMetaInfo() {
        announce = "";
        createdBy = "";
        creationDate = "";
        encoding = "UTF-8";
        comment = "";
    }
};

struct TorrentFileInfo {
    string name;
    bigInt length;
    bigInt pieceLength;
    string pieces;

    TorrentFileInfo() {
        name = "";
        length = 0;
        pieceLength = 0;
        pieces = "";
    }
};

struct Directory;
struct DirectoryComparator {
    bool operator()(const Directory* dir1, const Directory* dir2) const;
};

struct FileComparator {
    bool operator()(const TorrentFileInfo& file1,
                    const TorrentFileInfo& file2) const {
        return (file1.name > file2.name);
    }
};

struct Directory {
    string directoryName;
    set<Directory*, DirectoryComparator> subDir;
    set<TorrentFileInfo, FileComparator> files;

    ~Directory() {
        for (auto i : subDir) {
            delete i;
        }
    }
};

inline bool DirectoryComparator::operator()(const Directory* dir1,
                                            const Directory* dir2) const {
    if (dir1 == dir2) {
        return 0;
    }
    if (dir1 != NULL && dir2 == NULL) {
        return 1;
    }
    if (dir1 == NULL && dir2 != NULL) {
        return -1;
    }
    return (dir1->directoryName > dir2->directoryName);
}

void prettyPrintDirectoryStruct(Directory* rootDir, int indentationLevel = 0);

class TorrentParser {
   private:
    TorrentMetaInfo metaInfo;
    Directory* rootDir;

   public:
    TorrentParser() : metaInfo() { rootDir = NULL; }

    static bool isValidTorrentFile(const BEncodeToken& token);
    static TorrentParser* parseFile(const string& input, bool& isSuccess);
    void printDir() { prettyPrintDirectoryStruct(rootDir); };

    ~TorrentParser() { delete rootDir; }
};
