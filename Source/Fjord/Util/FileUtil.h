#pragma once 

#include "Fjord/Common.h" 

namespace Fjord 
{

    String ReadFile(const String& file); 

    String FindPath(const String& file, const Vector<String>* prefixes = nullptr, const Vector<String>* suffixes = nullptr); 

}