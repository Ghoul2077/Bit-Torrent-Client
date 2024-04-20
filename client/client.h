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
