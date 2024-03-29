/*
D3D9PersistentMesh.h
Written by Matthew Fisher

D3D9PersistentMesh is a D3D9Mesh that is backed up with a RAM copy, so it can be
preserved when the device is lost and needs to be reset.
*/
#pragma once

#ifdef USE_D3D9

class D3D9PersistentMesh : public D3D9Object
{
public:
    D3D9PersistentMesh();
    ~D3D9PersistentMesh();
    void FreeMemory();
    void ReleaseMemory();
    void Reset(LPDIRECT3DDEVICE9 Device);

    void Init(const BaseMesh &M, LPDIRECT3DDEVICE9 Device);
    void Render();

    __forceinline Mesh& GetMesh()
    {
        return _Mesh;
    }

    __forceinline D3D9Mesh& GetD3DMesh()
    {
        return _D3DMesh;
    }

private:
    Mesh _Mesh;
    D3D9Mesh _D3DMesh;
    LPDIRECT3DDEVICE9 _Device;
};

#endif