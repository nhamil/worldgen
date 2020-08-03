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
    Random r(0); 
    KDTree3<int> tree; 
    double time; 
    
    FJ_INFO("Creating points"); 
    time = GetTimeSeconds(); 
    Vector<Vector3> points; 
    for (unsigned i = 0; i < 100000; i++) 
    {
        // FJ_INFO("tree size  : %lld", tree.Size()); 
        // FJ_INFO("point size : %lld", points.size()); 
        points.push_back(Normalized({
            r.NextFloat() * 2 - 1, 
            r.NextFloat() * 2 - 1, 
            r.NextFloat() * 2 - 1 
        }));
        // FJ_INFO("done"); 
        tree.Insert(points[i], i); 
    }
    FJ_INFO("Took %f seconds", GetTimeSeconds() - time); 

    FJ_INFO("Creating search points"); 
    time = GetTimeSeconds(); 
    Vector<Vector3> searchPoints; 
    for (unsigned i = 0; i < 1000; i++) 
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
        // FJ_INFO("%u", i); 
        int out = tree.Search(searchPoints[i]); 
    }
    FJ_INFO("Took %f seconds", GetTimeSeconds() - time); 

    FJ_INFO("Searching with brute force"); 
    time = GetTimeSeconds(); 
    for (unsigned i = 0; i < searchPoints.size(); i++) 
    {
        float dist = INFINITY; 
        int best = 0; 
        for (unsigned j = 0; j < points.size(); j++) 
        {
            float d = Length(searchPoints[i] - points[j]); 
            if (d < dist) 
            {
                dist = d; 
                best = j; 
            }
        }
    }
    FJ_INFO("Took %f seconds", (GetTimeSeconds() - time)); 

    for (unsigned i = 0; i < searchPoints.size(); i++) 
    {
        // FJ_INFO("%u", i); 
        int out = tree.Search(searchPoints[i]); 
        float dist = INFINITY; 
        int best = 0; 
        for (unsigned j = 0; j < points.size(); j++) 
        {
            float d = Length(searchPoints[i] - points[j]); 
            if (d < dist) 
            {
                dist = d; 
                best = j; 
            }
        }
        if (out != best) FJ_ASSERT_MSG(false, "Different nearest neighbors!"); 
    }

    FJ_INFO("Done!"); 
}