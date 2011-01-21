/*
MainControl.cpp
Written by Matthew Fisher

MainControl includes everything that changes often between applications, such as what meshes to load,
and also determines what is rendered each frame.
*/

#include "Main.h"

enum MenuEntryType
{
    MenuEntryFile,
    MenuEntryFileQuit,
};

void MainControl::InitMenu(WindowObjects &O)
{
    _Menu.Init(O.GetWindowManager());
    
    Menu &FileMenu = _Menu.MenuBar().AddSubMenu(O.GetWindowManager(), MenuEntryFile, "File");
    FileMenu.AddString(O.GetWindowManager(), MenuEntryFileQuit, "Quit\tCtrl+Q");
}

void MainControl::ProcessMenu(WindowObjects &O)
{
    _Menu.DispatchKeyboardShortcuts(O.GetInputManager());

    while(O.GetInputManager().EventPresent())
    {
        InputEvent CurEvent = O.GetInputManager().PopEvent();
        if(CurEvent.Type == InputEventMenu)
        {
            switch(CurEvent.Id)
            {
            case MenuEntryFileQuit:
                PostQuitMessage(0);
                break;
            }
        }
    }
}

void MainControl::FreeMemory()
{

}

void MainControl::ResetCamera()
{
    _C.Reset(Vec3f(3.0f, 0.0f, 7.0f), Vec3f::eZ, Vec3f::Origin);
    _RotationTransform = Matrix4::Identity();
}

void MainControl::ReleaseObjects(WindowObjects &O)
{
    
}

void MainControl::ReInit(WindowObjects &O)
{
    ResetCamera();
    
    _MC.Perspective = Matrix4::PerspectiveFov(60.0f * Math::PIf / 180.0f,       //the field of view = 60 degrees
                                    float(O.GetWindowManager().GetWidth()) /
                                    float(O.GetWindowManager().GetHeight()),   //the aspect ratio
                                    0.01f,                                     //near Z-plane
                                    200.0f);                                   //far Z-plane

    _Ind.Init(O.GetGraphicsDevice(), 0, 1, 4);
}

void MainControl::Init(WindowObjects &O)
{
    AllocConsole();

    InitMenu(O);

    _UseRovingCamera = false;

    _Sphere.SetGD(O.GetGraphicsDevice());
    _Sphere.CreateSphere(2.0f, 5);
    _Sphere.GCNormals();
    _Sphere.ApplyMatrix(_Sphere.UnitSphereTransform());

    ReInit(O);
}

void MainControl::UpdateCamera(WindowObjects &O)
{
    if(O.GetInputManager().KeyCheckOnce(KEY_F))
    {
        O.GetGraphicsDevice().ToggleWireframe();    //toggle wireframe if the user presses "F"
    }

    if(_UseRovingCamera)
    {
        _C.WindowKeyboard(O, 10.0f, 1.0f);               //move based upon the keyboard
        if(O.GetGraphicsDevice().GetFullScreen())
        {
            _C.Mouse(0.001f);                            //move based upon the mouse, if we're in full-screen mode
        }
        _MC.World = Matrix4::Identity();
    }
    else
    {
        _C.PlanarMouse(O.GetInputManager(), 0.0f, 1.0f);
        if(O.GetInputManager().Mouse().Buttons[MouseButtonLeft] && O.GetInputManager().MouseDiff().Pos.Length() > 0.0f)
        {
            Matrix4 MatX = Matrix4::RotationX(-O.GetInputManager().MouseDiff().Pos.x * 0.005f);
            Matrix4 MatY = Matrix4::RotationY(O.GetInputManager().MouseDiff().Pos.y * 0.005f);
            _RotationTransform = _RotationTransform * MatX * MatY;
        }
    }
    _C.Update();
    _MC.World = Matrix4::Identity();
    _MC.View = _RotationTransform * _C.ViewMatrix();
    O.GetGraphicsDevice().LoadMatrix(_MC);
}

void MainControl::ProcessKeyboardInput(WindowObjects &O)
{

}

void MainControl::StatusText(WindowObjects &O)
{
    UINT CurYOffset = 2;
    O.GetGraphicsDevice().DrawDouble("fps: ", O.GetFrameTimer().GetFramesPerSec(), 2, CurYOffset);
    CurYOffset += 20;
}

void MainControl::Render(WindowObjects &O)
{
    ProcessKeyboardInput(O);
    ProcessMenu(O);

    UpdateCamera(O);

    //_Ind.RenderCylinder(O.GetGraphicsDevice(), _MC, 0.1f, Vec3f::Origin, Vec3f::eX, RGBColor::Red);
    //_Ind.RenderCylinder(O.GetGraphicsDevice(), _MC, 0.1f, Vec3f::Origin, Vec3f::eY, RGBColor::Green);
    //_Ind.RenderCylinder(O.GetGraphicsDevice(), _MC, 0.1f, Vec3f::Origin, Vec3f::eZ, RGBColor::Blue);

    _MC.World = Matrix4::Identity();
    O.GetGraphicsDevice().LoadMatrix(_MC);
    _Sphere.Render();

    StatusText(O);
}
