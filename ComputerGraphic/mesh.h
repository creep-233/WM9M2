#pragma once
#include"Mathematic.h"
#include"DXCore.h"
#include"shader.h"
#include"GEMLoader.h"
#include"Camera.h"
#include"Texture.h"

struct STATIC_VERTEX
{
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
};

struct ANIMATED_VERTEX
{
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
	unsigned int bonesIDs[4];
	float boneWeights[4];
};


class mesh {
public:

	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	int indicesSize;
	UINT strides;
	ID3D11Device* device;

	void init(void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, DXCore& device) {

		D3D11_BUFFER_DESC bd;
		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA data;
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;
		device.device->CreateBuffer(&bd, &data, &indexBuffer);
		bd.ByteWidth = vertexSizeInBytes * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		data.pSysMem = vertices;
		device.device->CreateBuffer(&bd, &data, &vertexBuffer);
		indicesSize = numIndices;
		strides = vertexSizeInBytes;
	}


	void init(std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices, DXCore& device)
	{
		init(&vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size(), device);
	}

	void init(std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices, DXCore& device)
	{
		init(&vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size(), device);
	}

	void draw(ID3D11DeviceContext* devicecontext) {

		UINT offsets = 0;
		devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		devicecontext->DrawIndexed(indicesSize, 0, 0);

	}

	//void load() {

	//	std::vector<std::string> textureFilenames;

	//	for (int i = 0; i < gemmeshes.size(); i++) {
	//		Mesh mesh;
	//		std::vector<STATIC_VERTEX> vertices;
	//		for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
	//			STATIC_VERTEX v;
	//			memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
	//			vertices.push_back(v);
	//		}
	//		textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
	//		// Load texture with filename: gemmeshes[i].material.find("diffuse").getValue()
	//		mesh.init(core, vertices, gemmeshes[i].indices);
	//		meshes.push_back(mesh);
	//	}
	//}


};



class plane {
public:
	mesh Mesh;
	ID3D11DeviceContext* devicecontext;
	shader* shaders;
	Camera* camera;  
	Texture Textures;

	float t = 0.0f;

	Matrix planeWorld;

	void setTransform(const Vec3& position, const Vec3& rotation, const Vec3& scale) {
		Matrix trans = Matrix::translation(position);
		Matrix rotX = Matrix::rotateX(rotation.x);
		Matrix rotY = Matrix::rotateY(rotation.y);
		Matrix rotZ = Matrix::rotateZ(rotation.z);
		Matrix scl = Matrix::scaling(scale);
		planeWorld = trans * (rotX * rotY * rotZ) * scl;
	}

	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv) {
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0, 0, 0);
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	//void draw() {
	//	if (camera) {
	//		Matrix vp = camera->getViewMatrix() * PerPro(1.f, 1.f, 20.f, 100.f, 0.1f);

	//		shaders->updateConstantVS("staticMeshBuffer", "W", &planeWorld);
	//		shaders->updateConstantVS("staticMeshBuffer", "VP", &vp);

	//		Mesh.draw(devicecontext);
	//	}
	//}

	void draw() {
		if (shaders) {
			shaders->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
			//devicecontext->PSSetShaderResources(0, 1, &Textures.srv);
			Mesh.draw(devicecontext);
		}
	}


	void initTexture(DXCore& core, std::string filename) {
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-1.5, 0, -1.5), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(1.5, 0, -1.5), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-1.5, 0, 1.5), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(1.5, 0, 1.5), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);
		Textures.load(&core, filename);
		Mesh.init(vertices, indices, core);
	}

	void init(ID3D11DeviceContext* _devicecontext, shader* _shaders, DXCore& core, Camera* _camera) {
		devicecontext = _devicecontext;
		shaders = _shaders;
		camera = _camera;  // 初始化相机指针

		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-15, 0, -15), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(15, 0, -15), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-15, 0, 15), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(15, 0, 15), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices = { 2, 1, 0, 1, 2, 3 };

		Mesh.init(vertices, indices, core);
	}




};


class cube {
public:

	ID3D11DeviceContext* devicecontext;
	shader* shader;

	mesh Mesh;
	Matrix planeWorld;
	Matrix vp;
	Vec3 from = Vec3(11 * cos(t), 5, 11 * sin(t));
	Vec3 to = Vec3(0.0f, 1.0f, 0.0f);
	Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
	float t = 0.0f;
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		//Frame frame;
		//frame.fromVector(n);
		//v.tangent = frame.u;
		v.tangent = Vec3(0, 0, 0);
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	void init(DXCore& core) {
		std::vector<STATIC_VERTEX> vertices;
		Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
		Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
		Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
		Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
		Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
		Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);
		std::vector<unsigned int> indices;
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(0); indices.push_back(2); indices.push_back(3);
		indices.push_back(4); indices.push_back(5); indices.push_back(6);
		indices.push_back(4); indices.push_back(6); indices.push_back(7);
		indices.push_back(8); indices.push_back(9); indices.push_back(10);
		indices.push_back(8); indices.push_back(10); indices.push_back(11);
		indices.push_back(12); indices.push_back(13); indices.push_back(14);
		indices.push_back(12); indices.push_back(14); indices.push_back(15);
		indices.push_back(16); indices.push_back(17); indices.push_back(18);
		indices.push_back(16); indices.push_back(18); indices.push_back(19);
		indices.push_back(20); indices.push_back(21); indices.push_back(22);
		indices.push_back(20); indices.push_back(22); indices.push_back(23);
		Mesh.init(vertices, indices, core);
		PerPro(800.f, 600.f, 60.f, 1000.f, 0.1f);
		lookAt(from, to, up);
	}

	void draw() {
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "W", &planeWorld);
		shader->updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vp);
		Mesh.draw(devicecontext);
	}

};


//class loadModel {
//public:
//
//	std::vector<mesh> meshes;  // 存储多个子网格
//	mesh Mesh;
//	DXCore core;
//	shader* shaders;
//
//	void setCore(DXCore& _core) {
//		core = _core;
//	}
//
//	void load(std::string filename) {
//
//		std::vector<mesh> meshes;
//
//		GEMLoader::GEMModelLoader loader;
//		std::vector<GEMLoader::GEMMesh> gemmeshes;
//		loader.load(filename, gemmeshes);
//		for (int i = 0; i < gemmeshes.size(); i++) {
//			mesh mesh;
//			std::vector<STATIC_VERTEX> vertices;
//			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
//				STATIC_VERTEX v;
//				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
//				vertices.push_back(v);
//			}
//			Mesh.init(vertices, gemmeshes[i].indices, core);
//			meshes.push_back(mesh);
//		}
//
//	}
//
//	void draw(ID3D11DeviceContext* deviceContext) {
//		for (auto& mesh : meshes) {
//			mesh.draw(deviceContext);
//		}
//	}
//
//};





class Sphere {
public:
	mesh Mesh;
	Texture text;
	// make sphere huge enough to create a sky!
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		// help function
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0, 0, 0); // For now
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	void init(DXCore& core, int rings, int segments, float radius, TextureManager* textures) {
		std::vector<STATIC_VERTEX> vertices;
		std::vector<unsigned int> indices;
		// create sphere vertices
		for (int lat = 0; lat <= rings; lat++) {
			float theta = lat * M_PI / rings;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);
			for (int lon = 0; lon <= segments; lon++) {
				float phi = lon * 2.0f * M_PI / segments;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);
				Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta, radius * sinTheta * sinPhi);
				Vec3 normal = position.normalize();
				float tu = 1.0f - (float)lon / segments;
				float tv = 1.0f - (float)lat / rings;
				vertices.push_back(addVertex(position, normal, tu, tv));
			}
		}


		// create indices vertices
		for (int lat = 0; lat < rings; lat++)
		{
			for (int lon = 0; lon < segments; lon++)
			{
				int current = lat * (segments + 1) + lon;
				int next = current + segments + 1;
				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);
				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}
		//	text.load(&core, "SkyDome.png");

		textures->load(&core, "SkyDome.png");
		Mesh.init(vertices, indices, core);
	}

	void init(DXCore& core, int rings, int segments, float radius, std::string filename) {
		std::vector<STATIC_VERTEX> vertices;
		std::vector<unsigned int> indices;
		// create sphere vertices
		for (int lat = 0; lat <= rings; lat++) {
			float theta = lat * M_PI / rings;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);
			for (int lon = 0; lon <= segments; lon++) {
				float phi = lon * 2.0f * M_PI / segments;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);
				Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta, radius * sinTheta * sinPhi);
				Vec3 normal = position.normalize();
				float tu = 1.0f - (float)lon / segments;
				float tv = (float)lat / rings;
				vertices.push_back(addVertex(position, normal, tu, tv));
			}
		}
		for (int lat = 0; lat < rings; lat++)
		{
			for (int lon = 0; lon < segments; lon++)
			{
				int current = lat * (segments + 1) + lon;
				int next = current + segments + 1;
				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);
				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}
		text.load(&core, filename);
		Mesh.init(vertices, indices, core);
	}

	void draw(ID3D11DeviceContext* devicecontext) {
		Mesh.draw(devicecontext);
	}
	void updateWorld(Matrix w, shader shad, DXCore core) {
		// submit world matrix to vertex shader
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &w); // adjust buffer
		shad.apply(&core); // apply adjusted buffer immediately
	}
};

class Hemisphere {
public:
	mesh Mesh;
	Texture text;

	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv) {
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0, 0, 0); // For now
		v.tu = tu;
		v.tv = tv;
		return v;
	}
	void init(DXCore& core, int rings, int segments, float radius, std::string filename, bool isUpper = true) {
		std::vector<STATIC_VERTEX> vertices;
		std::vector<unsigned int> indices;

		int startLat = isUpper ? 0 : rings / 2;
		int endLat = isUpper ? rings / 2 : rings;

		for (int lat = startLat; lat <= endLat; lat++) {
			float theta = lat * M_PI / rings;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);

			for (int lon = 0; lon <= segments; lon++) {
				float phi = lon * 2.0f * M_PI / segments;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);

				Vec3 position(
					radius * sinTheta * cosPhi,
					radius * cosTheta,
					radius * sinTheta * sinPhi
				);
				Vec3 normal = position.normalize();
				float tu = 1.0f - (float)lon / segments;
				float tv = isUpper ? (float)lat / (rings / 2) : (float)(lat - rings / 2) / (rings / 2);
				vertices.push_back(addVertex(position, normal, tu, tv));
			}
		}

		for (int lat = startLat; lat < endLat; lat++) {
			for (int lon = 0; lon < segments; lon++) {
				int current = lat * (segments + 1) + lon;
				int next = current + segments + 1;

				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);

				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}

		text.load(&core, filename);
		Mesh.init(vertices, indices, core);
	}

	void draw(ID3D11DeviceContext* devicecontext) {
		Mesh.draw(devicecontext);
	}

	void updateWorld(Matrix w, shader shad, DXCore core) {
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &w); // Update world matrix
		shad.apply(&core);
	}
};

class loadModel {
	// all complex meshes
public:
	std::vector<mesh> meshes;
	mesh Mesh;
	DXCore core;
	std::vector<std::string> textureFilenames;


	void init(std::string filename, DXCore& core, TextureManager* textures) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(filename, gemmeshes);
		for (int i = 0; i < gemmeshes.size(); i++) {
			mesh Mesh;
			std::vector<STATIC_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
			}
			textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
			textures->load(&core, gemmeshes[i].material.find("diffuse").getValue());
			//textureFilenames.push_back(gemmeshes[i].material.find("normals").getValue());
			//textures->load(&core, gemmeshes[i].material.find("normals").getValue());
			Mesh.init(vertices, gemmeshes[i].indices, core);
			meshes.push_back(Mesh);
		}
		std::cout << "Textures" << textures << std::endl;
	}


	void loadWithoutTexture(std::string filename, DXCore& core) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(filename, gemmeshes);
		for (int i = 0; i < gemmeshes.size(); i++) {
			mesh Mesh;
			std::vector<STATIC_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
			}
			Mesh.init(vertices, gemmeshes[i].indices, core);
			meshes.push_back(Mesh);
		}
	}
	void updateWorld(Matrix w, shader shad, DXCore core) {
		// submit world matrix to vertex shader
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &w); // adjust buffer
		shad.apply(&core); // apply adjusted buffer immediately
	}

	void draw(ID3D11DeviceContext* devicecontext) {
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].draw(devicecontext);
		}

	}


	void drawTexture(DXCore* core, ID3D11DeviceContext* devicecontext, shader shaders, TextureManager* textureM) {
		for (int i = 0; i < meshes.size(); i++)
		{
			shaders.updateShaderPS(core, "tex", textureM->find(textureFilenames[i]));
			meshes[i].draw(devicecontext);
		}

	}
};
