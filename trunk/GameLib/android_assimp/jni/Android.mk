
#一个Android.mk文件必须以定义一个LOCAL_PATH变量
LOCAL_PATH := $(call my-dir)/..

#清理很多LOCAL_PATH中的LOCAL_XXX变量
include $(CLEAR_VARS)               

$(call import-add-path,$(LOCAL_PATH)/include/assimp)
$(call import-add-path,$(LOCAL_PATH)/include/assimp/Compiler)
$(call import-add-path,$(LOCAL_PATH)/code)
$(call import-add-path,$(LOCAL_PATH)/code/BoostWorkaround)
$(call import-add-path,$(LOCAL_PATH)/contrib/unzip)

#模块名，名字必须是唯一的而且不能有空格
LOCAL_MODULE    := assimp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/code/BoostWorkaround


#包含将要被编译和汇编进module的一个C/C++资源文件列表
LOCAL_SRC_FILES := \
    code/Assimp.cpp \
	code/AssimpPCH.cpp \
	code/AssimpPCH.h \
	code/BoostWorkaround/boost/math/common_factor_rt.hpp \
	code/BoostWorkaround/boost/foreach.hpp \
	code/BoostWorkaround/boost/format.hpp \
	code/BoostWorkaround/boost/scoped_array.hpp \
	code/BoostWorkaround/boost/scoped_ptr.hpp \
	code/BoostWorkaround/boost/shared_array.hpp \
	code/BoostWorkaround/boost/shared_ptr.hpp \
	code/BoostWorkaround/boost/make_shared.hpp \
	code/BoostWorkaround/boost/static_assert.hpp \
	code/BoostWorkaround/boost/tuple/tuple.hpp \
	code/fast_atof.h \
	code/qnan.h \
	code/BaseImporter.cpp \
	code/BaseImporter.h \
	code/BaseProcess.cpp \
	code/BaseProcess.h \
	code/Importer.h \
	code/ScenePrivate.h \
	code/PostStepRegistry.cpp \
	code/ImporterRegistry.cpp \
	code/ByteSwap.h \
	code/DefaultProgressHandler.h \
	code/DefaultIOStream.cpp \
	code/DefaultIOStream.h \
	code/DefaultIOSystem.cpp \
	code/DefaultIOSystem.h \
	code/CInterfaceIOWrapper.h \
	code/Hash.h \
	code/Importer.cpp \
	code/IFF.h \
	code/ParsingUtils.h \
	code/StdOStreamLogStream.h \
	code/StreamReader.h \
	code/StringComparison.h \
	code/SGSpatialSort.cpp \
	code/SGSpatialSort.h \
	code/VertexTriangleAdjacency.cpp \
	code/VertexTriangleAdjacency.h \
	code/GenericProperty.h \
	code/SpatialSort.cpp \
	code/SpatialSort.h \
	code/SceneCombiner.cpp \
	code/SceneCombiner.h \
	code/ScenePreprocessor.cpp \
	code/ScenePreprocessor.h \
	code/SkeletonMeshBuilder.cpp \
	code/SkeletonMeshBuilder.h \
	code/SplitByBoneCountProcess.cpp \
	code/SplitByBoneCountProcess.h \
	code/SmoothingGroups.h \
	code/StandardShapes.cpp \
	code/StandardShapes.h \
	code/TargetAnimation.cpp \
	code/TargetAnimation.h \
	code/RemoveComments.cpp \
	code/RemoveComments.h \
	code/Subdivision.cpp \
	code/Subdivision.h \
	code/Vertex.h \
	code/LineSplitter.h \
	code/TinyFormatter.h \
	code/Profiler.h \
	code/LogAux.h \
	code/Bitmap.cpp \
	code/Bitmap.h \
	code/ObjFileData.h \
	code/ObjFileImporter.cpp \
	code/ObjFileImporter.h \
	code/ObjFileMtlImporter.cpp \
	code/ObjFileMtlImporter.h \
	code/ObjFileParser.cpp \
	code/ObjFileParser.h \
	code/ObjTools.h \
	code/ObjExporter.h \
	code/ObjExporter.cpp \
	code/CalcTangentsProcess.cpp \
	code/CalcTangentsProcess.h \
	code/ComputeUVMappingProcess.cpp \
	code/ComputeUVMappingProcess.h \
	code/ConvertToLHProcess.cpp \
	code/ConvertToLHProcess.h \
	code/FindDegenerates.cpp \
	code/FindDegenerates.h \
	code/FindInstancesProcess.cpp \
	code/FindInstancesProcess.h \
	code/FindInvalidDataProcess.cpp \
	code/FindInvalidDataProcess.h \
	code/FixNormalsStep.cpp \
	code/FixNormalsStep.h \
	code/GenFaceNormalsProcess.cpp \
	code/GenFaceNormalsProcess.h \
	code/GenVertexNormalsProcess.cpp \
	code/GenVertexNormalsProcess.h \
	code/PretransformVertices.cpp \
	code/PretransformVertices.h \
	code/ImproveCacheLocality.cpp \
	code/ImproveCacheLocality.h \
	code/JoinVerticesProcess.cpp \
	code/JoinVerticesProcess.h \
	code/LimitBoneWeightsProcess.cpp \
	code/LimitBoneWeightsProcess.h \
	code/RemoveRedundantMaterials.cpp \
	code/RemoveRedundantMaterials.h \
	code/RemoveVCProcess.cpp \
	code/RemoveVCProcess.h \
	code/SortByPTypeProcess.cpp \
	code/SortByPTypeProcess.h \
	code/SplitLargeMeshes.cpp \
	code/SplitLargeMeshes.h \
	code/TerragenLoader.cpp \
	code/TerragenLoader.h \
	code/TextureTransform.cpp \
	code/TextureTransform.h \
	code/TriangulateProcess.cpp \
	code/TriangulateProcess.h \
	code/ValidateDataStructure.cpp \
	code/ValidateDataStructure.h \
	code/OptimizeGraph.cpp \
	code/OptimizeGraph.h \
	code/OptimizeMeshes.cpp \
	code/OptimizeMeshes.h \
	code/DeboneProcess.cpp \
	code/DeboneProcess.h \
	code/ProcessHelper.h \
	code/ProcessHelper.cpp \
	code/PolyTools.h \
	code/MakeVerboseFormat.cpp \
	code/MakeVerboseFormat.h \
	code/MaterialSystem.cpp \
	code/MaterialSystem.h \
	code/DefaultLogger.cpp \
	contrib/ConvertUTF/ConvertUTF.h \
	contrib/ConvertUTF/ConvertUTF.c \
	code/FBXImporter.cpp \
	code/FBXCompileConfig.h \
	code/FBXImporter.h \
	code/FBXParser.cpp \
	code/FBXParser.h \
	code/FBXTokenizer.cpp \
	code/FBXTokenizer.h \
	code/FBXImportSettings.h \
	code/FBXConverter.h \
	code/FBXConverter.cpp \
	code/FBXUtil.h \
	code/FBXUtil.cpp \
	code/FBXDocument.h \
	code/FBXDocument.cpp \
	code/FBXProperties.h \
	code/FBXProperties.cpp \
	code/FBXMeshGeometry.cpp \
	code/FBXMaterial.cpp \
	code/FBXModel.cpp \
	code/FBXAnimation.cpp \
	code/FBXNodeAttribute.cpp \
	code/FBXDeformer.cpp \
	code/FBXBinaryTokenizer.cpp \
	code/FBXDocumentUtil.cpp

 #这里加入必要的编译选项
LOCAL_CFLAGS    := -fexceptions -DASSIMP_BUILD_NO_X_IMPORTER \
-DASSIMP_BUILD_NO_3DS_IMPORTER \
-DASSIMP_BUILD_NO_MD3_IMPORTER \
-DASSIMP_BUILD_NO_MD2_IMPORTER \
-DASSIMP_BUILD_NO_PLY_IMPORTER \
-DASSIMP_BUILD_NO_MDL_IMPORTER \
-DASSIMP_BUILD_NO_ASE_IMPORTER \
-DASSIMP_BUILD_NO_HMP_IMPORTER \
-DASSIMP_BUILD_NO_SMD_IMPORTER \
-DASSIMP_BUILD_NO_MDC_IMPORTER \
-DASSIMP_BUILD_NO_MD5_IMPORTER \
-DASSIMP_BUILD_NO_STL_IMPORTER \
-DASSIMP_BUILD_NO_LWO_IMPORTER \
-DASSIMP_BUILD_NO_DXF_IMPORTER \
-DASSIMP_BUILD_NO_NFF_IMPORTER \
-DASSIMP_BUILD_NO_RAW_IMPORTER \
-DASSIMP_BUILD_NO_OFF_IMPORTER \
-DASSIMP_BUILD_NO_AC_IMPORTER \
-DASSIMP_BUILD_NO_BVH_IMPORTER \
-DASSIMP_BUILD_NO_IRRMESH_IMPORTER \
-DASSIMP_BUILD_NO_IRR_IMPORTER \
-DASSIMP_BUILD_NO_Q3D_IMPORTER \
-DASSIMP_BUILD_NO_B3D_IMPORTER \
-DASSIMP_BUILD_NO_COLLADA_IMPORTER \
-DASSIMP_BUILD_NO_TERRAGEN_IMPORTER \
-DASSIMP_BUILD_NO_CSM_IMPORTER \
-DASSIMP_BUILD_NO_3D_IMPORTER \
-DASSIMP_BUILD_NO_LWS_IMPORTER \
-DASSIMP_BUILD_NO_OGRE_IMPORTER \
-DASSIMP_BUILD_NO_MS3D_IMPORTER \
-DASSIMP_BUILD_NO_COB_IMPORTER \
-DASSIMP_BUILD_NO_BLEND_IMPORTER \
-DASSIMP_BUILD_NO_Q3BSP_IMPORTER \
-DASSIMP_BUILD_NO_NDO_IMPORTER \
-DASSIMP_BUILD_NO_IFC_IMPORTER \
-DASSIMP_BUILD_NO_XGL_IMPORTER

#-DASSIMP_BUILD_NO_FBX_IMPORTER
#-DASSIMP_BUILD_NO_OBJ_IMPORTER

#生成一个静态库目标 
include $(BUILD_STATIC_LIBRARY)



