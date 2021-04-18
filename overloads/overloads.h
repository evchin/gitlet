#ifndef OVERLOADS_H
#define OVERLOADS_H

#include <iostream>
#include <filesystem>
#include <string.h>

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

#endif // OVERLOADS_H