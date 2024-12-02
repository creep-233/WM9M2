#pragma once
#include"Mathematic.h"
#include"DXCore.h"
#include"shader.h"

struct STATIC_VERTEX
{
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
};



class mesh {
public:

	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	int indicesSize;
	UINT strides;
	ID3D11Device* device;

	void init(void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, ID3D11Device* device) {
		D3D11_BUFFER_DESC bd;
		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA data;
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;
		device->CreateBuffer(&bd, &data, &indexBuffer);
		bd.ByteWidth = vertexSizeInBytes * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		data.pSysMem = vertices;
		device->CreateBuffer(&bd, &data, &vertexBuffer);
		indicesSize = numIndices;
		strides = vertexSizeInBytes;
	}


	void init(std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices)
	{
		init(&vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size(), device);
	}


	void draw(ID3D11DeviceContext* devicecontext){

		UINT offsets = 0;
		devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		devicecontext->DrawIndexed(indicesSize, 0, 0);

	}

};

class plane {
public:
	mesh Mesh;
	ID3D11DeviceContext* devicecontext;
	shader* shaders;

	float t = 0.0f;

	Matrix planeWorld;
	Matrix vp;

	void setTransform(const Vec3& position, const Vec3& rotation, const Vec3& scale) {
		// Create individual transformation matrices
		Matrix trans = Matrix::translation(position);
		Matrix rotX = Matrix::rotateX(rotation.x);
		Matrix rotY = Matrix::rotateY(rotation.y);
		Matrix rotZ = Matrix::rotateZ(rotation.z);
		Matrix scl = Matrix::scaling(scale);

		// Combine transformations by multiplying them
		planeWorld = trans * (rotX * rotY * rotZ) * scl;
	}


	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0,0,0); // For now
		v.tu = tu;
		v.tv = tv;
		return v;
	}


	void draw() {

		// Initialize from and v in the constructor
		Vec3 from = Vec3(11 * cos(t), 5, 11 * sinf(t));
		Vec3 to = Vec3(0, 1, 0);
		Vec3 up = Vec3(0, 1, 0);
		Matrix v = lookAt(from, to, up);
		planeWorld.identity();
		// vp should also be initialized properly here or in another method

		vp = PerPro(1.f, 1.f, 20.f, 100.f, 0.1f) * lookAt(from, to, up);

		shaders->updateConstantVS("staticMeshBuffer", "W", &planeWorld);
		shaders->updateConstantVS("staticMeshBuffer", "VP", &vp);
			
		Mesh.draw(devicecontext);
	}


	void init() {

		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-15, 0, -15), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(15, 0, -15), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-15, 0, 15), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(15, 0, 15), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);
		Mesh.init(vertices, indices);

		//draw();
	}

};