#include"Mathematic.h"
#include <d3d11.h>



//Screen space position and colour

struct Vertex
{
	Vec3 position;
	Color colour;
};



//In screen space: z = 0

class Triangle {
	Vertex vertices[3];
public:

	ID3D11Buffer* vertexBuffer;


	void init(){
	vertices[0].position = Vec3(0, 1.0f, 0);
	vertices[0].colour = Color(0, 1.0f, 0);
	vertices[1].position = Vec3(-1.0f, -1.0f, 0);
	vertices[1].colour = Color(1.0f, 0, 0);
	vertices[2].position = Vec3(1.0f, -1.0f, 0);
	vertices[2].colour = Color(0, 0, 1.0f);
	}


	void createVertexBuffer(int N, ID3D11Device* device) {
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA uploadData;
		bd.ByteWidth = sizeof(Vertex) * N;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		uploadData.pSysMem = vertices;
		uploadData.SysMemPitch = 0;
		uploadData.SysMemSlicePitch = 0;
		device->CreateBuffer(&bd, &uploadData, &vertexBuffer);

	}

	void draw(ID3D11Device* device, ID3D11DeviceContext* devicecontext) {

		UINT offsets;
		offsets = 0;
		UINT strides = sizeof(Vertex);
		devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		devicecontext->Draw(3, 0);

		ID3D11RasterizerState* rasterizerState;

		D3D11_RASTERIZER_DESC rsdesc;
		ZeroMemory(&rsdesc, sizeof(D3D11_RASTERIZER_DESC));
		rsdesc.FillMode = D3D11_FILL_SOLID;
		rsdesc.CullMode = D3D11_CULL_NONE;
		device->CreateRasterizerState(&rsdesc, &rasterizerState);


		devicecontext->RSSetState(rasterizerState);


	}



};