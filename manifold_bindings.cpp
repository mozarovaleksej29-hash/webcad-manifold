#include <emscripten/bind.h>
#include <vector>
#include <cstdint>
#include "manifold/manifold.h"

using namespace emscripten;
using namespace manifold;

// Структура для возврата в JS
struct MeshData {
    std::vector<float> vertices;
    std::vector<uint32_t> triangles;
    float volume;
};

MeshData manifoldSubtract(
    std::vector<float> baseVerts, std::vector<uint32_t> baseTris,
    std::vector<float> toolVerts, std::vector<uint32_t> toolTris
) {
    MeshData result;

    try {
        // 1. Создаём MeshGL из входных данных
        MeshGL baseMeshGL;
        baseMeshGL.vertProperties = baseVerts;
        baseMeshGL.triVerts = baseTris;
        baseMeshGL.numProp = 3;

        MeshGL toolMeshGL;
        toolMeshGL.vertProperties = toolVerts;
        toolMeshGL.triVerts = toolTris;
        toolMeshGL.numProp = 3;

        // 2. Создаём Manifold-объекты
        Manifold baseManifold(baseMeshGL);
        Manifold toolManifold(toolMeshGL);

        // 3. Вычитаем
        Manifold resultManifold = baseManifold - toolManifold;

        // 4. Получаем результат
        MeshGL resultMeshGL = resultManifold.GetMeshGL();

        // 5. Заполняем нашу структуру
        result.vertices = resultMeshGL.vertProperties;
        result.triangles = resultMeshGL.triVerts;
        result.volume = resultManifold.Volume();

    } catch (const std::exception& e) {
        // Если что-то пошло не так — возвращаем пустоту
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
