/*
App.cpp
Written by Matthew Fisher

The App class glues the operating system, the graphics device, the application interface,
and the window manager together.  It is essentially the first and the last code that is executed.
See App.h for a description of the class and member functions.
*/

//All source files include Main.h
#include "Main.h"

const int GraphicsDeviceType = GD_D3D9;
//GraphicsDeviceType tells App what type of graphics device/API to use.
//possible choices are:
//GD_D3D9        -> DirectX 9.0 API
//GD_D3D10       -> DirectX 10.0 API
//GD_OpenGL      -> OpenGL API
//GD_Software    -> Software rendering
//DirectX offers more functionality, but many people find it harder to understand and it's Windows only
//OpenGL is more widely supported but doesn't have nice things like mesh simplification built in
//Software is written by me, by hand, and is slow and has very limited functionality
//you'll need #define USE_DIRECTX or #define USE_OPENGL (in Main.h) to use DirectX or OpenGL

const UINT WindowWidth = 720;
const UINT WindowHeight = 640;

const bool VSync = true;            //true to wait for the monitor to finish the entire display (vertical retracing) before
                                    //the front buffer is flipped.  Setting this to false results in higher frame rate, but may
                                    //induce flickering.

const String FontName = "Verdana";

void App::Init(HINSTANCE hInstance, int nCmdShow)
{
    GraphicsDevice *GD = NULL;
    WindowManager *WM = NULL;
    _WindowName = "BaseCode";

    //
    // Create a new GraphicsDevice object based upon the type of graphics device requested in GraphicsDeviceType
    //
    if(GraphicsDeviceType == GD_Software)
    {
        GD = new SoftwareGraphicsDevice;
        WM = new WindowsWindowManager;
    }
#ifdef USE_D3D9
    else if(GraphicsDeviceType == GD_D3D9)
    {
        GD = new D3D9GraphicsDevice;
        WM = new WindowsWindowManager;
    }
#endif
#ifdef USE_D3D10
    else if(GraphicsDeviceType == GD_D3D10)
    {
        GD = new D3D10GraphicsDevice;
        WM = new WindowsWindowManager;
    }
#endif
#ifdef USE_OPENGL
    else if(GraphicsDeviceType == GD_OpenGL)
    {
        GD = new OpenGLGraphicsDevice;
        WM = new WindowsWindowManager;
    }
#endif
    else
    {
        Assert(NULL, "Invalid choice of graphics device type.");
    }
    Assert(GD != NULL, "Failed to allocate graphics device");
    Assert(WM != NULL, "Failed to allocate window manager");
    _Objects.SetGraphicsDevice(GD);
    _Objects.SetWindowManager(WM);

    g_WndProcContext = &_Objects;
    _Objects.GetWindowManager().CastWindows().InitAll(hInstance, nCmdShow, false, WindowWidth, WindowHeight, _WindowName);

    GraphicsDeviceParameters Parameters;
    Parameters.WaitForVSync = VSync;
    Parameters.MainFontName = FontName;
    GD->Init3D(_Objects, Parameters);
    GD->DisableWireframe();

    _Objects.GetFrameTimer().Start(60.0f);
    _Objects.GetFrameTimer().Pause();

    _App.Init(_Objects);
}

void App::RenderFrame()
{
    _Objects.GetGraphicsDevice().StartRender(_Objects);   //have the Graphics device prepare a new frame

    if(_Objects.GetGraphicsDevice().GetSystemActive())    //if we are good to render (we wouldn't be if say we were in the middle of a window change)
    {
        _Objects.GetFrameTimer().Frame();        //update the FPS counter
        _App.Render(_Objects);                   //have the application render its _Objects through the graphics device
    }
    else
    {
        _Objects.GetFrameTimer().Pause();        //if we aren't good to render, don't update the FPS counter
    }
    //RedrawWindow(NULL, NULL, NULL, RDW_UPDATENOW | RDW_INTERNALPAINT | RDW_INVALIDATE);
    _Objects.GetGraphicsDevice().FinishRender(_Objects, _App);    //put the rendered scene onto the screen
}

void App::ResetGraphicsObjects()
{
    _Objects.GetFrameTimer().Pause();
    _App.ReInit(_Objects);    //reinialize all the scene _Objects
}

void App::HandleReSize()
{
    _Objects.GetFrameTimer().Pause();
    _Objects.GetGraphicsDevice().ReSize(_Objects, _App);
}

void App::MessageLoop(HINSTANCE hInstance, int nCmdShow)
{
    //the main Windows message loop

    MSG msg;
    BOOL bGotMsg;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );    //get a new message

    while( WM_QUIT != msg.message  )    //while we don't have the quit message
    {
        bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );    //get a new message
        if( bGotMsg )                    //if we have a message
        {
            TranslateMessage( &msg );    //send it out
            DispatchMessage( &msg );
        }
        else
        {
            //
            // If we have no messages we decide it's a good time to render 
            //

            _Objects.GetInputManager().Frame(_Objects.GetWindowManager());

            RenderFrame();                                    //render a new frame
            /*if(_Objects.GetInputManager().KeyCheckOnce(KEY_ALT_ENTER))
            {
                HandleAltEnter();    //toggle full screen display if the user Alt+Enters
            }*/
            if(_Objects.AcknowledgeReSize())
            {
                HandleReSize();        //if we're in a window and the user resizes it
            }
            if(_Objects.GetGraphicsDevice().NeedToRestoreDevices()) //if we've lost devices,
            {
                _Objects.GetFrameTimer().Pause();                //pause the timer for a short bit,
                ResetGraphicsObjects();                          //reset the graphics _Objects,
                _Objects.GetGraphicsDevice().DevicesRestored();  //tell the GD everything's okay
            }
        }
    }
}

void App::FreeMemory()
{
    _App.End(_Objects);            //free application data

    _Objects.GetWindowManager().FreeMemory();    //free the window
    _Objects.GetGraphicsDevice().FreeMemory();   //free the graphics API data
}
