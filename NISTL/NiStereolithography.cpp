// NISTL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <XnOS.h>
#include "XnCppWrapper.h"

using namespace xn;

int _tmain(int argc, _TCHAR* argv[])
{
	XnStatus nRetVal = XN_STATUS_OK;
	Context context;

	nRetVal = context.Init();
	DepthGenerator depth;
	nRetVal = depth.Create(context);

	XnMapOutputMode mapMode;

	mapMode.nXRes = XN_VGA_X_RES;
	mapMode.nYRes = XN_VGA_Y_RES; 
	mapMode.nFPS = 30; 
	nRetVal = depth.SetMapOutputMode(mapMode);

	nRetVal = context.StartGeneratingAll();


	XnUInt32 nMiddleIndex = XN_VGA_X_RES * XN_VGA_Y_RES/2 + XN_VGA_X_RES/2;

	while (TRUE) { // Update to next frame 
		nRetVal = context.WaitOneUpdateAll(depth);
		const XnDepthPixel* pDepthMap = depth.GetDepthMap(); 
		printf("Middle pixel is %u millimeters away\n", pDepthMap[nMiddleIndex]); 
	} 
	// Clean up context.Shutdown();
}

