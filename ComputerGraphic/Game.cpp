#include "Window.h"
#include <iostream>
#include "DXCore.h"
#include "adapter.h"
#include"shader.h"
#include<string>
#include "Vertex.cpp"
#include"mesh.h"


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {

	Window win;
	DXCore dx;
	shader shader;
	Triangle t;
	plane p;

	t.init();
	std::string s1 = "vertexShader.txt";
	std::string s2 = "pixelShader.txt";
	win.init("My Window", 1024, 1024, 0, 0);
	dx.init(1024, 1024, win.hwnd, false);
	//t.createVertexBuffer(3, dx.device);

	p.init();

	shader.init(s1, s2, dx.device, &dx);
	//shader.apply(&dx, dx.devicecontext);




	while (true)
	{
		dx.clear();
		win.processMessages();
		//t.draw(dx.device, dx.devicecontext);

		shader.apply(&dx, dx.devicecontext);


		p.draw();
		dx.present();

	}


}


