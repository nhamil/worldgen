#include "WorldGen/WorldGenerator.h" 

#include <Fjord/Util/Thread.h> 

#include "WorldGen/FluidSim.h" 

#define RES (8)

const int ClimateSimulationRule::SimWidth = 1 << RES; 
const int ClimateSimulationRule::SimHeight = 1 << (RES - 1); 
const float ClimateSimulationRule::InvSimWidth = 1.0 / ClimateSimulationRule::SimWidth; 
const float ClimateSimulationRule::InvSimHeight = 1.0 / ClimateSimulationRule::SimHeight; 

ClimateSimulationRule::ClimateSimulationRule(int air, int ocean) 
{
    AirIterations_ = air; 
    OceanIterations_ = ocean; 
}

void ClimateSimulationRule::Apply(World& world) 
{
    FJ_DEBUG("Simulating air currents..."); 
    FluidSim air(SimWidth, SimHeight); 
    FluidSim ocean(SimWidth, SimHeight); 

    AddPressureBands(air); 
    SetBounds(air, ocean, world); 

    air.AdvectEnabled = false; 
    air.Diff = 5.0f * (6-RES)*(6-RES); 
    air.Visc = 0.0f * (6-RES)*(6-RES); 

    ocean.AdvectEnabled = true; 
    ocean.Diff = 0.0f * (6-RES)*(6-RES); 
    ocean.Visc = 0.0f * (6-RES)*(6-RES); 

    for (int i = 0; i < AirIterations_; i++) 
    {
        InitWind(air); 
        air.Update(1.0 / 60.0); 
    }

    UpdateCellsAir(air, world); 

    FJ_DEBUG("Simulating ocean currents..."); 
    InitOcean(ocean, air); 
    for (int i = 0; i < OceanIterations_; i++) 
    {
        ocean.Update(1.0 / 60.0); 
    }

    UpdateCellsOcean(ocean, world); 
}

void ClimateSimulationRule::SetBounds(FluidSim& air, FluidSim& ocean, World& world) 
{
    for (int y = 0; y < SimHeight; y++) 
    {
        float lat = 90.0f - y * 180.0f * InvSimHeight; 
        for (int x = 0; x < SimWidth; x++) 
        {
            float lon = -180.0f + x * 360.0f * InvSimWidth; 
            Vector3 pos = GetPositionFromGeoCoords({lat, lon}); 

            CellId cell = world.GetCellIdPyPosition(pos); 
            Terrain terrain = world.GetTerrain(cell); 

            ocean.Wall[ocean.ix(x, y)] = !IsTerrainWater(terrain); 

            if (terrain == Terrain::Mountain) 
            {
                air.Wall[air.ix(x, y)] = true; 
                air.P[air.ix(x, y)] = 0.5f; 
            }
            else if (!IsTerrainWater(terrain)) 
            {
                if (y < 0.5f * SimHeight) 
                {
                    air.P[air.ix(x, y)] = 1; 
                }
                else 
                {
                    air.P[air.ix(x, y)] = 0; 
                }
            }
        }
    }
}

float ClimateSimulationRule::GradX(FluidSim& sim, float* a, int x, int y) 
{
    return a[sim.ix(x-1, y)] - a[sim.ix(x+1, y)]; 
}

float ClimateSimulationRule::GradY(FluidSim& sim, float* a, int x, int y) 
{
    return a[sim.ix(x, y-1)] - a[sim.ix(x, y+1)]; 
}

void ClimateSimulationRule::AddPressureBands(FluidSim& air) 
{
    static Vector<float> pressureLats({
            0,  // polar front, low pressure
            30, // subtropical high, high pressure
            60, // low pressure 
            90, 
    }); 
    static Vector<float> pressureAmts({
            0, 
            1, 
            0, 
            1
    }); 
    const int N = pressureLats.size(); 
    const float HalfHeight = (SimHeight-1) * 0.5f; 
    
    for (int y = 0; y < SimHeight; y++) {
        float lat = 90 * std::abs(HalfHeight - y) / HalfHeight; 
        float pressure = 0; 
        
        for (int i = 1; i < N; i++) { // start at 1, can't be lower than itcz anyways
            if (lat <= pressureLats[i]+0.1f) {
                float lerpAmt = (lat - pressureLats[i-1]) / (pressureLats[i] - pressureLats[i-1]); 
                pressure = Lerp(pressureAmts[i-1], pressureAmts[i], lerpAmt); 
                break; 
            }
        }
        
        for (int x = 0; x < SimWidth; x++) {
            air.P[air.ix(x, y)] = pressure; 
        }
    }
}

void ClimateSimulationRule::InitWind(FluidSim& air) 
{
    for (int y = 0; y < SimHeight; y++) 
    {
        for (int x = 0; x < SimWidth; x++) 
        {
            float mul = 1;// - 1.15f * Mathf.pow(Mathf.abs(0.5f - air.p[air.ix(x, y)]), 0.2f); 
            float gx = GradX(air, air.P, x, y); 
            float gy = GradY(air, air.P, x, y); 
            if (y < SimHeight/2) 
            {
                mul *= -1; 
            }
            air.Vx[air.ix(x, y)] = mul*gy; 
            air.Vy[air.ix(x, y)] = mul*-gx; 
        }
    }
}

void ClimateSimulationRule::UpdateCellsAir(FluidSim& air, World& world) 
{
    // for (CellId cell = 0; cell < world.GetCellCount(); cell++) 
    ParallelFor(CellId(0), CellId(world.GetCellCount()), [&](CellId cell)
    {
        Vector3 pos = world.GetPosition(cell); 
        GeoCoords gc = GetGeoCoordsFromPosition(pos); 

        int x = (int) Round((gc.Longitude+180)/360 * SimWidth); 
		int y = (int) Round((-gc.Latitude+90)/180 * SimHeight); 

        float vx = air.Vx[air.ix(x, y)]; 
        float vy = air.Vy[air.ix(x, y)]; 

        float northAngle = std::atan2(vx, vy);
        float strength = Length(Vector2{vx, vy}) * 4; 

        Vector3 up = Quaternion::AxisAngle(pos, northAngle) * Vector3::Up; 
        Vector3 axis = Normalized(Cross(up, pos)); 

        world.SetWindCurrent(cell, Quaternion::AxisAngle(axis, strength)); 
    }); 
}

void ClimateSimulationRule::UpdateCellsOcean(FluidSim& ocean, World& world) 
{
    // for (CellId cell = 0; cell < world.GetCellCount(); cell++) 
    ParallelFor(CellId(0), CellId(world.GetCellCount()), [&](CellId cell)
    {
        Vector3 pos = world.GetPosition(cell); 
        GeoCoords gc = GetGeoCoordsFromPosition(pos); 

        int x = (int) Round((gc.Longitude+180)/360 * SimWidth); 
		int y = (int) Round((-gc.Latitude+90)/180 * SimHeight); 

        float vx = ocean.Vx[ocean.ix(x, y)]; 
        float vy = ocean.Vy[ocean.ix(x, y)]; 

        float northAngle = std::atan2(vx, vy);
        float strength = Length(Vector2{vx, vy}) * 2; 
        // strength = std::pow(strength, 2.0f) * 4; 

        Vector3 up = Quaternion::AxisAngle(pos, northAngle) * Vector3::Up; 
        Vector3 axis = Normalized(Cross(up, pos)); 

        world.SetOceanCurrent(cell, Quaternion::AxisAngle(axis, strength)); 
    }); 
}

void ClimateSimulationRule::InitOcean(FluidSim& ocean, FluidSim& air) 
{
    float mul = 1; 
    for (int i = 0; i < ocean.WH; i++) 
    {
        ocean.Vxs[i] = air.Vx[i] * mul; 
        ocean.Vys[i] = air.Vy[i] * mul; 
    }
}