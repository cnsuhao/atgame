#pragma once

#include <vector>

class atgBlenderImport
{
public:
    static bool loadMesh(const char* fileName, std::vector<class atgMesh*>& mesh);
};

