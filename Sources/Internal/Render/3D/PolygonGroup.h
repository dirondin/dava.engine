#pragma once

#include "Base/BaseTypes.h"
#include "Base/BaseMath.h"
#include "Scene3D/DataNode.h"
#include "Scene3D/SceneFile/SerializationContext.h"
#include "Render/RHI/rhi_Public.h"

#include "MemoryManager/MemoryProfiler.h"

namespace DAVA
{
/**
    \ingroup render_3d
    \brief Group of polygons with same data type & structure
 */

class SceneFileV2;
class GeometryOctTree;
class PolygonGroup : public DataNode
{
    DAVA_ENABLE_CLASS_ALLOCATION_TRACKING(ALLOC_POOL_POLYGONGROUP)

public:
    enum VertexDataType
    {
        VERTEX_FLOAT = 1,
    };

    enum
    {
        PACKING_NONE = 0,
        PACKING_DEFAULT,
    };

    enum : uint32
    {
        TEXTURE_COORDS_COUNT = 4
    };

protected:
    virtual ~PolygonGroup();

public:
    PolygonGroup();

    //! Getters
    inline int32 GetFormat();

    inline void GetCoord(int32 i, Vector3& v);
    inline void GetNormal(int32 i, Vector3& v);
    inline void GetTangent(int32 i, Vector3& v);
    inline void GetBinormal(int32 i, Vector3& v);

    inline void GetColor(int32 i, uint32& v);
    inline void GetTexcoord(int32 ti, int32 i, Vector2& v);
    inline void GetCubeTexcoord(int32 ti, int32 i, Vector3& v);
    inline void GetIndex(int32 i, int32& index);

    inline void GetPivot(int32 i, Vector4& v);
    inline void GetPivotDeprecated(int32 i, Vector3& v);
    inline void GetFlexibility(int32 i, float32& v);
    inline void GetAngle(int32 i, Vector2& v);

    inline void GetJointIndex(int32 vIndex, int32& boneIndexValue) const;
    inline void GetJointCount(int32 vIndex, int32& jointCount) const;
    inline void GetJointWeight(int32 vIndex, float32& boneWeightValue) const;

    inline rhi::PrimitiveType GetPrimitiveType();

    //! Setters
    inline void SetCoord(int32 i, const Vector3& v);
    inline void SetNormal(int32 i, const Vector3& v);
    inline void SetTangent(int32 i, const Vector3& v);
    inline void SetBinormal(int32 i, const Vector3& v);

    inline void SetColor(int32 i, const uint32& c);
    inline void SetTexcoord(int32 ti, int32 i, const Vector2& v);
    inline void SetCubeTexcoord(int32 ti, int32 i, const Vector3& v);
    inline void SetJointIndex(int32 vIndex, int32 jointIndex, int32 boneIndexValue);
    inline void SetJointCount(int32 vIndex, int32 jointCount);
    inline void SetJointWeight(int32 vIndex, int32 jointIndex, float32 boneWeightValue);

    inline void SetIndex(int32 i, int16 index);

    inline void SetPivot(int32 i, const Vector4& v);
    inline void SetPivotDeprecated(int32 i, const Vector3& v);
    inline void SetFlexibility(int32 i, const float32& v);
    inline void SetAngle(int32 i, const Vector2& v);

    inline int32 GetVertexCount();
    inline int32 GetIndexCount();
    inline int32 GetPrimitiveCount();

    inline const AABBox3& GetBoundingBox() const;

    inline void SetPrimitiveType(rhi::PrimitiveType type);

    inline void GetTriangleIndices(int32 firstIndex, uint16 indices[3]);

    int32 vertexCount;
    int32 indexCount;
    int32 textureCoordCount;
    int32 vertexStride;
    int32 vertexFormat;
    int32 indexFormat;
    int32 primitiveCount;
    rhi::PrimitiveType primitiveType;
    int32 cubeTextureCoordCount;

    Vector3* vertexArray;
    Vector2* textureCoordArray[TEXTURE_COORDS_COUNT];
    Vector3* normalArray;
    Vector3* tangentArray;
    Vector3* binormalArray;
    float32* jointIdxArray;
    float32* jointWeightArray;
    Vector3** cubeTextureCoordArray;

    uint32* jointCountArray;

    Vector4* pivot4Array;
    Vector3* pivotArray;
    float32* flexArray;
    Vector2* angleArray;

    uint32* colorArray;
    int16* indexArray; // Boroda: why int16? should be uint16?
    uint8* meshData;

    AABBox3 aabbox;

    void GenerateGeometryOctTree();
    GeometryOctTree* GetGeometryOctTree();
    GeometryOctTree* GetGeometryOctTree() const;
    GeometryOctTree* octTree = nullptr;

    /*
        Used for animated meshes to hold original vertexes in array that suitable for fast access
     */
    void CreateBaseVertexArray();
    Vector3* baseVertexArray;

    //meshFormat is EVF_VERTEX etc.
    void AllocateData(int32 meshFormat, int32 vertexCount, int32 indexCount, int32 primitiveCount = 0);
    void ReleaseData();
    void RecalcAABBox();

    uint32 ReleaseGeometryData();

    /*
        Apply matrix to polygon group. If polygon group is used with vertex buffers
        you should call BuildBuffers to refresh buffers in memory.
        TODO: refresh buffers function???
     */
    void ApplyMatrix(const Matrix4& matrix);

    void BuildBuffers();
    void RestoreBuffers();

    void Save(KeyedArchive* keyedArchive, SerializationContext* serializationContext) override;
    void LoadPolygonData(KeyedArchive* keyedArchive, SerializationContext* serializationContext, int32 requiredFlags, bool cutUnusedStreams);

    static void CopyData(const uint8** meshData, uint8** newMeshData, uint32 vertexFormat, uint32 newVertexFormat, uint32 format);

    rhi::HVertexBuffer vertexBuffer;
    rhi::HIndexBuffer indexBuffer;
    uint32 vertexLayoutId;

private:
    void UpdateDataPointersAndStreams();

    template <class T>
    void SetVertexData(int32 i, T* basePtr, const T& value);

    template <class T>
    T& GetVertexData(int32 i, T* base);

    DAVA_VIRTUAL_REFLECTION(PolygonGroup, DataNode);
};

template <class T>
inline void PolygonGroup::SetVertexData(int32 i, T* base, const T& value)
{
    DVASSERT(i >= 0);
    DVASSERT(base != nullptr);

    ptrdiff_t baseOffset = reinterpret_cast<uint8*>(base) - meshData;
    DVASSERT(baseOffset >= 0);
    DVASSERT(baseOffset < vertexStride);

    uint8* ptr = meshData + baseOffset + i * vertexStride;
    DVASSERT(ptr < meshData + vertexCount * vertexStride);

    T* typePtr = reinterpret_cast<T*>(ptr);
    *typePtr = value;
}

template <class T>
T& PolygonGroup::GetVertexData(int32 i, T* base)
{
    DVASSERT(i >= 0);
    DVASSERT(base != nullptr);

    ptrdiff_t baseOffset = reinterpret_cast<uint8*>(base) - meshData;
    DVASSERT(baseOffset >= 0);
    DVASSERT(baseOffset < vertexStride);

    uint8* ptr = meshData + baseOffset + i * vertexStride;
    DVASSERT(ptr < meshData + vertexCount * vertexStride);

    return *(reinterpret_cast<T*>(ptr));
}

inline void PolygonGroup::SetCoord(int32 i, const Vector3& _v)
{
    SetVertexData(i, vertexArray, _v);
    aabbox.AddPoint(_v);
}

inline void PolygonGroup::SetNormal(int32 i, const Vector3& _v)
{
    Vector3 vn = _v;
    vn.Normalize();
    SetVertexData(i, normalArray, vn);
}

inline void PolygonGroup::SetTangent(int32 i, const Vector3& _v)
{
    SetVertexData(i, tangentArray, _v);
}

inline void PolygonGroup::SetBinormal(int32 i, const Vector3& _v)
{
    SetVertexData(i, binormalArray, _v);
}

inline void PolygonGroup::SetColor(int32 i, const uint32& _c)
{
    SetVertexData(i, colorArray, _c);
}

inline void PolygonGroup::SetPivot(int32 i, const Vector4& _v)
{
    SetVertexData(i, pivot4Array, _v);
}

inline void PolygonGroup::SetPivotDeprecated(int32 i, const Vector3& _v)
{
    SetVertexData(i, pivotArray, _v);
}

inline void PolygonGroup::SetFlexibility(int32 i, const float32& _v)
{
    SetVertexData(i, flexArray, _v);
}

inline void PolygonGroup::SetAngle(int32 i, const Vector2& _v)
{
    SetVertexData(i, angleArray, _v);
}

inline void PolygonGroup::SetTexcoord(int32 ti, int32 i, const Vector2& _t)
{
    DVASSERT(ti < TEXTURE_COORDS_COUNT);
    SetVertexData(i, textureCoordArray[ti], _t);
}

inline void PolygonGroup::SetCubeTexcoord(int32 ti, int32 i, const Vector3& _t)
{
    DVASSERT(ti < cubeTextureCoordCount);
    SetVertexData(i, cubeTextureCoordArray[ti], _t);
}

inline void PolygonGroup::SetJointIndex(int32 i, int32 jointIndex, int32 boneIndexValue)
{
    DVASSERT(jointIndex >= 0 && jointIndex < 4);
    SetVertexData(i, jointIdxArray, static_cast<float32>(boneIndexValue));
}

inline void PolygonGroup::SetJointWeight(int32 i, int32 jointIndex, float32 boneWeightValue)
{
    DVASSERT(jointIndex >= 0 && jointIndex < 4);
    SetVertexData(i, jointWeightArray, boneWeightValue);
}

inline void PolygonGroup::SetJointCount(int32 vIndex, int32 jointCount)
{
    jointCountArray[vIndex] = jointCount;
}

inline void PolygonGroup::GetJointIndex(int32 vIndex, int32& boneIndexValue) const
{
    boneIndexValue = static_cast<int32>(*reinterpret_cast<float32*>(reinterpret_cast<uint8*>(jointIdxArray) + vIndex * vertexStride));
}

inline void PolygonGroup::GetJointWeight(int32 vIndex, float32& boneWeightValue) const
{
    boneWeightValue = *reinterpret_cast<float32*>(reinterpret_cast<uint8*>(jointWeightArray) + vIndex * vertexStride);
}

inline void PolygonGroup::GetJointCount(int32 vIndex, int32& jointCount) const
{
    jointCount = jointCountArray[vIndex];
}

inline void PolygonGroup::SetIndex(int32 i, int16 index)
{
    indexArray[i] = index;
}

inline void PolygonGroup::SetPrimitiveType(rhi::PrimitiveType type)
{
    primitiveType = type;
}

inline int32 PolygonGroup::GetFormat()
{
    return vertexFormat;
}

inline void PolygonGroup::GetCoord(int32 i, Vector3& _v)
{
    _v = GetVertexData(i, vertexArray);
}

inline void PolygonGroup::GetNormal(int32 i, Vector3& _v)
{
    _v = GetVertexData(i, normalArray);
}

inline void PolygonGroup::GetTangent(int32 i, Vector3& _v)
{
    _v = GetVertexData(i, tangentArray);
}

inline void PolygonGroup::GetBinormal(int32 i, Vector3& _v)
{
    _v = GetVertexData(i, binormalArray);
}

inline void PolygonGroup::GetColor(int32 i, uint32& _v)
{
    _v = GetVertexData(i, colorArray);
}

inline void PolygonGroup::GetTexcoord(int32 ti, int32 i, Vector2& _v)
{
    DVASSERT(ti < TEXTURE_COORDS_COUNT);
    _v = GetVertexData(i, textureCoordArray[ti]);
}

inline void PolygonGroup::GetCubeTexcoord(int32 ti, int32 i, Vector3& _v)
{
    DVASSERT(ti < cubeTextureCoordCount);
    _v = GetVertexData(i, cubeTextureCoordArray[ti]);
}

inline void PolygonGroup::GetPivot(int32 i, Vector4& _v)
{
    _v = GetVertexData(i, pivot4Array);
}

inline void PolygonGroup::GetPivotDeprecated(int32 i, Vector3& _v)
{
    _v = GetVertexData(i, pivotArray);
}

inline void PolygonGroup::GetFlexibility(int32 i, float32& _v)
{
    _v = GetVertexData(i, flexArray);
}

inline void PolygonGroup::GetAngle(int32 i, Vector2& _v)
{
    _v = GetVertexData(i, angleArray);
}

inline void PolygonGroup::GetIndex(int32 i, int32& index)
{
    index = uint16(indexArray[i]);
}

inline int32 PolygonGroup::GetVertexCount()
{
    return vertexCount;
}
inline int32 PolygonGroup::GetIndexCount()
{
    return indexCount;
}
inline int32 PolygonGroup::GetPrimitiveCount()
{
    return primitiveCount;
}
inline const AABBox3& PolygonGroup::GetBoundingBox() const
{
    return aabbox;
}
inline rhi::PrimitiveType PolygonGroup::GetPrimitiveType()
{
    return primitiveType;
}

inline GeometryOctTree* PolygonGroup::GetGeometryOctTree()
{
    if (octTree == nullptr)
        GenerateGeometryOctTree();

    return octTree;
}

inline GeometryOctTree* PolygonGroup::GetGeometryOctTree() const
{
    return octTree;
}

inline void PolygonGroup::GetTriangleIndices(int32 firstIndex, uint16 indices[3])
{
    indices[0] = static_cast<uint16>(indexArray[firstIndex]);
    indices[1] = static_cast<uint16>(indexArray[firstIndex + 1]);
    indices[2] = static_cast<uint16>(indexArray[firstIndex + 2]);
}
}
