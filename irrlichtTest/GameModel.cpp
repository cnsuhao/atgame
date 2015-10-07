#include "irrlicht.h"
using namespace irr;
using namespace video;
using namespace scene;
#include "IrrGlobal.h"

#include "GameModel.h"

GameModel::GameModel(void)
{
    mesh = NULL;
}


GameModel::~GameModel(void)
{
}

void GameModel::Load()
{
    mesh = gSceneMng->getMesh("../data/tree1/tree1.ms3d");
    if (mesh)
    {
        node = gSceneMng->addAnimatedMeshSceneNode(mesh);
        node->setMaterialFlag(video::EMF_LIGHTING, false);
        node->setMaterialFlag(video::EMF_ANTI_ALIASING, true);
        node->getMaterial(0).setFlag(video::EMF_BACK_FACE_CULLING, false);
        node->getMaterial(0).setFlag(video::EMF_FRONT_FACE_CULLING, false);
        ////node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
        ////node->setMaterialFlag(video::EMF_FRONT_FACE_CULLING, false);
        node->getMaterial(0).MaterialType = E_MATERIAL_TYPE::EMT_TRANSPARENT_ALPHA_CHANNEL;
    }
}
