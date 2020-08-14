#include "WorldGen/World.h" 

#include <algorithm> 
#include <cfloat> 

Color GetTerrainColor(Terrain t) 
{
    switch (t) 
    {
    case Terrain::DeepWater: return {0, 0, 0.7}; 
    case Terrain::ShallowWater: return {0.0, 0.3, 0.7}; 
    case Terrain::Grassland: return {0.25, 0.7, 0.1}; 
    case Terrain::Desert: return {0.7, 0.7, 0.3}; 
    case Terrain::Forest: return {0, 0.5, 0}; 
    case Terrain::Rainforest: return {0, 0.25, 0}; 
    case Terrain::Tundra: return {0.5, 0.5, 0.2}; 
    case Terrain::Snow: return {0.95, 0.9, 0.95}; 
    case Terrain::Mountain: return {0.35, 0.35, 0.35}; 
    case Terrain::Ice: return {1, 1, 1}; 
    default: 
        return {1, 0, 1}; 
    }
}

bool IsTerrainWater(Terrain t) 
{
    return t == Terrain::DeepWater || t == Terrain::ShallowWater; 
}

bool IsTerrainLand(Terrain t) 
{
    return !IsTerrainWater(t) && t != Terrain::Invalid && t != Terrain::Ice; 
}

GeoCoords GetGeoCoordsFromPosition(const Vector3& pos) 
{
    return {
        90.0 - std::acos(pos.Y / Length(pos)) * FJ_TO_DEG, 
        std::atan2(pos.X, pos.Z) * FJ_TO_DEG 
    };
}

Vector3 GetPositionFromGeoCoords(const GeoCoords& coord) 
{
    Vector3 out; 
    float lat = -coord.Latitude + 90; 
    float lon = coord.Longitude; 
    out.Y = std::cos(lat * FJ_TO_RAD); 
    float s = std::sin(lat * FJ_TO_RAD); 
    out.X = s * std::sin(lon * FJ_TO_RAD); 
    out.Z = s * std::cos(lon * FJ_TO_RAD); 
    return Normalized(out); 
}

World::World() 
{

} 

World::~World() 
{

}

unsigned World::GetCellCount() const 
{
    return Cells_.size(); 
}

unsigned World::GetConnectionCount() const 
{
    return Connections_.size(); 
}

void World::AddConnection(CellId a, CellId b) 
{
    FJ_ASSERT(a < GetCellCount()); 
    FJ_ASSERT(b < GetCellCount()); 
    bool existed = !Connections_.insert(MakeSortedPair(a, b)).second; 
    if (!existed) 
    {
        Cells_[a].Connections.push_back(b); 
        Cells_[b].Connections.push_back(a); 
    }
}

void World::RemoveConnection(CellId a, CellId b) 
{
    FJ_ASSERT(a < GetCellCount()); 
    FJ_ASSERT(b < GetCellCount()); 

    auto it = Connections_.find(MakeSortedPair(a, b)); 
    if (it != Connections_.end()) 
    {
        Connections_.erase(it); 

        auto removeVal = [](Vector<CellId>& list, CellId val) -> void 
        {
            for (auto it = list.begin(); it != list.end(); it++) 
            {
                if (*it == val) 
                {
                    list.erase(it); 
                    break; 
                }
            }
        }; 

        removeVal(Cells_[a].Connections, b); 
        removeVal(Cells_[b].Connections, a); 
    }
}

bool World::HasConnection(CellId a, CellId b) const
{
    return Connections_.find(MakeSortedPair(a, b)) != Connections_.end(); 
}

CellId World::GetCellIdPyPosition(const Vector3& position)  
{
    UpdateSpatialGeometry(); 
    return SearchTree_.Search(Normalized(position)); 
}

CellId World::GetCellIdPyPosition(const Vector3& position) const 
{
    FJ_ASSERT(!UpdateTree_); 
    return SearchTree_.Search(Normalized(position)); 
}

TriCellId World::GetTriCellIdPyPosition(const Vector3& position) 
{
    UpdateSpatialGeometry(); 

    Vector3 pos = Normalized(position); 

    CellId cell = GetCellIdPyPosition(pos);

    Vector<CellId> bounds = GetOrderedNeighbors(cell); 
    Vector3 aPos = GetPosition(cell); 

    TriCellId tc; 
    float minimum = -INFINITY; 
    unsigned iMin = 0; 
    for (unsigned i = 0; i < bounds.size(); i++) 
    {
        Vector3 bPos = GetPosition(bounds[i]); 
        Vector3 cPos = GetPosition(bounds[(i+1)%bounds.size()]); 
        Vector3 bary = GetBarycentricCoords(pos, aPos, bPos, cPos); 
        if (bary.X >= 0 && bary.Y >= 0 && bary.Z >= 0) 
        {
            tc.A = cell; 
            tc.B = bounds[i]; 
            tc.C = bounds[(i+1)%bounds.size()]; 
            break; 
        }
        else 
        {
            float minLast = minimum; 
            for (int j = 0; j < 3; j++) if (bary[j] < 0) minimum = Max(minimum, bary[j]); 
            if (minLast != minimum) 
            {
                iMin = i; 
            }

            if (i == bounds.size() - 1) 
            {
                // FJ_FWARN("Invalid barycentric coordinates! %f", minimum); 
                tc.A = cell; 
                tc.B = bounds[iMin]; 
                tc.C = bounds[(iMin+1)%bounds.size()]; 
            }
        }
    }

    // Vector<CellId> ids; 
    // SearchTree_.Search(pos, 3, ids); 

    // TriCellId tc; 
    // tc.A = ids[0]; 
    // tc.B = ids[1]; 
    // tc.C = ids[2]; 
    tc.Coords = GetBarycentricCoords(pos, 
        Cells_[tc.A].Position, 
        Cells_[tc.B].Position, 
        Cells_[tc.C].Position, 
        true
    ); 
    return tc; 
}

TriCellId World::GetTriCellIdPyPosition(const Vector3& position) const 
{
    FJ_ASSERT(false); 
    // FJ_ASSERT(!UpdateTree_); 

    // Vector3 pos = Normalized(position); 

    // Vector<CellId> ids; 
    // SearchTree_.Search(pos, 3, ids); 

    // TriCellId tc; 
    // tc.A = ids[0]; 
    // tc.B = ids[1]; 
    // tc.C = ids[2]; 
    // tc.Coords = GetBarycentricCoords(pos, 
    //     Cells_[tc.A].Position, 
    //     Cells_[tc.B].Position, 
    //     Cells_[tc.C].Position 
    // ); 
    // return tc; 
}

void World::UpdateSpatialGeometry() 
{
    if (!UpdateTree_) return; 

    UpdateTree_ = false; 
    SearchTree_.Clear(); 
    for (unsigned i = 0; i < GetCellCount(); i++) 
    {
        SearchTree_.Insert(GetPosition(i), i); 
    }
}

unsigned World::GetNeighborCount(CellId id) const 
{
    FJ_ASSERT(id < GetCellCount()); 
    return Cells_[id].Connections.size(); 
}

CellId World::GetNeighbor(CellId id, unsigned index) const 
{
    FJ_ASSERT(id < GetCellCount()); 
    FJ_ASSERT(index < GetNeighborCount(id)); 
    return Cells_[id].Connections[index]; 
}

static bool CCW(Vector3 a, Vector3 b, Vector3 c) 
{
    Vector3 n = Cross(b - a, c - a); 
    return Dot(n, a) < 0; 
}

Vector<CellId> World::GetOrderedNeighbors(CellId id) const 
{
    Vector<CellId> bounds; 

    if (GetNeighborCount(id) > 0) 
    {
        CellId last = GetNeighbor(id, 0); 
        HashSet<CellId> used; 
        used.insert(last); 
        while (bounds.size() < GetNeighborCount(id) - 1) 
        {
            for (unsigned i = 0; i < GetNeighborCount(id); i++) 
            {
                CellId c = GetNeighbor(id, i); 
                if (HasConnection(last, c) && used.find(c) == used.end()) 
                {
                    // Vector3 pos = GetPosition(id); 
                    // Vector3 cPos = GetPosition(c); 
                    // Vector3 lastPos = GetPosition(last); 
                    bounds.push_back(c); 
                    last = c; 
                    used.insert(c); 
                }
            }
        }
        // Vector3 pos = GetPosition(id); 
        // Vector3 firstPos = GetPosition(GetNeighbor(id, 0)); 
        // Vector3 lastPos = GetPosition(last); 
        bounds.push_back(GetNeighbor(id, 0)); 

        // if (CCW(bounds[0], bounds[1], bounds[2]) > 0) 
        // {
        //     // FJ_LOG(Debug, "reverse"); 
        //     // std::reverse(bounds.begin(), bounds.end()); 
        //     Vector<Vector3> list; 
        //     for (auto it = bounds.rbegin(); it != bounds.rend(); it++) 
        //     {
        //         list.push_back(*it); 
        //     }
        //     return list; 
        // }
    }

    return bounds; 
}

Vector<Vector3> World::GetBounds(CellId id) const 
{
    Vector<Vector3> bounds; 

    if (GetNeighborCount(id) > 0) 
    {
        CellId last = GetNeighbor(id, 0); 
        HashSet<CellId> used; 
        used.insert(last); 
        while (bounds.size() < GetNeighborCount(id) - 1) 
        {
            for (unsigned i = 0; i < GetNeighborCount(id); i++) 
            {
                CellId c = GetNeighbor(id, i); 
                if (HasConnection(last, c) && used.find(c) == used.end()) 
                {
                    Vector3 pos = GetPosition(id); 
                    Vector3 cPos = GetPosition(c); 
                    Vector3 lastPos = GetPosition(last); 
                    bounds.push_back(Normalized((pos + cPos + lastPos) * (1.0/3))); 
                    last = c; 
                    used.insert(c); 
                }
            }
        }
        Vector3 pos = GetPosition(id); 
        Vector3 firstPos = GetPosition(GetNeighbor(id, 0)); 
        Vector3 lastPos = GetPosition(last); 
        bounds.push_back(Normalized((pos + firstPos + lastPos) * (1.0/3))); 

        if (CCW(bounds[0], bounds[1], bounds[2]) > 0) 
        {
            // FJ_LOG(Debug, "reverse"); 
            // std::reverse(bounds.begin(), bounds.end()); 
            Vector<Vector3> list; 
            for (auto it = bounds.rbegin(); it != bounds.rend(); it++) 
            {
                list.push_back(*it); 
            }
            return list; 
        }
    }

    return bounds; 
}

CellId World::AddCell(const Vector3& position) 
{
    UpdateTree_ = true; 
    Cell cell; 
    cell.Position = Normalized(position); 
    // TODO geo coords 
    Cells_.push_back(cell); 
    return Cells_.size() - 1; 
}

Vector3 World::GetPosition(CellId id) const 
{
    FJ_ASSERT_RET(id < GetCellCount(), Vector3::Zero); 
    return Cells_[id].Position; 
}

void World::SetPosition(CellId id, const Vector3& position) 
{
    UpdateTree_ = true; 
    FJ_ASSERT(id < GetCellCount()); 
    Cells_[id].Position = Normalized(position); 
    // TODO update geo coords 
}

float World::GetHeight(CellId id) const 
{
    FJ_ASSERT_RET(id < GetCellCount(), 0.0);
    return Cells_[id].Height; 
}

void World::SetHeight(CellId id, float height)  
{
    FJ_ASSERT(id < GetCellCount());
    Cells_[id].Height = height; 
}

float World::GetHeat(CellId id) const 
{
    FJ_ASSERT_RET(id < GetCellCount(), 0.0);
    return Cells_[id].Heat; 
}

void World::SetHeat(CellId id, float heat)  
{
    FJ_ASSERT(id < GetCellCount());
    Cells_[id].Heat = heat; 
}

float World::GetMoisture(CellId id) const 
{
    FJ_ASSERT_RET(id < GetCellCount(), 0.0);
    return Cells_[id].Moisture; 
}

void World::SetMoisture(CellId id, float moisture)  
{
    FJ_ASSERT(id < GetCellCount());
    Cells_[id].Moisture = moisture; 
}

Terrain World::GetTerrain(CellId id) const 
{
    FJ_ASSERT_RET(id < GetCellCount(), Terrain::Invalid); 
    return Cells_[id].Terrain; 
}

void World::SetTerrain(CellId id, Terrain terrain) 
{
    FJ_ASSERT(id < GetCellCount()); 
    Cells_[id].Terrain = terrain; 
}

Quaternion World::GetWindCurrent(CellId id) const
{
    FJ_ASSERT(id < GetCellCount()); 
    return Cells_[id].WindCurrent; 
}

void World::SetWindCurrent(CellId id, const Quaternion& rotation) 
{
    FJ_ASSERT(id < GetCellCount()); 
    Cells_[id].WindCurrent = rotation; 
}

Quaternion World::GetOceanCurrent(CellId id) const
{
    FJ_ASSERT(id < GetCellCount()); 
    return Cells_[id].OceanCurrent; 
}

void World::SetOceanCurrent(CellId id, const Quaternion& rotation) 
{
    FJ_ASSERT(id < GetCellCount()); 
    Cells_[id].OceanCurrent = rotation; 
}