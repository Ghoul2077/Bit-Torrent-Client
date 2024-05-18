#include <bits/stdc++.h>

#include "../utils/utils.h"
using namespace std;

#define B_TOKEN_END 'e'

enum bEncodeDataTypes { B_INTEGER, B_STRING, B_LIST, B_DICTIONARY, B_NONE };

struct BEncodeToken {
    variant<bigInt, string, vector<BEncodeToken>*, map<string, BEncodeToken>*>
        val;

    BEncodeToken();
    BEncodeToken(const bigInt& _);
    BEncodeToken(const string& _);
    BEncodeToken(const vector<BEncodeToken>& _);
    BEncodeToken(const map<string, BEncodeToken>& _);
    BEncodeToken(const BEncodeToken& _);
    BEncodeToken& operator=(BEncodeToken const& _data);
    bool operator==(BEncodeToken const& _data) const;
    bEncodeDataTypes getDatatype() const;
    string toString() const;
    void print() const;
    ~BEncodeToken();
};

typedef map<string, BEncodeToken> dict_t;
typedef vector<BEncodeToken> list_t;
