#include "BEncodeToken.h"

#define COPY_BENCODE_TOKEN_VALUE(obj1, obj2)            \
    if (holds_alternative<list_t*>(obj1.val)) {         \
        list_t const* listPtr = get<list_t*>(obj1.val); \
        list_t* ptr = new list_t(*listPtr);             \
        obj2.val = ptr;                                 \
    } else if (holds_alternative<dict_t*>(obj1.val)) {  \
        dict_t const* dictPtr = get<dict_t*>(obj1.val); \
        dict_t* ptr = new dict_t(*dictPtr);             \
        obj2.val = ptr;                                 \
    } else {                                            \
        obj2.val = obj1.val;                            \
    }

BEncodeToken::BEncodeToken() : val() {}

BEncodeToken::BEncodeToken(const bigInt& _) : val(_) {}

BEncodeToken::BEncodeToken(const string& _) : val(_) {}

BEncodeToken::BEncodeToken(const list_t& _) : val() {
    list_t* ptr = new list_t(_);
    val = ptr;
}

BEncodeToken::BEncodeToken(const dict_t& _) : val() {
    dict_t* ptr = new dict_t(_);
    val = ptr;
}

BEncodeToken::BEncodeToken(const BEncodeToken& _){
    COPY_BENCODE_TOKEN_VALUE(_, (*this))}

BEncodeToken& BEncodeToken::operator=(BEncodeToken const& _) {
    COPY_BENCODE_TOKEN_VALUE(_, (*this))
    return *this;
}

bool BEncodeToken::operator==(const BEncodeToken& _) const {
    return (val == _.val);
}

void BEncodeToken::print() const {
    if (holds_alternative<bigInt>(val)) {
        cout << get<bigInt>(val);
    } else if (holds_alternative<string>(val)) {
        string const* str = &get<string>(val);
        cout << "\"" + *str + "\"";
    } else if (holds_alternative<list_t*>(val)) {
        bigUInt instances = 0;
        cout << "[";
        list_t const* listPtr = get<list_t*>(val);
        for (auto i : *listPtr) {
            i.print();
            instances++;
            if (instances != listPtr->size()) {
                cout << ", ";
            }
        }
        cout << "]";
    } else if (holds_alternative<dict_t*>(val)) {
        bigUInt instances = 0;
        cout << "{";
        dict_t const* dictPtr = get<dict_t*>(val);
        for (auto i : *dictPtr) {
            cout << i.first;
            cout << " : ";
            i.second.print();
            instances++;
            if (instances != dictPtr->size()) {
                cout << ", ";
            }
        }
        cout << "}";
    }
}

string BEncodeToken::toString() const {
    string res = "";

    if (holds_alternative<bigInt>(val)) {
        bigInt const* num = &get<bigInt>(val);
        if (!intToString(*num, res)) {
            cout << __FUNCTION__ << "() : " << __LINE__ << " : "
                 << "Cannot convert the integer token to string" << endl;
            res = "";
        }
    } else if (holds_alternative<string>(val)) {
        string const* str = &get<string>(val);
        res = (*str);
    } else if (holds_alternative<list_t*>(val)) {
        list_t const* listPtr = get<list_t*>(val);
        vector<string> out;
        for (auto i : *listPtr) {
            out.push_back(i.toString());
        }
        if (!arrayToString(out, res)) {
            cout << __FUNCTION__ << "() : " << __LINE__ << " : "
                 << "Cannot convert the list to string" << endl;
            res = "";
        }
    } else if (holds_alternative<dict_t*>(val)) {
        dict_t const* dictPtr = get<dict_t*>(val);
        unordered_map<string, string> out;
        for (auto i : *dictPtr) {
            if (out.count(i.first) == 0) {
                out[i.first] = i.second.toString();
            }
        }
        if (!unorderedMapToString(out, res)) {
            cout << __FUNCTION__ << "() : " << __LINE__ << " : "
                 << "Cannot convert dictionary to string" << endl;
        }
    }

    return res;
}

bEncodeDataTypes BEncodeToken::getDatatype() const {
    if (holds_alternative<bigInt>(val)) {
        return B_INTEGER;
    } else if (holds_alternative<string>(val)) {
        return B_STRING;
    } else if (holds_alternative<list_t*>(val)) {
        return B_LIST;
    } else if (holds_alternative<dict_t*>(val)) {
        return B_DICTIONARY;
    } else {
        return B_NONE;
    }
}

BEncodeToken::~BEncodeToken() {
    if (holds_alternative<list_t*>(val)) {
        list_t const* listPtr = get<list_t*>(val);
        delete listPtr;
    } else if (holds_alternative<dict_t*>(val)) {
        dict_t const* dictPtr = get<dict_t*>(val);
        delete dictPtr;
    }
}
