#include "Window.h"
#include <iostream>
#include "DXCore.h"
#include "adapter.h"
#include"shader.h"
#include<string>
#include "Vertex.cpp"
#include"mesh.h"
#include"Camera.h"
#include "GEbase.h"
#include"Character.h"
#include"Collider.h"
#include"anime.h"
#include"Texture.h"


void DebugLog(const std::string& message) {
	OutputDebugStringA(message.c_str()); // Send string to Output window
}

class FPS {
public:
	int frame = 0;
	float elapsed = 0;
	int fps = 0;

	void update(float dt) {
		frame++;
		elapsed += dt;

		if (elapsed >= 1.0f) { // Update every second
			fps = frame;
			frame = 0;
			elapsed = 0.0f;
		}
	}

	int getFps() {
		return fps;
	}

	void draw() {
		std::ostringstream logStream;
		logStream << "FPS: " << fps << "\n";
		DebugLog(logStream.str());
	}
};


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {

	float mouseSensitivity = 0.001f;

	Window win;
	DXCore dx;
	shader shaders;
	Triangle t;
	plane Plane;
	cube Cube;
	FPS fps;
	TextureManager textureManager;
	shader animationTextureShadG;
	//Camera camera(Vec3(0.0f, 5.0f, -10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	//Camera camera(Vec3(0.0f, 5.0f, -10.0f), Vec3(0.0f, 1.0f, 0.0f));
	Camera camera;  // 使用默认构造函数
	camera.setPosition(Vec3(0.0f, 5.0f, -10.0f));
	camera.setRotation(Vec3(0.0f, 0.0f, 0.0f));

	loadModel load;

	float elapsedTime = 0.f;
	GamesEngineeringBase::Timer timer;

	//t.init();
	win.init("My Window", 1024, 1024, 0, 0);
	dx.init(1024, 1024, win.hwnd, false);
	//t.createVertexBuffer(3, dx.device);

	std::string s1 = "vertexShader.txt";
	std::string s2 = "pixelShader.txt";
	//std::string s2 = "texturePixelShader.txt";
	shaders.init(s1, s2, dx.device, &dx);
	//shader.apply(&dx, dx.devicecontext);

	//load.setCore(dx);

	Plane.init(dx.devicecontext, &shaders, dx, &camera);
	//Cube.init(dx);

	//std::string modelPath = "Resources/model/pine.gem";
	//load.init(modelPath, dx, &textureManager);

	


	AnimationInstance dina;

	dina.initTexture("Resources/TRex.gem", dx, &textureManager); // 加载恐龙模型和贴图

	animationTextureShadG.initializeAnimationShader("Animation_vertex_shader.txt", "G_buffer_pixel_shader.txt", &dx); // 3d animation texture shader



	while (true)
	{
		dx.clear();
		win.processMessages();
		//t.draw(dx.device, dx.devicecontext);

		float dt = timer.dt();

		elapsedTime += dt;
		// camera control
		float speed = 6.f;
		fps.update(dt);
		fps.draw();

		//camera
		camera.captureInput(win.hwnd, mouseSensitivity);

		// Process keyboard input for movement
		bool moveForward = win.keys['W'];
		bool moveBackward = win.keys['S'];
		bool moveLeft = win.keys['A'];
		bool moveRight = win.keys['D'];
		bool reset = win.keys['T'];
		bool mouseLeftPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		if (win.keys['Q']) break; // put q in here to break the loop, more convenient than put everything in camera class
		camera.processInput(moveForward, moveBackward, moveLeft, moveRight, reset, speed, dt);

		float aspectRatio = static_cast<float>(1024) / 1024; // Example aspect ratio calculation
		Matrix viewMatrix = camera.getViewMatrix();
		Matrix projectionMatrix = camera.getProjectionMatrix(aspectRatio);
		Matrix vpMatrix = viewMatrix * projectionMatrix;

		shaders.apply(&dx);
		shaders.updateConstantVS("StaticModel","staticMeshBuffer", "VP", &vpMatrix);



		Plane.draw();
		//Cube.draw();
		//load.draw(dx.devicecontext);




		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &vpMatrix);

		// 更新恐龙动画
		Matrix w1;
		dina.update("Run", dt); // 更新动画，"Run" 是动画名称

		// 将动画骨骼矩阵传递给着色器
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "bones", dina.matrices);

		// 计算世界矩阵：缩放、旋转、位置
		w1 = Matrix::worldTrans(Vec3(0.9f, 0.9f, 0.9f), Vec3(0, 0, 0), Vec3(-10, 0, 0));

		// 将世界矩阵传递给着色器
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);

		// 应用着色器并绘制恐龙模型
		animationTextureShadG.apply(&dx);
		dina.drawTexture(&dx, animationTextureShadG, &textureManager, dx.devicecontext);





		dx.present();

	}


}



