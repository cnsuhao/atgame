#pragma once

class GameModel
{
public:
    GameModel(void);
    ~GameModel(void);

    void Load();




protected:
    class scene::IAnimatedMesh* mesh;
    class scene::IAnimatedMeshSceneNode* node;
};

