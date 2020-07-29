#include "Fjord/Util/FileUtil.h" 

#include <cstdio> 
#include <cstdlib> 

namespace Fjord 
{

    String ReadFile(const String& filename) 
    {
        FILE* file = fopen(filename.c_str(), "r"); 

        if (!file) 
        {
            FJ_EFWARN("Could not open file '%s'", filename.c_str()); 
            return ""; 
        }

        fseek(file, 0, SEEK_END); 
        size_t size = ftell(file); 
        fseek(file, 0, SEEK_SET); 

        char* contents = (char*) calloc(size+1, 1); 

        fread(contents, size, 1, file); 
        fclose(file); 

        String out(contents); 
        free(contents); 

        return out; 
    }

}