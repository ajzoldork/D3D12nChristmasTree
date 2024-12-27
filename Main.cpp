//*********************************************************
// Create the instance and run the Win32 application
//*********************************************************

#include "stdafx.h"
#include "D3D12nChristmasTree.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    D3D12nChristmasTree sample(1600, 900, L"D3D12 On Christmas Tree Simulation");  // 1280, 720 original window size
    return Win32Application::Run(&sample, hInstance, nCmdShow);
}
