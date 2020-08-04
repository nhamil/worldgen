#pragma once 

#include <Fjord/Common.h> 
#include <Fjord/Math/MathUtil.h> 
#include <Fjord/Util/Memory.h> 

struct FluidSim 
{
	FJ_NO_COPY(FluidSim); 
public: 
    FluidSim(int w, int h) 
		: W(w) 
		, H(h) 
		, N((int) std::sqrt(w * h)) 
		, WH(w * h)
	{
		Vx = new float[WH](); 
		Vx0 = new float[WH](); 
		Vxs = new float[WH](); 
		Vy = new float[WH](); 
		Vy0 = new float[WH](); 
		Vys = new float[WH](); 
		P = new float[WH](); 
		P0 = new float[WH](); 
		Ps = new float[WH](); 
		Wall = new bool[WH](); 
		Tmp = new float[WH](); 
		Dist = new float[H](); 

		for (int i = 0; i < H; i++) 
		{
			Dist[i] = std::abs(std::sin(FJ_PI * (float) i / (H - 1))); 
			Dist[i] = Max(Dist[i], 0.001f); 
		}
	} 

    ~FluidSim() 
	{
		delete[] Vx; 
		delete[] Vx0; 
		delete[] Vxs; 
		delete[] Vy; 
		delete[] Vy0; 
		delete[] Vys; 
		delete[] P; 
		delete[] P0; 
		delete[] Ps; 
		delete[] Wall; 
		delete[] Tmp; 
		delete[] Dist; 
	} 

    void Update(float dt) 
    {
		DensStep(Diff, dt); 
        VelStep(Visc, dt); 
    }

    int ix(int x, int y) const 
    {
        if (y < 0) { y = -1 - y; x += W/2; } else if (y >= H) { y = 2*H-1 - y; x -= W/2; } 
		// while (y < 0) y += H; 
        // while (y >= H) y -= H; 
		while (x < 0) x += W; 
        while (x >= W) x -= W; 
		FJ_ASSERT_RET_MSG(!(x < 0 || x >= W || y < 0 || y >= H), 0, "Invalid pos: %d, %d", x, y); 
		return x + W * y; 
    }

    const int W, H, N, WH; 

	bool AdvectEnabled = true; 
	float Diff = 1.0f; 
    float Visc = 1.0f; 

    float* Vx, * Vx0, * Vxs; 
    float* Vy, * Vy0, * Vys; 
	float* P, * P0, * Ps; 
    bool* Wall; 

    float* Tmp; 
    float* Dist; 

private: 
    int m(int y, bool vector) {
		if (!vector) return 1; 
		if (y < 0) return -1; else if (y >= H) return -1;  
		return 1; 
	}
	
	int m(int y) {
		if (y < 0) return -1; else if (y >= H) return -1;  
		return 1; 
	}

    void Swap(float** a, float** b) 
    {
        float* tmp = *a; 
        *a = *b; 
        *b = tmp; 
    }
    
    void Copy(const float* from, float* to) 
    {
        // Fjord::MemCpy(to, from, WH, sizeof(float)); 
		for (int i = 0; i < WH; i++) to[i] = from[i]; 
    }

	// TODO multiply by d? 
	void AddSource(float* x, float* s, bool minZero, float dt) {
		int size = WH; 
		if (minZero) {
			for (int i = 0; i < size; i++) x[i] = Fjord::Max(0.0f, x[i] + s[i] * dt * Dist[i/W]); 
		}
		else {
			for (int i = 0; i < size; i++) x[i] += s[i] * dt * Dist[i/W]; 
		}
	}

    void Diffuse(int b, float* x, float* x0, float diff, bool v, float dt) {
//		float a = W * H * diff * dt; 
		float a = diff * dt; 
		
		for (int k = 0; k < 20; k++) {
			for (int j = 0; j < H; j++) {
				float dd = 1.0f / Dist[j]; 
				for (int i = 0; i < W; i++) {
					x[ix(i,j)] = (
							x0[ix(i,j)] + a * (
									dd*x[ix(i-1,j)] + dd*x[ix(i+1,j)] + m(j-1,v)*x[ix(i,j-1)] + m(j+1,v)*x[ix(i,j+1)]
							)
					) / (1 + (2*dd+2)*a); 
				}
			}
			SetBounds(b, x, v); 
		}
	}

    void Advect(int b, float* d, float* d0, float* u, float* v, bool w, float dt) {
		// float dt0 = dt * N
		float dtx = dt * N; 
		float dty = dt * N; 
		for (int j = 0; j < H; j++) {
			float dd = 1.0f / Dist[j]; 
			for (int i = 0; i < W; i++) {
//				System.out.println(dtx + " " + d[j] + " " + dd + " " + (dtx * dd));
				float x = i - dtx * dd * u[ix(i,j)]; 
				float y = j - dty * v[ix(i,j)]; 
//				if (x < 0.5) x = 0.5f; if (x > W-1.5) x = W-1.5f; 
				int i0 = (int) Fjord::Floor(x), i1 = i0 + 1; 
//				if (y < 0.5) y = 0.5f; if (y > H-1.5) y = H-1.5f; 
				int j0 = (int) Fjord::Floor(y), j1 = j0 + 1; 
				float s1 = x - i0, s0 = 1 - s1, t1 = y - j0, t0 = 1 - t1; 
				d[ix(i,j)] = s0*(t0*m(j0,w)*d0[ix(i0,j0)] + t1*m(j1,w)*d0[ix(i0,j1)]) + 
						     s1*(t0*m(j0,w)*d0[ix(i1,j0)] + t1*m(j1,w)*d0[ix(i1,j1)]); 
			}
		}
		SetBounds(b, d, w); 
	}

    void Project(float* u, float* v, float* p, float* div) {
		// float h = 1.0f / N; 
		float hx = 1.0f / N; 
		float hy = 1.0f / N; 
		
		for (int j = 0; j < H; j++) {
			for (int i = 0; i < W; i++) {
				div[ix(i,j)] = -0.5f*hx*(u[ix(i+1,j)]-u[ix(i-1,j)]) - 0.5f*hy*(m(j+1)*v[ix(i,j+1)]-m(j-1)*v[ix(i,j-1)]); 
				p[ix(i,j)] = 0; 
			}
		}
		SetBounds(0, div, true); 
		SetBounds(0, p, true); 
		
		for (int k = 0; k < 20; k++) {
			for (int j = 0; j < H; j++) {
				for (int i = 0; i < W; i++) {
					p[ix(i,j)] = (
							div[ix(i,j)] + p[ix(i-1,j)] + p[ix(i+1,j)] + m(j-0)*p[ix(i,j-1)] + m(j+0)*p[ix(i,j+1)] 
					) / 4;
				}
			}
			SetBounds(0, p, true); 
		}
		
		for (int j = 0; j < H; j++) {
			for (int i = 0; i < W; i++) {
				u[ix(i,j)] -= 0.5f * (p[ix(i+1,j)] - p[ix(i-1,j)]) / hx; 
				v[ix(i,j)] -= 0.5f * (m(j+0)*p[ix(i,j+1)] - m(j-0)*p[ix(i,j-1)]) / hy; 
			}
		}
		SetBounds(1, u, false); 
		SetBounds(2, v, true); 
	}

    void ZeroW(float* a) {
		for (int i = 0; i < W*H; i++) if (Wall[i]) a[i] = 0; 
	}
	
	void SetBounds(int b, float* a, bool revY) {
		ZeroW(a); 
//		Distort(a); 
		Copy(a, Tmp); 

		for (int y = 0; y < H; y++) {
			for (int x = 0; x < W; x++) {
				if (Wall[ix(x,y)]) {
					float v = 0; 
					int count = 0; 
					if (!Wall[ix(x-1,y)]) { count++; v += Tmp[ix(x-1,y)] * (b == 1 ? -1 : 1); }
					if (!Wall[ix(x+1,y)]) { count++; v += Tmp[ix(x+1,y)] * (b == 1 ? -1 : 1); }
					if (!Wall[ix(x,y-1)]) { count++; v += m(y-1,revY)*Tmp[ix(x,y-1)] * (b == 2 ? -1 : 1); }
					if (!Wall[ix(x,y+1)]) { count++; v += m(y+1,revY)*Tmp[ix(x,y+1)] * (b == 2 ? -1 : 1); }
					if (count != 0) {
						a[ix(x,y)] = v / count; 
					}
				}
			}
		}
	}

	void DensStep(float diff, float dt) 
	{
		Copy(Ps, P0); 
		AddSource(P, P0, true, dt); 
		Swap(&P, &P0); Diffuse(0, P, P0, diff, false, dt); 
		if (AdvectEnabled) 
		{
			Swap(&P, &P0); Advect(0, P, P0, Vx, Vy, false, dt); 
		}
	}

    void VelStep(float visc, float dt) 
    {
        Copy(Vxs, Vx0); 
        Copy(Vys, Vy0); 

        AddSource(Vx, Vx0, false, dt); AddSource(Vy, Vy0, false, dt); 
        Swap(&Vx, &Vx0); Diffuse(1, Vx, Vx0, visc, true, dt); 
        Swap(&Vy, &Vy0); Diffuse(2, Vy, Vy0, visc, true, dt); 
        Project(Vx, Vy, Vx0, Vy0); 

        if (AdvectEnabled) 
        {
			Swap(&Vx, &Vx0); Swap(&Vy, &Vy0); 
			Advect(1, Vx, Vx0, Vx0, Vy0, false, dt); 
			Advect(2, Vy, Vy0, Vx0, Vy0, true, dt); 
			Project(Vx, Vy, Vx0, Vy0); 
        }
    }
};