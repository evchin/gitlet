#include "repository/repository.h"

int main()
{
    stringstream ss;
    fs::path filepath = "C:\\Users\\ecwin\\Downloads\\gitlet-test\\.gitlet\\blobs\\cc\\f1fffaa648b1b307a2b6c3b2d273b50c0553d1";
    ifstream file_stream(filepath, ios::binary);
    ss << file_stream.rdbuf();
    cereal::BinaryInputArchive iarchive(ss);
    string object;
    iarchive(object);
    cout << object;
}