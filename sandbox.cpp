#include <iostream>
#include <filesystem>
#include <fstream>
#include <string.h>
#include <map>
#include "cereal/types/map.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/string.hpp"

namespace std
{
  namespace filesystem
  {
    template<class Archive>
    void CEREAL_LOAD_MINIMAL_FUNCTION_NAME(const Archive&, path& out, const string& in)
    {
        out = in;
    }

    template<class Archive>
    string CEREAL_SAVE_MINIMAL_FUNCTION_NAME(const Archive& ar, const path& p)
    {
      return p.string();
    }
  }
}

using namespace std;
namespace fs = std::filesystem;

struct Object
{
    map<string, string> _map;

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(_map);
    }

    friend ostream& operator<<(ostream& outs, const Object c)
    {
        for (auto const &pair: c._map)
            std::cout << "{" << pair.first << ": " << pair.second << "}\n";
        return outs;
    }
};

int main()
{
    // create Object
    cout << "Creating object..." << endl;
    Object o;
    string a = "bye.txt";
    string b = "hello.txt";
    o._map[a] = b;
    o._map[b] = a;
    cout << "Object created: " << endl;
    cout << o;

    // serialize
    cout << "Serializing object...." << endl;
    stringstream ss;
    cereal::BinaryOutputArchive oarchive(ss);
    oarchive(o);
    cout << "Object serialized as " << ss.str() << endl;

    // write to file
    cout << "Writing serialized object to file...." << endl;
    ofstream file("serialized_object", ios::binary);
    file << ss.str();
    cout << "Object written to file." << endl;

    // read from file
    cout << "Reading from file..." << endl;
    stringstream ss2;
    fs::path ins = "serialized_object";
    ifstream file_stream(ins, ios::binary);
    ss2 << file_stream.rdbuf();
    cereal::BinaryInputArchive iarchive(ss2);
    Object out;
    iarchive(out);
    cout << "Object read from file." << endl;
    cout << out;
}