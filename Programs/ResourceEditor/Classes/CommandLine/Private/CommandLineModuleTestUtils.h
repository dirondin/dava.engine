#pragma once

#include "Base/BaseTypes.h"
#include "Render/RenderBase.h"
#include "Scene3D/Scene.h"
#include "FileSystem/FilePath.h"

#include <memory>

namespace DAVA
{
class FilePath;
};

namespace CommandLineModuleTestUtils
{
using namespace DAVA;

class TextureLoadingGuard final
{
public:
    TextureLoadingGuard(const Vector<eGPUFamily>& newLoadingOrder);
    ~TextureLoadingGuard();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

std::unique_ptr<TextureLoadingGuard> CreateTextureGuard(const Vector<eGPUFamily>& newLoadingOrder);

void CreateTestFolder(const FilePath& folder);
void ClearTestFolder(const FilePath& folder);

void CreateProjectInfrastructure(const FilePath& projectPathname);

class SceneBuilder
{
public:
    /*
    creates scene
    */
    explicit SceneBuilder(const FilePath& scenePathname, const FilePath& projectPathname = "", Scene* scene = nullptr);

    /*
    saves scene into scenePathname passed to constructor
    */
    ~SceneBuilder();

    /*
    creates scene with all objects that can be added with SceneBuilder: camera, box etc.
    */
    static void CreateFullScene(const FilePath& scenePathname, const FilePath& projectPathname = "", DAVA::Scene* scene = nullptr);
    static bool FilesIdentical(const FilePath& filePath1, const FilePath& filePath2);
    static String GetSceneRelativePathname(const FilePath& scenePath, const FilePath& dataSourcePath, const String& filename);

    enum R2OMode
    {
        WITH_REF_TO_OWNER,
        WITHOUT_REF_TO_OWNER
    };

    Entity* AddCamera(R2OMode mode = WITHOUT_REF_TO_OWNER);
    Entity* AddBox(R2OMode mode = WITHOUT_REF_TO_OWNER, const String& tag = "", const String& slotPath = "");
    Entity* AddLandscape(R2OMode mode = WITHOUT_REF_TO_OWNER);
    Entity* AddWater(R2OMode mode = WITHOUT_REF_TO_OWNER);
    Entity* AddSky(R2OMode mode = WITHOUT_REF_TO_OWNER);
    Entity* AddVegetation(R2OMode mode = WITHOUT_REF_TO_OWNER);
    Entity* AddLights(R2OMode mode = WITHOUT_REF_TO_OWNER);
    Entity* AddStaticOcclusion(R2OMode mode = WITHOUT_REF_TO_OWNER);
    Entity* AddEntityWithTestedComponents(R2OMode mode = WITHOUT_REF_TO_OWNER);

    /*
    adds 'reference to owner' scene to passed `entity`
    */
    void AddR2O(Entity* entity, const FilePath& path = "");

    const FilePath scenePathname;
    const FilePath projectPathname;
    ScopedPtr<Scene> scene;
    const static String tagChina;
    const static String tagJapan;
    const static String tagDefault;
    const static String chinaSlotDir;
    const static String defaultSlotDir;
};
};
