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


void RandomCreation(std::vector<Matrix>& matrices, unsigned int count) {
	matrices.clear(); // clear matrices to avoid some bugs
	for (unsigned int i = 0; i < count; i++) {
		float x = static_cast<float>(rand() % 30);
		float z = static_cast<float>(rand() % 30);
		Matrix w2;
		w2 = Matrix::worldTrans(Vec3(0.01, 0.01, 0.01), Vec3(0, 0, 0), Vec3(x, 0, z));
		matrices.push_back(w2);
	}
}

void SaveTrees(std::vector<Matrix>& matrices) {
	// use fstream to save random tree position into binary file
	std::ofstream file("Tree.dat", std::ios::binary);
	if (!file.is_open()) return;

	size_t count = matrices.size();
	file.write(reinterpret_cast<const char*>(&count), sizeof(count));
	file.write(reinterpret_cast<const char*>(matrices.data()), sizeof(Matrix) * count);
	file.close();
}

bool LoadTrees(std::vector<Matrix>& matrices, const std::string& filename) {
	// load file, set it to bool so the game can check if world file exist to decide generate a new one or just load
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) return false;

	size_t count;
	file.read(reinterpret_cast<char*>(&count), sizeof(count));
	matrices.resize(count);
	file.read(reinterpret_cast<char*>(matrices.data()), sizeof(Matrix) * count);
	file.close();
	return true;
}


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
	loadModel load;
	//Camera camera(Vec3(0.0f, 5.0f, -10.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	//Camera camera(Vec3(0.0f, 5.0f, -10.0f), Vec3(0.0f, 1.0f, 0.0f));

	shader GtextureShaders;
	shader GanimationTextureShaders;
	shader shadowShaders;
	shader GtextureAlphaShaders;



	//skybox
	Sphere skybox;
	shader skyBoxShader;

	win.init("CGDemo", 1024, 1024, 0, 0);
	dx.init(1024, 1024, win.hwnd, false);

	//dinasor
	AnimationInstance dinasour;


	//skybox
	skyBoxShader.initializeStaticShader("3D_vertex_shader.txt", "G_buffer_pixel_shader.txt", &dx);
	skybox.init(dx, 35, 35, 90.0f, "Resources/Textures/Noon_07_3K.png");


	//pine
	loadModel pine;
	GtextureAlphaShaders.initializeStaticShader("3D_vertex_shader.txt", "G_buffer_pixel_shader_alpha.txt", &dx);
	pine.init("Resources/pine.gem", dx, &textureManager); // ���� pine ģ��

	//TextureManager textureManager;


	//player
	GanimationTextureShaders.initializeAnimationShader("Animation_vertex_shader.txt", "G_buffer_pixel_shader.txt", &dx);

	//---------------------------------------------------------------------------

	Camera camera;  // ʹ��Ĭ�Ϲ��캯��
	camera.setPosition(Vec3(0.0f, 5.0f, -10.0f));
	camera.setRotation(Vec3(0.0f, 0.0f, 0.0f));

	float elapsedTime = 0.f;
	GamesEngineeringBase::Timer timer;

	//t.init();
	//win.init("My Window", 1024, 1024, 0, 0);
	//dx.init(1024, 1024, win.hwnd, false);
	//t.createVertexBuffer(3, dx.device);

	std::string s1 = "vertexShader.txt";
	std::string s2 = "pixelShader.txt";
	//std::string s2 = "texturePixelShader.txt";
	shaders.init(s1, s2, dx.device, &dx);
	//shaders.apply(&dx);

	//load.setCore(dx);



	//plane
	Plane.init(dx.devicecontext, &shaders, dx, &camera);
	Plane.setTransform(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec3(50.0f, 1.0f, 50.0f));
	Plane.initTexture(dx, "Resources/Textures/stump01.png");



	//skybox
	shader textureShadG;
	textureShadG.initializeStaticShader("3D_vertex_shader.txt", "G_buffer_pixel_shader.txt", &dx);



	//dinasour


	dinasour.initTexture("Resources/TRex.gem", dx, &textureManager); // ���ؿ���ģ�ͺ���ͼ

	animationTextureShadG.initializeAnimationShader("Animation_vertex_shader.txt", "G_buffer_pixel_shader.txt", &dx); // 3d animation texture shader


	//pine
	std::vector<Matrix> trees;

	 ////pine ����������������̶�λ�ã�
	//trees.clear(); // ��� trees�������ظ�
	//trees.push_back(Matrix::worldTrans(Vec3(0.5f, 0.5f, 0.5f), Vec3(0, 0, 0), Vec3(5.0f, 0.0f, 5.0f)));  // ��һ������λ��
	//trees.push_back(Matrix::worldTrans(Vec3(0.5f, 0.5f, 0.5f), Vec3(0, 0, 0), Vec3(-5.0f, 0.0f, -5.0f))); // �ڶ�������λ��wwwwwwwwww
	//Matrix pineWorldMatrix = Matrix::worldTrans(Vec3(0.02f, 0.02f, 0.02f), Vec3(0, 0, 0), Vec3(0.0f, 0.0f, 0.0f));


	//if (trees.empty()) {
	//	for (int i = 0; i < 3; ++i) {
	//		float x = static_cast<float>(rand() % 50 - 25);
	//		float z = static_cast<float>(rand() % 50 - 25);
	//		Matrix treeWorld = Matrix::worldTrans(Vec3(0.5f, 0.5f, 0.5f), Vec3(0, 0, 0), Vec3(x, 0, z));
	//		DebugLog("Tree Matrix: " + std::to_string(treeWorld.a[0][0])); // ����֤����ֵ
	//		trees.push_back(treeWorld);
	//	}
	//}

	int treeCount = 20;
	bool running = true;
	if (!LoadTrees(trees, "world.dat")) {
		// if there is no file, create one and save
		// TODO one potential improvement: The previous dat file must be deleted if want to change number of trees, maybe can add a number check
		RandomCreation(trees, 10);
		SaveTrees(trees);
	}


	//GEMLoader::GEMModelLoader loader;
	//std::vector<GEMLoader::GEMMesh> gemmeshes;
	//GEMLoader::GEMAnimation gemanimation;

	//// ���� GEM �ļ�
	//loader.load("Resources/TRex.gem", gemmeshes, gemanimation);

	//// �����������
	//for (const auto& anim : gemanimation.animations) {
	//	std::string logMessage = "Animation Name: " + anim.name + "\n";
	//	DebugLog(logMessage);
	//}


	AnimationManager animManager;
	animManager.addAnimationInstance("dino", &dinasour);


	BoundingVolume dinoBoundingBox;
	dinoBoundingBox.points = { Vec3(-0.5f, 0.0f, -0.5f), Vec3(0.5f, 2.0f, 0.5f) }; // ���¿�����Χ
	dinoBoundingBox.applyTransform(Vec3(0.9f, 0.9f, 0.9f), Vec3(0, 0, 0), Vec3(-10, 0, 0)); // ���������š���ת��λ��

	// 2. ������İ�Χ��
	BoundingVolume cameraBoundingBox;
	cameraBoundingBox.points = { Vec3(-0.2f, -0.2f, -0.2f), Vec3(0.2f, 0.2f, 0.2f) }; // �������Χ
	cameraBoundingBox.applyTransform(Vec3(1.0f, 1.0f, 1.0f), Vec3(0, 0, 0), camera.position); // �����λ��


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
		bool reset = win.keys['I'];
		bool mouseLeftPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		if (win.keys['P']) break; // put q in here to break the loop, more convenient than put everything in camera class
		camera.processInput(moveForward, moveBackward, moveLeft, moveRight, reset, speed, dt);

		float aspectRatio = static_cast<float>(1024) / 1024; // Example aspect ratio calculation
		Matrix viewMatrix = camera.getViewMatrix();
		Matrix projectionMatrix = camera.getProjectionMatrix(aspectRatio);
		Matrix vpMatrix = viewMatrix * projectionMatrix;




		cameraBoundingBox.clear();
		cameraBoundingBox.points = { Vec3(-2.0f, -2.0f, -2.0f), Vec3(2.0f, 2.0f, 2.0f) };
		cameraBoundingBox.applyTransform(Vec3(1.0f, 1.0f, 1.0f), Vec3(0, 0, 0), camera.position);


		dinoBoundingBox.clear();
		dinoBoundingBox.points = { Vec3(-0.5f, 0.0f, -0.5f), Vec3(0.5f, 2.0f, 0.5f) };
		dinoBoundingBox.applyTransform(Vec3(0.9f, 0.9f, 0.9f), Vec3(0, 0, 0), Vec3(-10, 0, 0));

		if (cameraBoundingBox.isOverlapping(dinoBoundingBox)) {
			DebugLog("Collision detected! Changing animation to 'Run'.");
			animManager.update("dino", "Run", dt); 
		}
		else {
			animManager.update("dino", "Idle", dt); 
		}



		// Plane
		shaders.apply(&dx);
		shaders.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vpMatrix);
		Plane.draw();

		//Cube.draw();
		//load.draw(dx.devicecontext);




		// skybox
		Matrix skyboxWorldMatrix = Matrix::worldTrans(Vec3(1, 1, 1), Vec3(0, 0, 0), camera.position);
		skyBoxShader.apply(&dx);
		skyBoxShader.updateConstantVS("SkyDome", "staticMeshBuffer", "W", &skyboxWorldMatrix);
		skyBoxShader.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vpMatrix);
		dx.devicecontext->PSSetShaderResources(0, 1, &skybox.text.srv);
		skybox.draw(dx.devicecontext);



		//dinasour
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &vpMatrix);

		// ���¿�������
		Matrix w1;
		//dinasour.update("Idle", dt); // ���¶�����"Run" �Ƕ�������

		// �������������󴫵ݸ���ɫ��
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "bones", dinasour.matrices);

		// ��������������š���ת��λ��
		w1 = Matrix::worldTrans(Vec3(0.9f, 0.9f, 0.9f), Vec3(0, 0, 0), Vec3(-10, 0, 0));

		// ��������󴫵ݸ���ɫ��
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);

		// Ӧ����ɫ�������ƿ���ģ��p
		animationTextureShadG.apply(&dx);
		dinasour.drawTexture(&dx, animationTextureShadG, &textureManager, dx.devicecontext);



		////pine
		//// ���� G-Buffer ������ɫ���ĳ���������
		//GtextureAlphaShaders.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vpMatrix);
		//GtextureAlphaShaders.apply(&dx);

		//// ��Ⱦ pine ģ��
		//for (const Matrix& mat : trees) {
		//	pine.updateWorld(mat, GtextureAlphaShaders, dx); // ���� pine ���������
		//	pine.drawTexture(&dx, dx.devicecontext, GtextureAlphaShaders, &textureManager);
		//}

		// pine
		//GtextureAlphaShaders.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vpMatrix);
		//GtextureAlphaShaders.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &pineWorldMatrix);
		//GtextureAlphaShaders.apply(&dx);
		//pine.updateWorld(pineWorldMatrix, GtextureAlphaShaders, dx);
		//pine.drawTexture(&dx, dx.devicecontext, GtextureAlphaShaders, &textureManager);

		GtextureAlphaShaders.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vpMatrix);
		GtextureAlphaShaders.apply(&dx);
		for (const Matrix& mat : trees) {
			pine.updateWorld(mat, GtextureAlphaShaders, dx);
			pine.drawTexture(&dx, dx.devicecontext, GtextureAlphaShaders, &textureManager);
		}



		dx.present();

	}


}



