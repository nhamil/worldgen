#pragma once 

#include <Fjord/Fjord.h> 
#include <Fjord/Graphics/Color.h> 
#include <Fjord/Graphics/Types.h> 
#include <Fjord/Math/Vector3.h> 
#include <Fjord/Math/Quaternion.h> 
#include <Fjord/Util/KDTree.h> 

using namespace Fjord; 

static const unsigned MaxCellNeighborCount = 8; 

using CellId = unsigned; 

enum class Terrain 
{
    Invalid = 0, 

    DeepWater, 
    ShallowWater, 

    Grassland, 
    Desert, 
    Forest, 
    Rainforest, 
    Tundra, 
    Snow, 

    Mountain, 
    Ice 
};

Color GetTerrainColor(Terrain t); 

bool IsTerrainWater(Terrain t); 
bool IsTerrainLand(Terrain t); 

struct GeoCoords 
{
    float Latitude; 
    float Longitude; 
};

GeoCoords GetGeoCoordsFromPosition(const Vector3& pos); 

Vector3 GetPositionFromGeoCoords(const GeoCoords& coords); 

struct Cell 
{
    Quaternion OceanCurrent = Quaternion::Identity; 
    Quaternion WindCurrent = Quaternion::Identity; 
    Vector3 Position = Vector3::Zero; 
    Vector<CellId> Connections; 
    float AirPressure = 0; 
    float Height = 0; 
    float Heat = 0; 
    float Moisture = 0; 
    enum Terrain Terrain = Terrain::Invalid; 
};

class World 
{
public: 
    World(); 
    ~World(); 

    unsigned GetCellCount() const; 
    CellId AddCell(const Vector3& position); 

    CellId GetCellIdPyPosition(const Vector3& position); 
    CellId GetCellIdPyPosition(const Vector3& position) const; 

    unsigned GetConnectionCount() const; 
    bool HasConnection(CellId a, CellId b) const; 
    void AddConnection(CellId a, CellId b); 
    void RemoveConnection(CellId a, CellId b); 

    /* Cell Get Properties */ 

    Vector3 GetPosition(CellId id) const; 
    float GetGeoCoords(CellId id) const; 

    Quaternion GetOceanCurrent(CellId id) const; 
    Quaternion GetWindCurrent(CellId id) const; 

    float GetHeight(CellId id) const; 
    float GetHeat(CellId id) const; 
    float GetMoisture(CellId id) const; 

    Terrain GetTerrain(CellId id) const; 

    unsigned GetNeighborCount(CellId id) const; 
    CellId GetNeighbor(CellId id, unsigned index) const; 

    Vector<Vector3> GetBounds(CellId id) const; 

    /* Cell Set Properties */ 

    void SetPosition(CellId id, const Vector3& position); 

    void SetOceanCurrent(CellId id, const Quaternion& rotation); 
    void SetWindCurrent(CellId id, const Quaternion& rotation); 

    void SetHeight(CellId id, float height); 
    void SetHeat(CellId id, float heat); 
    void SetMoisture(CellId id, float moisture); 

    void SetTerrain(CellId, Terrain terrain); 

    /* Utilities */ 

    void UpdateSpatialGeometry(); 

private: 
    Vector<Cell> Cells_; 
    HashSet<Pair<CellId, CellId>, PairHash> Connections_; 
    KDTree3<CellId> SearchTree_; 
    bool UpdateTree_ = true; 
};