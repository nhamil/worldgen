#include "Fjord/Util/FileUtil.h" 

#include <cstdio> 
#include <cstdlib> 
#include <fstream> 

namespace Fjord 
{

    static Vector<String> s_EnginePrefixes({
        "", 
        "./Assets/", 
        "../Assets/" 
    });

    String ReadFile(const String& filename) 
    {
        String fn = FindPath(filename); 
        FILE* file = fopen(fn.c_str(), "r"); 

        if (!file) 
        {
            FJ_EFWARN("Could not open file '%s'", fn.c_str()); 
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

    String FindPath(const String& filename, const Vector<String>* prefixes, const Vector<String>* suffixes) 
    {
        Vector<String> pre({""}), suf({""}); 
        if (prefixes) pre.insert(pre.end(), prefixes->begin(), prefixes->end()); 
        if (suffixes) suf.insert(suf.end(), suffixes->begin(), suffixes->end()); 

        for (const String& engPre : s_EnginePrefixes) 
        {
            for (const String& p : pre) 
            {
                for (const String& s : suf) 
                {
                    String f = engPre + p + filename + s; 
                    // FJ_EFDEBUG("Trying '%s'", f.c_str()); 

                    if (FILE* file = fopen(f.c_str(), "r")) 
                    {
                        // FJ_EFDEBUG("Found file '%s' at '%s'", filename.c_str(), f.c_str()); 
                        fclose(file); 
                        return f; 
                    }
                    else 
                    {
                        continue; 
                    }
                }
            }
        }

        FJ_EFWARN("Could not find file '%s'", filename.c_str()); 
        return filename; 
    }

}