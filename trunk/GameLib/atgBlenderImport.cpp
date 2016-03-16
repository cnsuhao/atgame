#include "atgBlenderImport.h"

#include "atgBase.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/mesh.h>

#include <sstream>
using namespace Assimp;

#include "atgMesh.h"
#include "atgMaterial.h"
#include "atgRenderer.h"
#include "atgMisc.h"
#include "atgShaderLibrary.h"

//atgBlenderImport::atgBlenderImport(void)
//{
//}
//
//
//atgBlenderImport::~atgBlenderImport(void)
//{
//}


PrimitveType ConvertPrimitiveType(aiPrimitiveType type)
{
    PrimitveType newType;

    switch (type)
    {
    case aiPrimitiveType_POINT:
        newType = PT_POINTS;
        break;
    case aiPrimitiveType_LINE:
        newType = PT_LINES;
        break;
    case aiPrimitiveType_TRIANGLE:
        newType = PT_TRIANGLES;
        break;
    case aiPrimitiveType_POLYGON:
        newType = PT_TRIANGLE_FAN;
        break;
    case _aiPrimitiveType_Force32Bit:
        newType = PT_TRIANGLE_STRIP;
        break;
    default:
        break;
    }

    return newType;
}


bool atgBlenderImport::loadMesh( const char* fileName, std::vector<class atgMesh*>& meshs )
{
    Importer importer;

    //FILE * pf = fopen(fileName, "r+b");
    //if (pf == NULL) { LOG("Open file(%s) failure.\n", fileName); }

    //const aiScene* pScene = importer.ReadFile(  fileName,
    //                                            /*aiProcess_CalcTangentSpace|*/
    //                                            aiProcess_Triangulate|
    //                                            aiProcess_JoinIdenticalVertices|
    //                                            aiProcess_SortByPType |
    //                                            aiProcess_OptimizeMeshes );
    
    //if (pScene == 0)
    //{
    //    fclose(pf);
    //    return false;
    //}

    atgReadFile reader;
    if(!reader.Open(fileName))
    {
        LOG("Open file(%s) failure.\n", fileName);
        return false;
    }

    uint32 srcBufLen = reader.GetLength();
    int8* pSourceBuffer = new int8[srcBufLen];
    reader.Read(pSourceBuffer, srcBufLen, 1);

    std::string extension = ImageLoader::GetFileExt(fileName);

    const aiScene* pScene = importer.ReadFileFromMemory(  pSourceBuffer,
                                                            srcBufLen,
                                                            /*aiProcess_CalcTangentSpace|*/
                                                            aiProcess_Triangulate|
                                                            aiProcess_JoinIdenticalVertices|
                                                            aiProcess_SortByPType |
                                                            aiProcess_OptimizeMeshes , extension.c_str());

    if (pScene == 0)
    {
        const char* p = importer.GetErrorString();
        delete [] pSourceBuffer;
        return false;
    }


    if(pScene->HasCameras())
    {
        LOG("has Cameras.\n");
    }

    if (pScene->HasLights())
    {
        LOG("has Lights.\n");
    }


    if (pScene->HasTextures())
    {
        LOG("has Textures.\n");
    }

    for (uint32 i = 0; i < pScene->mRootNode->mNumChildren; ++i)
    {
        aiNode* pNode = pScene->mRootNode->mChildren[i];
        for (uint32 mi = 0; mi < pNode->mNumMeshes; mi++ )
        {
            aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[mi]];

            atgMesh* pNewMesh =  new atgMesh(); 
            meshs.push_back(pNewMesh);
            pNewMesh->_submeshs.resize(1);
            pNewMesh->_submeshs[0]._parent = pNewMesh;

            memcpy(&pNewMesh->_transformation.m, &(pNode->mTransformation.a1), sizeof(atgMatrix));

            aiPrimitiveType primitType = (aiPrimitiveType)pMesh->mPrimitiveTypes;
            switch (primitType)
            {
            case aiPrimitiveType_POINT:
                LOG("mesh type is point.\n");
                break;
            case aiPrimitiveType_LINE:
                LOG("mesh type is line.\n");
                break;
            case aiPrimitiveType_TRIANGLE:
                LOG("mesh type is triangle.\n");
                break;
            case aiPrimitiveType_POLYGON:
                LOG("mesh type is polygon.\n");
                break;
            case _aiPrimitiveType_Force32Bit:
                LOG("mesh type is Force32Bit.\n");
                break;
            default:
                break;
            }

            pNewMesh->_submeshs[0]._faces.primitiveType = ConvertPrimitiveType(primitType);

            if (pMesh->HasPositions())
            {
                LOG("mesh has Positions[%d].\n", pMesh->mNumVertices);

                //for (uint32 j = 0; j < pMesh->mNumVertices; ++j)
                //{
                    //LOG("vertex [%f,%f,%f].\n", pMesh->mVertices[j].x,
                    //    pMesh->mVertices[j].y,
                    //    pMesh->mVertices[j].z);
                //}

                pNewMesh->_vertices.number = pMesh->mNumVertices;
                uint32 size = pMesh->mNumVertices * 3 * sizeof(float);
                pNewMesh->_vertices.positions = (float*)new char[size];
                memcpy(pNewMesh->_vertices.positions, pMesh->mVertices, size);
            }

            if (pMesh->HasNormals())
            {
                LOG("mesh has Normals[%d].\n", pMesh->mNumVertices);

                if (pMesh->mNumVertices > 65535)
                {
                    LOG("-----!!!!!! Too many(%d>65535) vertices will not support.", pMesh->mNumVertices);
                }

                //for (uint32 j = 0; j < pMesh->mNumVertices; ++j)
                //{
                    //LOG("normal [%f,%f,%f].\n", pMesh->mNormals[j].x,
                    //    pMesh->mNormals[j].y,
                    //    pMesh->mNormals[j].z);
                //}

                uint32 size = pMesh->mNumVertices * 3 * sizeof(float);
                pNewMesh->_vertices.normals = (float*)new char[size];
                memcpy(pNewMesh->_vertices.normals, pMesh->mNormals, size);
            }

            if (pMesh->HasFaces())
            {
                LOG("mesh has Faces[%d].\n", pMesh->mNumFaces);

                uint32 size = pMesh->mNumFaces * pMesh->mFaces[0].mNumIndices * sizeof(uint16);
                pNewMesh->_submeshs[0]._faces.indices = (uint16*)new char[size];
                pNewMesh->_submeshs[0]._faces.number = pMesh->mNumFaces;
                pNewMesh->_submeshs[0]._faces.numberIndices = pMesh->mFaces[0].mNumIndices;
                uint16* pIndices = pNewMesh->_submeshs[0]._faces.indices;

                std::stringstream indexStr;
                for (uint32 j = 0; j < pMesh->mNumFaces; ++j)
                {
                    indexStr.str("");
                    for (uint32 k = 0; k < pMesh->mFaces[j].mNumIndices; k++)
                    {
                        //indexStr << pMesh->mFaces[j].mIndices[k] << " ";
                        *pIndices++ = (uint16)pMesh->mFaces[j].mIndices[k];
                    }
                    //LOG("face vertex[%d],[ %s]\n.", pMesh->mFaces[j].mNumIndices, indexStr.str().c_str());
                }
            }

            if (pMesh->HasTangentsAndBitangents())
            {
                LOG("mesh has Tangents And Bitangents[%d].\n", pMesh->mNumVertices);
            }


            if (pMesh->GetNumUVChannels() > 0)
            {
                LOG("mesh has uv channels[%d].\n", pMesh->GetNumUVChannels());

                uint32 size = pMesh->mNumVertices * 2 * sizeof(float);
                pNewMesh->_vertices.texureCoords = (float*)new char[size];
                //memcpy(pNewMesh->_vertices.texureCoords, pMesh->mTextureCoords[0], size);
                for (uint32 k = 0; k < pMesh->mNumVertices; ++k)
                {
                    pNewMesh->_vertices.texureCoords[2*k] = (pMesh->mTextureCoords[0])[k].x;
                    pNewMesh->_vertices.texureCoords[2*k+1] = ((pMesh->mTextureCoords[0])[k].y < 0 ? (pMesh->mTextureCoords[0])[k].y + 1.0f : (pMesh->mTextureCoords[0])[k].y);
                }

                //for (uint32 j = 0; j < pMesh->GetNumUVChannels(); ++j)
                //{
                //    for (uint32 k = 0; k < pMesh->mNumVertices; ++k)
                //    {
                //        LOG("mesh uv channels[%d] TextureCoords[%f,%f,%f].\n", 
                //            j, 
                //            (pMesh->mTextureCoords[j])[k].x,
                //            (pMesh->mTextureCoords[j])[k].y,
                //            (pMesh->mTextureCoords[j])[k].z);
                //    }
                //}
            }

            if (pMesh->GetNumColorChannels())
            {
                LOG("mesh has colors channels[%d].\n", pMesh->GetNumColorChannels());
                for (uint32 j = 0; j < pMesh->GetNumColorChannels(); ++j)
                {
                    for (uint32 k = 0; k < pMesh->mNumVertices; ++k)
                    {
                        LOG("mesh color channels[%d] Color[%f,%f,%f,%f].\n", 
                            j, 
                            (pMesh->mColors[j])[k].r,
                            (pMesh->mColors[j])[k].g,
                            (pMesh->mColors[j])[k].b,
                            (pMesh->mColors[j])[k].a);
                    }
                }
            }

            if (pMesh->HasBones())
            {
                LOG("mesh has Bones[%d].\n",pMesh->mBones);
            }


            if (pMesh->mMaterialIndex >= 0)
            {
                pNewMesh->_submeshs[0]._materialIndex = pMesh->mMaterialIndex;
                aiMaterial* pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

                atgMaterial* pNewMaterial = new atgMaterial();
                pNewMesh->_materials.push_back(pNewMaterial);
                atgPass* pPass = atgShaderLibFactory::FindOrCreatePass(SOLIDCOLOR_PASS_IDENTITY);
                if (pPass)
                {
                    ((atgShaderSolidColor*)pPass)->SetSolidColor(Vec3One);
                    pNewMaterial->SetPass(pPass);
                }
                

                aiString name;
                pMaterial->Get(AI_MATKEY_NAME, name);

                LOG("material[%i] name=[%s].\n", i, name.C_Str());

                for (int pi = 0, numProp = pMaterial->mNumProperties; pi < numProp; ++pi )
                {
                    aiMaterialProperty* pProp = pMaterial->mProperties[pi];
                    LOG("pAIMaterial[%s] Properties[%d][%s] dataType[%d] dataLength[%d]\n", name.C_Str(), pi, pProp->mKey.C_Str(), pProp->mType, pProp->mDataLength);
                }

                aiColor3D diffColor;
                if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffColor) == AI_SUCCESS)
                {
                    pNewMaterial->SetDiffuseColor(atgVec3(diffColor.r, diffColor.g, diffColor.b));
                }

                aiColor3D emisColor;
                if (pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emisColor) == AI_SUCCESS)
                {
                    pNewMaterial->SetEmissiveColor(atgVec3(emisColor.r, emisColor.g, emisColor.b));
                }

                aiString path;
                aiTextureMapping mapping;
                aiTextureOp op;
                aiTextureMapMode mapMode;
                for (uint32 i = 0; i < pMaterial->GetTextureCount(aiTextureType_DIFFUSE); ++i)
                {
                    if(AI_SUCCESS == pMaterial->GetTexture(aiTextureType_DIFFUSE, 
                        i, 
                        &path, 
                        &mapping, 
                        NULL, 
                        NULL, 
                        &op,
                        &mapMode))
                    {

                        path = ImageLoader::GetFilePath(fileName).append(path.C_Str());

                        atgTexture* pTexture = g_Renderer->FindCacheTexture(path.C_Str());
                        if (!pTexture)
                        {
                            Image* pImage = ImageLoader::GetInstance().Load(path.C_Str(), false, IsOpenGLGraph() ? CO_RGBA : CO_ARGB);
                            if (pImage)
                            {
                                pTexture = new atgTexture();
                                if (pTexture->Create(pImage->width, pImage->height, pImage->bpp == 24 ? TF_R8G8B8 : TF_R8G8B8A8, pImage->imageData, false))
                                {
                                    g_Renderer->InsertCacheTexture(path.C_Str(), pTexture);
                                }
                            }
                        }

                        aiUVTransform uvt;
                        if (pMaterial->Get(AI_MATKEY_UVTRANSFORM_DIFFUSE(0), uvt) == AI_SUCCESS)
                        {

                        }

                        pNewMaterial->AddTexture(TL_DIFFUSE, pTexture);
                        atgPass* pPass = atgShaderLibFactory::FindOrCreatePass(LIGHT_TEXTURE_PASS_IDENTITY);
                        if (pPass)
                        {
                            ((atgShaderLightTexture*)pPass)->SetMaterial(pNewMaterial);
                            pNewMaterial->SetPass(pPass);
                        }
                    }
                }

                
                for (uint32 i = 0; i < pMaterial->GetTextureCount(aiTextureType_NORMALS); ++i)
                {
                    if(AI_SUCCESS == pMaterial->GetTexture(aiTextureType_NORMALS, 
                        i, 
                        &path, 
                        &mapping, 
                        NULL, 
                        NULL, 
                        &op,
                        &mapMode))
                    {
                        path = ImageLoader::GetFilePath(fileName).append(path.C_Str());

                        atgTexture* pTexture = g_Renderer->FindCacheTexture(path.C_Str());
                        if (!pTexture)
                        {
                            Image* pImage = ImageLoader::GetInstance().Load(path.C_Str(), false, IsOpenGLGraph() ? CO_RGBA : CO_ARGB);
                            if (pImage)
                            {
                                pTexture = new atgTexture();
                                if (pTexture->Create(pImage->width, pImage->height, pImage->bpp == 24 ? TF_R8G8B8 : TF_R8G8B8A8, pImage->imageData, false))
                                {
                                    g_Renderer->InsertCacheTexture(path.C_Str(), pTexture);
                                }
                            }
                        }


                        aiUVTransform uvt;
                        if (pMaterial->Get(AI_MATKEY_UVTRANSFORM_NORMALS(0), uvt) == AI_SUCCESS)
                        {

                        }

                        pNewMaterial->AddTexture(TL_BUMPMAP,pTexture);

                        atgPass* pPass = atgShaderLibFactory::FindOrCreatePass(BUMP_MAP_PASS_IDENTITY);
                        if (pPass)
                        {
                            ((atgShaderBumpMap*)pPass)->SetMaterial(pNewMaterial);
                            pNewMaterial->SetPass(pPass);
                        }

                        if (pNewMesh->_vertices.tangent == NULL)
                        {
                            uint32 size = pMesh->mNumVertices * 4 * sizeof(float);
                            pNewMesh->_vertices.tangent = (float*)new char[size];

                            CalculateTangentArray
                            //calculateTangentSpaceVector
                                                 (  pMesh->mNumVertices, 
                                                    pNewMesh->_vertices.positions,
                                                    pNewMesh->_vertices.normals,
                                                    pNewMesh->_vertices.texureCoords,
                                                    pNewMesh->_submeshs[0]._faces.number,
                                                    pNewMesh->_submeshs[0]._faces.indices, 
                                                    pNewMesh->_vertices.tangent);

                        }
                    }
                }
            }
        }
    }

    if (pScene->HasMeshes())
    {
        LOG("has Meshes[%d].\n", pScene->mNumMeshes);
    }

    if (pScene->HasMaterials())
    {
        LOG("has number[%d] Materials.\n", pScene->mNumMaterials);
        /*
        for (uint32 i =0; i < pScene->mNumMaterials; ++i)
        {
            aiMaterial* pMaterial = pScene->mMaterials[i];

            aiString name;
            pMaterial->Get(AI_MATKEY_NAME, name);

            LOG("material[%i] name=[%s].\n", i, name.C_Str());

            for (int pi = 0, numProp = pMaterial->mNumProperties; pi < numProp; ++pi )
            {
                aiMaterialProperty* pProp = pMaterial->mProperties[pi];
                LOG("pAIMaterial[%s] Properties[%d][%s] dataType[%d] dataLength[%d]\n", name.C_Str(), pi, pProp->mKey.C_Str(), pProp->mType, pProp->mDataLength);
            }

            aiColor3D diffColor;
            if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffColor) == AI_SUCCESS)
            {

            }

            aiColor3D emisColor;
            if (pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emisColor) == AI_SUCCESS)
            {

            }

            aiString path;
            aiTextureMapping mapping;
            aiTextureOp op;
            aiTextureMapMode mapMode;
            for (uint32 i = 0; i < pMaterial->GetTextureCount(aiTextureType_DIFFUSE); ++i)
            {
                if(AI_SUCCESS == pMaterial->GetTexture(aiTextureType_DIFFUSE, 
                    i, 
                    &path, 
                    &mapping, 
                    NULL, 
                    NULL, 
                    &op,
                    &mapMode))
                {
                    aiUVTransform uvt;
                    if (pMaterial->Get(AI_MATKEY_UVTRANSFORM_DIFFUSE(0), uvt) == AI_SUCCESS)
                    {

                    }
                }
            }
        }
        */
    }

    if (pScene->HasAnimations())
    {
        LOG("has Animations.\n");
    }

    
    importer.FreeScene();

    delete [] pSourceBuffer;

    return meshs.empty();
}
