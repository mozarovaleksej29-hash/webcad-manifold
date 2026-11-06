#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>
#include <cstdint>
#include "manifold/manifold.h"

using namespace emscripten;
using namespace manifold;

struct MeshData {
    std::vector<float> vertices;
    std::vector<uint32_t> triangles;
    float volume;
};

// ✅ Меняем сигнатуру на val
MeshData manifoldSubtract(
    val baseVerts, val baseTris,
    val toolVerts, val toolTris
) {
    MeshData result;

    try {
        // 1. Конвертируем JS массивы в C++ векторы
        std::vector<float> baseV = convertJSArrayToNumberVector<float>(baseVerts);
        std::vector<uint32_t> baseT = convertJSArrayToNumberVector<uint32_t>(baseTris);
        std::vector<float> toolV = convertJSArrayToNumberVector<float>(toolVerts);
        std::vector<uint32_t> toolT = convertJSArrayToNumberVector<uint32_t>(toolTris);

        // 2. Создаём MeshGL
        MeshGL baseMeshGL;
        baseMeshGL.vertProperties = baseV;
        baseMeshGL.triVerts = baseT;
        baseMeshGL.numProp = 3;

        MeshGL toolMeshGL;
        toolMeshGL.vertProperties = toolV;
        toolMeshGL.triVerts = toolT;
        toolMeshGL.numProp = 3;

        // 3. Вычитаем
        Manifold baseManifold(baseMeshGL);
        Manifold toolManifold(toolMeshGL);
        Manifold resultManifold = baseManifold - toolManifold;

        // 4. Результат
        MeshGL resultMeshGL = resultManifold.GetMeshGL();

        result.vertices = resultMeshGL.vertProperties;
        result.triangles = resultMeshGL.triVerts;
        result.volume = resultManifold.Volume();

    } catch (const std::exception& e) {
        result.vertices.clear();
        result.triangles.clear();
        result.volume = 0.0f;
    }

    return result;
}

EMSCRIPTEN_BINDINGS(manifold_module) {
    value_object<MeshData>("MeshData")
        .field("vertices", &MeshData::vertices)
        .field("triangles", &MeshData::triangles)
        .field("volume", &MeshData::volume);

    register_vector<float>("VectorFloat");
    register_vector<uint32_t>("VectorUint32");

    function("subtract", &manifoldSubtract);
}
