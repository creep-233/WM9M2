#pragma once
#include"DXCore.h"
#include<fstream>
#include<sstream>
#include<string>
#include<D3Dcompiler.h>
#include<d3d11shader.h>
#include<d3d11.h>
#include"ShaderReflection.h"
#include"GEbase.h"
#include"texture.h"

//#pragma comment(lib, "dxguid.lib")



class ShaderManager {
public:
    ShaderManager(ID3D11Device* device) : m_device(device) {}
    ~ShaderManager();

    ID3D11VertexShader* loadVertexShader(const std::string& filename, const std::string& entryPoint = "VS");
    ID3D11PixelShader* loadPixelShader(const std::string& filename, const std::string& entryPoint = "PS");

private:
    ID3D11Device* m_device;
    std::map<std::string, ID3D11VertexShader*> m_vertexShaders;
    std::map<std::string, ID3D11PixelShader*> m_pixelShaders;

    ID3DBlob* compileShader(const std::string& filename, const std::string& entryPoint, const std::string& target);
    std::string readFile(const std::string& filename);
};

std::string ShaderManager::readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

ID3DBlob* ShaderManager::compileShader(const std::string& source, const std::string& entryPoint, const std::string& target) {
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(source.c_str(), source.size(), NULL, NULL, NULL, entryPoint.c_str(), target.c_str(), 0, 0, &shaderBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return nullptr;
    }
    return shaderBlob;
}

ID3D11VertexShader* ShaderManager::loadVertexShader(const std::string& filename, const std::string& entryPoint) {
    std::string code = readFile(filename);
    ID3DBlob* blob = compileShader(code, entryPoint, "vs_5_0");
    if (!blob) return nullptr;

    ID3D11VertexShader* shader = nullptr;
    m_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &shader);
    blob->Release();
    return shader;
}

ID3D11PixelShader* ShaderManager::loadPixelShader(const std::string& filename, const std::string& entryPoint) {
    std::string code = readFile(filename);
    ID3DBlob* blob = compileShader(code, entryPoint, "ps_5_0");
    if (!blob) return nullptr;

    ID3D11PixelShader* shader = nullptr;
    m_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &shader);
    blob->Release();
    return shader;
}

// ShaderManager Destructor
ShaderManager::~ShaderManager() {
    for (auto& shader : m_vertexShaders) {
        if (shader.second) shader.second->Release();
    }
    for (auto& shader : m_pixelShaders) {
        if (shader.second) shader.second->Release();
    }
}


class shader {

public:

    sampler Sampler;

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* layout;
    ID3D11Buffer* constantBuffer;
    ConstantBuffer constBufferCPU;

    std::vector<ConstantBuffer> psConstantBuffers;
    std::vector<ConstantBuffer> vsConstantBuffers;
    std::map<std::string, int> textureBindPointsVS;
    std::map<std::string, int> textureBindPointsPS;


    std::string readFile(std::string& filename)
    {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    //void loadPS(const std::string& pixelShaderHLSL, ID3D11Device* device) {
    //    ID3DBlob* compiledPixelShader;
    //    ID3DBlob* status;

    //    HRESULT hr = D3DCompile(pixelShaderHLSL.c_str(), strlen(pixelShaderHLSL.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &compiledPixelShader, &status);
    //    if (FAILED(hr))
    //    {
    //        MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Pixel Shader Error", 0);
    //        exit(0);
    //    }

    //    device->CreatePixelShader(compiledPixelShader->GetBufferPointer(), compiledPixelShader->GetBufferSize(), NULL, &pixelShader);

    //}

    void compileVS(DXCore* core, std::string filename) {
        // compile vertex shader
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        // number is version use.
        HRESULT hr = D3DCompile(filename.c_str(), strlen(filename.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        core->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);
        if (FAILED(hr)) {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "vertex shader error", 0); // check if open successfully
            exit(0);
        }

        //  create layout only for vertices.
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        core->device->CreateInputLayout(layoutDesc, 2, compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), &layout);
    }

    void compilePS(DXCore* core, std::string filename) {
        // compile pixel shader
        ID3DBlob* compiledPixelShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(filename.c_str(), strlen(filename.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &compiledPixelShader, &status);
        core->device->CreatePixelShader(compiledPixelShader->GetBufferPointer(), compiledPixelShader->GetBufferSize(), NULL, &pixelShader);
        if (FAILED(hr)) {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "pixel shader error", 0); // check if open successfully
            exit(0);
        }
    }


    void loadPS(DXCore* core, std::string hlsl)
    {
        ID3DBlob* shader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &shader, &status);
        if (FAILED(hr))
        {
            printf("%s\n", (char*)status->GetBufferPointer());
            exit(0);
        }
        core->device->CreatePixelShader(shader->GetBufferPointer(), shader->GetBufferSize(), NULL, &pixelShader);
        ConstantBufferReflection reflection;
        reflection.build(core, shader, psConstantBuffers, textureBindPointsPS, ShaderStage::PixelShader);
    }



    void loadVS(std::string& vertexShaderHLSL, DXCore* core) {
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;

        HRESULT hr = D3DCompile(vertexShaderHLSL.c_str(), vertexShaderHLSL.length(), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        if (FAILED(hr)) {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Vertex Shader Error", 0);
            exit(0);
        }

        core->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);

        // 设置顶点输入布局
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        core->device->CreateInputLayout(layoutDesc, 4, compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), &layout);

        // 反射
        ConstantBufferReflection reflection;
        reflection.build(core, compiledVertexShader, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);
    }



    void loadAnimationVS(DXCore* core, std::string hlsl) {
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        if (FAILED(hr))
        {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Vertex Shader Error", 0);
            exit(0);
        }

        core->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 								D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 								D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 								D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 								D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        core->device->CreateInputLayout(layoutDesc, 6, compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), &layout);
        ConstantBufferReflection reflection;
        reflection.build(core, compiledVertexShader, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);
    }



    void loadInstanceVS(DXCore* core, std::string hlsl) {
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        if (FAILED(hr))
        {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Vertex Shader Error", 0);
            exit(0);
        }
        core->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "INSTANCEPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, 							D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };
    }

    //constantBuffer update

    void updateConstant(std::string constantBufferName, std::string variableName, void* data, std::vector<ConstantBuffer>& buffers)
    {
        for (int i = 0; i < buffers.size(); i++)
        {
            if (buffers[i].name == constantBufferName)
            {
                buffers[i].update(variableName, data);
                return;
            }
        }
    }


    void updateConstantVS(std::string name, std::string constantBufferName, std::string variableName, void* data)
    {
        updateConstant(constantBufferName, variableName, data, vsConstantBuffers);
    }
    void updateConstantPS(std::string name, std::string constantBufferName, std::string variableName, void* data)
    {
        updateConstant(constantBufferName, variableName, data, psConstantBuffers);
    }



    void updateShaderPS(DXCore* core, std::string name, ID3D11ShaderResourceView* tex) {

        core->devicecontext->PSSetShaderResources(textureBindPointsPS[name], 1, &tex);
    }


    void apply(DXCore* core) {

        core->devicecontext->IASetInputLayout(layout);
        core->devicecontext->VSSetShader(vertexShader, NULL, 0);
        core->devicecontext->PSSetShader(pixelShader, NULL, 0);
        core->devicecontext->PSSetConstantBuffers(0, 1, &constantBuffer);
        //core->devicecontext->PSSetShaderResources(textureBindPointsPS[name], 1, &srv);
        ////core->devicecontext->PSSetSamplers(0, 1, &state);
        //Sampler.bind(core, state);


        for (int i = 0; i < vsConstantBuffers.size(); i++)
        {
            vsConstantBuffers[i].upload(core);
        }
        for (int i = 0; i < psConstantBuffers.size(); i++)
        {
            psConstantBuffers[i].upload(core);
        }
    }


    void initConstBuffer(int sizeInBytes, ID3D11Device* device) {
        D3D11_BUFFER_DESC bd;
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;
        bd.ByteWidth = sizeInBytes;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        device->CreateBuffer(&bd, NULL, &constantBuffer);
    }


	void init(std::string& vertexShaderFileName, std::string& pixelShaderFileName, ID3D11Device* device, DXCore* core) {

        std::string vertexShaderCode = readFile(vertexShaderFileName);
        std::string pixelShaderCode = readFile(pixelShaderFileName);

        int constBufferSize = sizeof(ConstantBuffer) + (16 - sizeof(ConstantBuffer) % 16);
        initConstBuffer(constBufferSize, device);

        loadVS(vertexShaderCode, core);
        loadPS(core, pixelShaderCode);

	}

    //std::_In_place_key_extract_map < std::string, shader > shaders;

    

    void initializeAnimationShader(std::string vertexFile, std::string pixelFile, DXCore* core) {
        std::string vertexShaderCode = readFile(vertexFile);
        std::string pixelShaderCode = readFile(pixelFile);
        loadAnimationVS(core, vertexShaderCode);
        loadPS(core, pixelShaderCode);
    }

    void initializeStaticShader(std::string vertexFile, std::string pixelFile, DXCore* core) {
        std::string vertexShaderCode = readFile(vertexFile);
        std::string pixelShaderCode = readFile(pixelFile);
        loadVS(vertexShaderCode, core);
        loadPS(core, pixelShaderCode);
    }

    void initializeInstancedShader(std::string vertexFile, std::string pixelFile, DXCore* core) {
        std::string vertexShaderCode = readFile(vertexFile);
        std::string pixelShaderCode = readFile(pixelFile);
        compileVS(core, vertexShaderCode);
        compilePS(core, pixelShaderCode);
    }


private:

};
