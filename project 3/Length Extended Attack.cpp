#include <openssl/sha.h>
#include <iostream>
#include <iomanip>
#include <string>
#include "sha256.hpp"
using namespace std;

void logdigest(string message, uint8_t* digest) {
    cout << "Plaintext: " << message << endl;
    cout << "SHA-256 result: " << hex << setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        cout << setw(2) << static_cast<uint32_t>(digest[i]);
    }
    cout << endl << endl;
}

void loghex(string message) {
    cout << hex << setfill('0');
    for (int i = 0; i < message.length(); i++) {
        cout << setw(2) << static_cast<uint32_t>(static_cast<unsigned char>(message[i]));
    }
    cout << endl << endl;
}

//void splitVector(vector<uint32_t>& vec, uint8_t* digest) {
//    for (size_t i = 0; i < vec.size(); i++) {
//        uint32_t value = vec[i];
//        for (size_t j = 0; j < 4; j++) {
//            digest[i * 4 + j] = static_cast<uint8_t>(value >> (j * 8));
//        }
//    }
//}

void getextendedtext(size_t len, string& append, string& extendedtext) {
    if (1) //||100бн 46
    {
        extendedtext += static_cast<uint8_t>(0x80); // ox80 == 10000000
        for (size_t i = 0; i < 56-len-1; i++)
        {
            extendedtext += static_cast<uint8_t>(0x00);
        }
    }
    len *= 8;
    for (int i = 1; i <= 8; ++i) //||len
    {
        uint8_t c = static_cast<uint8_t>(len >> (64 - 8 * i));
        extendedtext += c;
    }

    extendedtext += append; //||append
    cout << "Hex of extendedtext is: " << endl;
    loghex(extendedtext);
}

void server(string data, uint8_t* digest) {
    string secret = "secret";
    data = secret + data;
    SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), digest);
    cout << "Server   ";
    logdigest(data, digest);
}

void attacker(size_t len, string &append, string &extendedtext, uint8_t *digest) { //known parameters

    vector<uint32_t> state;
    uint32_t mergedValue;
    for (int i = 0; i < SHA256_DIGEST_LENGTH / 4; i++) {// merge digest to state
        mergedValue = 0;
        for (int j = 0; j < 4; j++) {
            mergedValue |= static_cast<uint32_t>(digest[i * 4 + j]) << ((3-j) * 8);
        }
        state.push_back(mergedValue);
    }
    vector<uint8_t> __message; //invert append to vector
    for (int i = 0; i<append.length(); i++)
    {
        __message.push_back(static_cast<uint8_t>(append[i]));
    }
    vector<uint8_t> __digest;

    Sha256 sha256;
    sha256.len = len * 8;;
    sha256.appendlen = append.length() * 8;
    sha256.initial_message_digest_ = state; //set state as IV
    sha256.encrypt(__message, &__digest); //H' encrypt
    

    for (int i = 0; i<SHA256_DIGEST_LENGTH; i++) //update digest
    {
        digest[i]=__digest[i];
    }
    getextendedtext(len, append, extendedtext);
    cout << "Attacker ";
    logdigest("secretdata" + extendedtext, digest);
}
//void test(string);
int main() {
    string secret = "secret";
    string data = "data";
    string append;
    string extendedtext;
    cout << "Please enter append message for length attack: " << endl;
    cin >> append;

    uint8_t* digest = new uint8_t[SHA256_DIGEST_LENGTH];
    uint8_t attackerdigest[SHA256_DIGEST_LENGTH];
    uint8_t virifydigest[SHA256_DIGEST_LENGTH];
    server(data, digest);
    attacker(secret.length() + data.length(), append, extendedtext, digest);
    memcpy(attackerdigest, digest, SHA256_DIGEST_LENGTH);
    server(data+extendedtext, digest);
    memcpy(virifydigest, digest, SHA256_DIGEST_LENGTH);
    //test(data + extendedtext);

    bool success = 1;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        if (attackerdigest[i] != virifydigest[i]) {
            cout << "Authentication not passed: Fail Attack." << endl;
            success = 0;
            break;
        }
    }
    if(success)
        cout << "Authentication passed: Success Attack!" << endl;
    delete[] digest;
    return 0;
}

//void test(string message) {
//    
//    Sha256 sha256t;
//    vector<uint32_t> iv =
//    {
//        0x6a09e667, 0xbb67ae85, 0x3c6ef372,
//        0xa54ff53a, 0x510e527f, 0x9b05688c,
//        0x1f83d9ab, 0x5be0cd19
//    };
//    sha256t.len = 80;
//    sha256t.appendlen = 48;
//    sha256t.initial_message_digest_ = iv;
//    string str = sha256t.getHexMessageDigest("secret" + message);
//    cout << "TEST: ";
//    cout << str<<endl;
//}

