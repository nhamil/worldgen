#include <Fjord/Common.h> 
#include <Fjord/Util/KDTree.h> 
#include <Fjord/Util/Random.h> 
#include <Fjord/Util/Time.h> 

using namespace Fjord; 
using namespace std; 

void Print(const Vector3& v) 
{
    FJ_DEBUG("%f %f %f", 
        v.X, 
        v.Y,
        v.Z
    );
}

int main() 
{
    Random r; 
    KDTree3<int> tree; 
    double time; 
    
    FJ_INFO("Creating points"); 
    time = GetTimeSeconds(); 
    Vector<Vector3> points; 
    for (unsigned i = 0; i < 1000000; i++) 
    {
        points.push_back(Normalized({
            r.NextFloat() * 2 - 1, 
            r.NextFloat() * 2 - 1, 
            r.NextFloat() * 2 - 1 
        }));
        tree.Insert(points[i], i); 
    }
    FJ_INFO("Took %f seconds", GetTimeSeconds() - time); 

    FJ_INFO("Creating search points"); 
    time = GetTimeSeconds(); 
    Vector<Vector3> searchPoints; 
    for (unsigned i = 0; i < 1000000; i++) 
    {
        searchPoints.push_back(Normalized({
            r.NextFloat() * 2 - 1, 
            r.NextFloat() * 2 - 1, 
            r.NextFloat() * 2 - 1 
        }));
    }
    FJ_INFO("Took %f seconds", GetTimeSeconds() - time); 

    FJ_INFO("Searching with tree"); 
    time = GetTimeSeconds(); 
    for (unsigned i = 0; i < searchPoints.size(); i++) 
    {
        int out = tree.Search(searchPoints[i]); 
    }
    FJ_INFO("Took %f seconds", GetTimeSeconds() - time); 

    FJ_INFO("Searching with brute force"); 
    time = GetTimeSeconds(); 
    for (unsigned i = 0; i < searchPoints.size(); i++) 
    {
        float dist = INFINITY; 
        int best = 0; 
        for (unsigned j = 0; j < points.size() / 100; j++) 
        {
            float d = Length(searchPoints[i] - points[j]); 
            if (d < dist) 
            {
                dist = d; 
                best = j; 
            }
        }
    }
    FJ_INFO("Took %f seconds", 100 * (GetTimeSeconds() - time)); 

    FJ_INFO("Done!"); 
}