//*********************************************************
//*
//* Each particle in the simulation is controlled by a GPU
//* thread running the CSMain code. The velocity is added
//* to the position until the particle "lands" on the tree,
//* then the particle rotates around its ring.
//*
//*********************************************************

#define blocksize 32


cbuffer cbCS : register(b0)
{
    uint4 g_param;      // param[0] = MAX_PARTICLES;
                        // param[1] = dimx;
    float4 g_paramf;    // paramf[0] = 0.1f;
                        // paramf[1] = 1; 
};

cbuffer cbCS2 : register(b1)
{
    float g_time;
}

struct PosVelo
{
    float4 pos;
    float4 velo;
};
struct PosVeloF
{
    float4 pos;
    float4 velo;
    bool onTree;
};


StructuredBuffer<PosVelo> oldPosVelo   : register(t0); // SRV
RWStructuredBuffer<PosVelo> newPosVelo : register(u0); // UAV
RWStructuredBuffer<PosVeloF> PosVeloOn : register(u1); // UAV2
 
[numthreads(blocksize, 1, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    // Each thread of the CS updates one of the particles.
    float4 pos = oldPosVelo[DTid.x].pos;
    float4 vel = oldPosVelo[DTid.x].velo;
     
    if (PosVeloOn[DTid.x].onTree == false)
    {
        //* When the particle gets close enough to its landing spot, go ahead and set the final value
        float d = distance(pos.xyz, PosVeloOn[DTid.x].pos.xyz);
        if (d < 20.0f)
        {
            PosVeloOn[DTid.x].onTree = true;
            pos = PosVeloOn[DTid.x].pos;
            vel = float4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            pos.xyz += vel.xyz;
        }
            
    }
    else 
    {
        // Particles are rotating on the xz plane. Each ring up the tree rotates faster than the previous
        int radius = 200 - (Gid.x * 5.0); // 5.5
        pos.x = radius * (float) sin(((((float) 360 / blocksize) * DTid.x) * 3.14 / 180) + g_time * ((Gid.x + .4) * .2));
        pos.y = pos.y;
        pos.z = radius * (float) cos(((((float) 360 / blocksize) * DTid.x) * 3.14 / 180) + g_time * ((Gid.x + .4) * .2));
    }
    
    if (DTid.x < g_param.x)
    {
        newPosVelo[DTid.x].pos = pos;
        newPosVelo[DTid.x].velo = vel;
    }
}