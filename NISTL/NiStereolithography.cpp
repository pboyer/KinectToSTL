// NISTL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <XnOS.h>
#include "XnCppWrapper.h"
#include <vector>
#include "vertex.h"
#include <iostream>
#include <fstream>

using namespace xn;
using namespace std;
using namespace NISTL;

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

	int count = 0;
	const XnDepthPixel* pDepthMap;

	while (count<50) { // Update to next frame 
		nRetVal = context.WaitOneUpdateAll(depth);
		pDepthMap = depth.GetDepthMap(); 
		printf("Middle pixel is %u millimeters away\n", pDepthMap[nMiddleIndex]);
		count++;
	} 

	DepthMetaData g_depthMD;
	depth.GetMetaData(g_depthMD);

	cout<<g_depthMD.FullXRes();
	cout<<g_depthMD.FullYRes();

	
	const double maxDepth = 3000;

	const int xScale = 1;
	const int yScale = 1;

	const int xActualRes = XN_VGA_X_RES;
	const int yActualRes = XN_VGA_Y_RES;

	const int xRes = xActualRes/xScale;
	const int yRes = yActualRes/yScale;

	const int sizeOfMap = xRes * yRes + 2*xRes + 2*(yRes-2) +2 ; //multiply by two for back face

	vertex* vertices = new vertex[sizeOfMap];

	double xRealScale = 1.0;
	double yRealScale = 1.0;

	for (int j = 0; j < yRes; j++) {
		for (int i = 0; i < xRes; i++) {
		
			double x = xScale * xRealScale * i;
			double y = yScale * yRealScale * j;

			double z = -(double) g_depthMD.DepthMap()[xActualRes * j * yScale + i * xScale];

			if (z == 0) 
				z = -maxDepth;

			if (z > 1800) 
				z = -maxDepth;

			vertices[xRes*j + i] = vertex(x,y,z);
		}
	}

	int offset = xRes*yRes;

	vector<int> indices;

	//bottom facets
	for (int i = 0; i< xRes; i++) {
		double x = xRealScale * xScale* i;
		double y = 0;
		double z = -maxDepth - 100;  //placeholder

		vertices[offset + i] = vertex(x,y,z);
	}

	//bottom facet winding
	for (int i = 0; i< xRes-1; i++) {
		indices.push_back(i);				
		indices.push_back(i+1);				
		indices.push_back(xRes*yRes + 1 + i);	

		indices.push_back(i);				
		indices.push_back(xRes*yRes + 1 + i);	
		indices.push_back(xRes*yRes + i);	
	}

	offset = offset + xRes;

	//right side facets 
	for (int i = 1; i< yRes; i++) {
		double x = xRealScale * xScale * (xRes-1);
		double y = yRealScale * yScale * i;
		double z = -maxDepth - 100;  //placeholder

		vertices[offset + i -1] = vertex(x,y,z);
	}

	offset = offset + yRes -1;

	//right facet winding
	for (int i = 0; i< yRes-1; i++) {

		indices.push_back(xRes-1 + i * xRes);			//2	 good		
		indices.push_back(xRes-1 + xRes * (i+1));		//5	 			
		indices.push_back(xRes*yRes + xRes + i);		//12

		indices.push_back(xRes-1 + i * xRes);			//2	 good		
		indices.push_back(xRes*yRes + xRes + i -1);		//11	
		indices.push_back(xRes*yRes + xRes + i);		//12
	}


	//top side facets 
	for (int i = 1; i< xRes; i++) {
		double x = xRealScale * xScale * (xRes-1 - i);
		double y = yRealScale * yScale * (yRes-1);
		double z = -maxDepth - 100;  //placeholder

		vertices[offset + i -1] = vertex(x,y,z);
	}

	//top facet winding
	for (int i = 0; i< xRes-1; i++) {

		indices.push_back(xRes*yRes-1 - i);					//8	 good		
		indices.push_back(xRes*yRes -1 + xRes + yRes + i);	//14	 			
		indices.push_back(xRes*yRes -2 - i);				//7

		indices.push_back(xRes*yRes-1 - i);					//8	 good	
		indices.push_back(xRes*yRes -1 + xRes + yRes + i -1);	//13	
		indices.push_back(xRes*yRes -1 + xRes + yRes + i);		//14
	}

	offset = offset + xRes - 1; //check

	//left side facet 
	for (int i = 1; i< yRes+1; i++) {
		double x = 0;
		double y = yRealScale * yScale * (yRes - i); //check
		double z = -maxDepth - 100;  //placeholder

		vertices[offset + i -1] = vertex(x,y,z); //check
	}

	//left side winding
	for (int i = 0; i < yRes-1; i++) {
		indices.push_back(xRes*yRes - (xRes*(i+2)));	 //3
		indices.push_back(xRes*yRes - (xRes*(i+1)));	//6
		indices.push_back(xRes*yRes + 2*xRes + yRes -3 +i);  //15

		indices.push_back(xRes*yRes + 2*xRes + yRes -3 + i + 1);  //16
		indices.push_back(xRes*yRes - (xRes*(i+2)));
		indices.push_back(xRes*yRes + 2*xRes + yRes -3 +i); //15

		//indices.push_back(xRes*yRes - xRes * (i+1));			//6	
		//indices.push_back(xRes*yRes + 2*xRes + yRes - 3 + i);	//15
		//indices.push_back(xRes*yRes - xRes * (i+2));	 		//3	

		//indices.push_back(xRes*yRes - xRes * (i+2));	 			//3	
		//indices.push_back(xRes*yRes + 2*xRes + yRes - 3 + i);		//15
		//indices.push_back(xRes*yRes + 2*xRes + yRes - 3 + i +1);	//16

	}

	
		indices.push_back(xRes*yRes);			//6	
		indices.push_back(0);			//15
		indices.push_back(xRes*yRes + 2*xRes + 2*yRes -4);	

	//bottom face
		indices.push_back(xRes*yRes);			//6	
		indices.push_back(xRes*yRes + xRes-1);			//15
		indices.push_back(xRes*yRes + xRes + yRes -2);	 		//3	

		indices.push_back(xRes*yRes);			//6	
		indices.push_back(xRes*yRes + xRes + yRes -2);			//15
		indices.push_back(xRes*yRes + 2*xRes + yRes -3);		//3	


	printf("Created %u vertices\n",sizeOfMap);

	//front face - this shows the actual depth map
	for (int i = 0; i < xRes-1; i++) {
		for (int j = 0; j < yRes-1; j++) {
			indices.push_back(j * xRes + i);
			indices.push_back((j+1) * xRes + i);
			indices.push_back(j * xRes + i + 1);
			
			indices.push_back(j * xRes + i + 1);
			indices.push_back((j+1) * xRes + i);
			indices.push_back((j+1) * xRes + i + 1);
		}
	}

	int numFacets = indices.size()/3;

	printf("Created %u facets\n",numFacets);

	//int offset = sizeOfMap-1; // number of vertices on front face

	////back face
	////for (int i = 0; i < xRes-1; i++) {
	////	for (int j = 0; j < yRes-1; j++) {
	////		indices.push_back(j * xRes + i + 1 + offset);
	////		indices.push_back((j+1) * xRes + i + offset);
	////		indices.push_back(j * xRes + i + offset);
	////		
	////		indices.push_back((j+1) * xRes + i + 1 + offset);
	////		indices.push_back((j+1) * xRes + i + offset);
	////		indices.push_back(j * xRes + i + 1 + offset);
	////	}
	////}

	//int n = indices.size()/3;

	ofstream stlOut;
	stlOut.open ("stlOut.stl");
	printf("Writing to STL...\n");
	stlOut << "solid kinectout\n";

	//  printf("Face Count: %u Vertex Count: %u", numFacets, xRes*yRes);

    for (int ii = 0; ii < numFacets; ++ii)
    {

        int v1 = indices[3*ii];
        int v2 = indices[3*ii + 1];
        int v3 = indices[3*ii + 2];

		vertex p1 = vertices[v1];
        vertex p2 = vertices[v2];
        vertex p3 = vertices[v3];
 
        vertex dir1 = p1.subtract(p2);
        vertex dir2  = p3.subtract(p2);

        vertex n = dir1.cross(dir2).normalize();
 
		stlOut << "  facet normal "<< n.x << " " << n.y << " " << n.z << "\n";
		stlOut << "    outer loop\n";
		stlOut << "      vertex " << p1.x << " " << p1.y << " " << p1.z << "\n";
		stlOut << "      vertex " << p2.x << " " << p2.y << " " << p2.z << "\n";
		stlOut << "      vertex " << p3.x << " " << p3.y << " " << p3.z << "\n";
		stlOut << "    endloop\n";
		stlOut << "  endfacet\n";

    }
	stlOut << "endsolid kinectout";

	printf("Complete!\n");
	stlOut.close();

}
