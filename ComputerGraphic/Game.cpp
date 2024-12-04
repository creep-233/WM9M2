#include "Window.h"
#include <iostream>
#include "DXCore.h"
#include "adapter.h"
#include"shader.h"
#include<string>
#include "Vertex.cpp"
#include"mesh.h"
#include"Camera.h"


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {

	Window win;
	DXCore dx;
	shader shader;
	Triangle t;
	plane Plane;
	cube Cube;
	Camera camera(Vec3(0.0f, 5.0f, -10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));

	t.init();
	std::string s1 = "vertexShader.txt";
	std::string s2 = "pixelShader.txt";
	//std::string s2 = "texturePixelShader.txt";

	win.init("My Window", 1024, 1024, 0, 0);
	dx.init(1024, 1024, win.hwnd, false);
	//t.createVertexBuffer(3, dx.device);


	shader.init(s1, s2, dx.device, &dx);
	//shader.apply(&dx, dx.devicecontext);


	Plane.init(dx.devicecontext, &shader, dx, &camera);
	//Cube.init(dx);
	



	while (true)
	{
		dx.clear();
		win.processMessages();
		//t.draw(dx.device, dx.devicecontext);

		shader.apply(&dx, dx.devicecontext);

		Plane.draw();
		//Cube.draw();
		dx.present();

	}


}


