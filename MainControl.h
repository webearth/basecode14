/*
MainControl.h
Written by Matthew Fisher

MainControl includes everything that changes often between applications, such as what meshes to load,
and also determines what is rendered each frame.
*/

class MainControl : public FrameController
{
public:
    void ResetCamera();
    void FreeMemory();
    void ReInit(WindowObjects &O);    //called after we lose focus, telling us to restore
                                      //our textures and other data that may have been list
    void Init(WindowObjects &O);      //called only one at the beginning of our application
    void Render(WindowObjects &O);    //called each frame
    void ReleaseObjects(WindowObjects &O);  //called to release local graphics card data
    
private:
    void InitMenu(WindowObjects &O);
    void ProcessMenu(WindowObjects &O);
    void ProcessKeyboardInput(WindowObjects &O);

    void StatusText(WindowObjects &O);

    void UpdateCamera(WindowObjects &O);
    
    bool _UseRovingCamera;
    Matrix4 _RotationTransform;
    MatrixController _MC;
    Camera _C;
    Indicator _Ind;
    Mesh _Sphere;

    MenuInterface _Menu;
};
