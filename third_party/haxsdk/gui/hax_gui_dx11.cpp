#define HAXGUI_INCLUDE_INTERNAL
#include "hax_gui_dx11.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler")

namespace Hax::Gui
{
    extern const char* g_VertexShaderText;
    extern const char* g_PixelShaderText;

    struct DirectX11Backend : IBackend
    {
                                DirectX11Backend(Context& ctx, ID3D11Device* device);
                                ~DirectX11Backend() override;

        void                    Render() override;

        Texture2D               CreateTexture(TextureFormat format, const uint8* pixels, uint32 width, uint32 height, uint32 depth = 1) override;
        void                    UpdateTextureRegion(Texture2D tex, const RectU& region, const uint8* src, uint32 srcPitch, uint32 arraySlice = 0) override;
        void                    DestroyTexture(Texture2D handle) override;

    private:
        void                    CompileVertexShader(const char* vertexShaderText);
        void                    CompilePixelShader(const char* pixelShaderText);
        void                    CreateConstantBuffers();
        void                    CreateRenderStates();
        void                    SetupRenderState(const Vector2& viewportSize);
        void                    CopyRenderItems(Vector<Layer*>& layers);
        void                    RenderLayers(Vector<Layer*>& layers);

    private:
        Context&                m_Context;
        ID3D11Device*           m_Device                = nullptr;
        ID3D11DeviceContext*    m_DeviceContext         = nullptr;
        ID3D11VertexShader*     m_VertexShader          = nullptr;
        ID3D11InputLayout*      m_InputLayout           = nullptr;
        ID3D11Buffer*           m_MatrixContantBuffer   = nullptr;
        ID3D11PixelShader*      m_PixelShader           = nullptr;
        ID3D11BlendState*       m_BlendState            = nullptr;
        ID3D11RasterizerState*  m_RasterizerState       = nullptr;
        ID3D11DepthStencilState* m_DepthStencilState    = nullptr;
        ID3D11SamplerState*     m_Sampler               = nullptr;
        ID3D11Buffer*           m_InstanceBuffer        = nullptr;
        size_t                  m_InstanceBufferSize    = 0;
    };

    DirectX11Backend::DirectX11Backend(Context& ctx, ID3D11Device* device) : m_Context(ctx)
    {
        HAX_ASSERT(device != nullptr);

        m_Device = device;
        m_Device->GetImmediateContext(&m_DeviceContext);

        this->CompileVertexShader(g_VertexShaderText);
        this->CreateConstantBuffers();
        this->CompilePixelShader(g_PixelShaderText);
        this->CreateRenderStates();
    }

    DirectX11Backend::~DirectX11Backend()
    {
        m_BlendState->Release();
        m_RasterizerState->Release();
        m_DepthStencilState->Release();
        m_Sampler->Release();
        m_PixelShader->Release();
        m_VertexShader->Release();
        m_InputLayout->Release();
        m_MatrixContantBuffer->Release();
        m_InstanceBuffer->Release();
        m_DeviceContext->Release();
    }

    void DirectX11Backend::Render()
    {
        HAX_ASSERT(m_Device != nullptr && "API not initialized");

        CopyRenderItems(m_Context.Layers);

        struct BackupState
        {
            UINT                        ScissorRectsCount, ViewportsCount;
            D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
            D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
            ID3D11RasterizerState*      RS;
            ID3D11BlendState*           BlendState;
            FLOAT                       BlendFactor[4];
            UINT                        SampleMask;
            UINT                        StencilRef;
            ID3D11DepthStencilState*    DepthStencilState;
            ID3D11ShaderResourceView*   PSShaderResources[3];
            ID3D11SamplerState*         PSSampler;
            ID3D11PixelShader*          PS;
            ID3D11VertexShader*         VS;
            ID3D11GeometryShader*       GS;
            ID3D11HullShader*           HS;
            ID3D11DomainShader*         DS;
            ID3D11ComputeShader*        CS;
            D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
            ID3D11Buffer*               IndexBuffer, *VertexBuffer;
            ID3D11Buffer*               VSConstantBuffer;
            UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
            DXGI_FORMAT                 IndexBufferFormat;
            ID3D11InputLayout*          InputLayout;
        };

        BackupState old{};
        old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
        m_DeviceContext->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
        m_DeviceContext->RSGetViewports(&old.ViewportsCount, old.Viewports);
        m_DeviceContext->RSGetState(&old.RS);
        m_DeviceContext->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
        m_DeviceContext->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
        m_DeviceContext->PSGetShaderResources(0, 3, old.PSShaderResources);
        m_DeviceContext->PSGetSamplers(0, 1, &old.PSSampler);

        UINT numInstances = 0;
        m_DeviceContext->PSGetShader(&old.PS, nullptr, &numInstances); HAX_ASSERT(numInstances == 0);
        m_DeviceContext->VSGetShader(&old.VS, nullptr, &numInstances); HAX_ASSERT(numInstances == 0);
        m_DeviceContext->GSGetShader(&old.GS, nullptr, &numInstances); HAX_ASSERT(numInstances == 0);
        m_DeviceContext->HSGetShader(&old.HS, nullptr, &numInstances); HAX_ASSERT(numInstances == 0);
        m_DeviceContext->DSGetShader(&old.DS, nullptr, &numInstances); HAX_ASSERT(numInstances == 0);
        m_DeviceContext->CSGetShader(&old.CS, nullptr, &numInstances); HAX_ASSERT(numInstances == 0);
        m_DeviceContext->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);

        m_DeviceContext->IAGetPrimitiveTopology(&old.PrimitiveTopology);
        m_DeviceContext->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
        m_DeviceContext->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
        m_DeviceContext->IAGetInputLayout(&old.InputLayout);

        SetupRenderState(m_Context.Viewport.Size);
        RenderLayers(m_Context.Layers);

        m_DeviceContext->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
        m_DeviceContext->RSSetViewports(old.ViewportsCount, old.Viewports);
        m_DeviceContext->RSSetState(old.RS); if (old.RS) old.RS->Release();
        m_DeviceContext->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask);  if (old.BlendState) old.BlendState->Release();
        m_DeviceContext->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef);     if (old.DepthStencilState) old.DepthStencilState->Release();
        m_DeviceContext->PSSetShaderResources(0, 3, old.PSShaderResources);
                                                                                            if (old.PSShaderResources[0]) old.PSShaderResources[0]->Release();
                                                                                            if (old.PSShaderResources[1]) old.PSShaderResources[1]->Release();
                                                                                            if (old.PSShaderResources[2]) old.PSShaderResources[2]->Release();
        m_DeviceContext->PSSetSamplers(0, 1, &old.PSSampler);                               if (old.PSSampler) old.PSSampler->Release();
        m_DeviceContext->PSSetShader(old.PS, nullptr, 0);                                   if (old.PS) old.PS->Release();
        m_DeviceContext->VSSetShader(old.VS, nullptr, 0);                                   if (old.VS) old.VS->Release();
        m_DeviceContext->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer);                 if (old.VSConstantBuffer) old.VSConstantBuffer->Release();
        m_DeviceContext->GSSetShader(old.GS, nullptr, 0);                                   if (old.GS) old.GS->Release();
        m_DeviceContext->HSSetShader(old.HS, nullptr, 0);                                   if (old.HS) old.HS->Release();
        m_DeviceContext->DSSetShader(old.DS, nullptr, 0);                                   if (old.DS) old.DS->Release();
        m_DeviceContext->CSSetShader(old.CS, nullptr, 0);                                   if (old.CS) old.CS->Release();

        m_DeviceContext->IASetPrimitiveTopology(old.PrimitiveTopology);
        m_DeviceContext->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset);               if (old.IndexBuffer) old.IndexBuffer->Release();
        m_DeviceContext->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
        m_DeviceContext->IASetInputLayout(old.InputLayout);                                                             if (old.InputLayout) old.InputLayout->Release();
    }

    static DXGI_FORMAT ToDxgiFormat(TextureFormat format)
    {
        switch (format)
        {
            case TextureFormat::R8_UNorm: return DXGI_FORMAT_R8_UNORM;
            case TextureFormat::R8G8B8A8_UNorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
        }

        HAX_ASSERT(0);
        return DXGI_FORMAT_UNKNOWN;
    }

    Texture2D DirectX11Backend::CreateTexture(TextureFormat format, const uint8* pixels, uint32 width, uint32 height, uint32 depth)
    {
        HAX_ASSERT(m_Device != nullptr && "API not initialized");

        DXGI_FORMAT dxgiFormat = ToDxgiFormat(format);
        D3D11_TEXTURE2D_DESC desc =
        {
            .Width = width,
            .Height = height,
            .MipLevels = 1,
            .ArraySize = depth,
            .Format = dxgiFormat,
            .SampleDesc = {.Count = 1},
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_SHADER_RESOURCE,
            .CPUAccessFlags = 0,
        };

        ID3D11Texture2D* texture = nullptr;
        D3D11_SUBRESOURCE_DATA subResource = 
        {
            .pSysMem = pixels,
            .SysMemPitch = GetBytesPerChannel(format) * GetChannelCount(format) * (uint32)width,
            .SysMemSlicePitch = 0
        };
        m_Device->CreateTexture2D(&desc, pixels == nullptr ? nullptr : &subResource, &texture);
        HAX_ASSERT(texture != nullptr);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {.Format = desc.Format};
        if (depth > 1)
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvDesc.Texture2DArray = {.MipLevels = desc.MipLevels, .ArraySize = depth};
        }
        else
        {
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D = {.MipLevels = desc.MipLevels};
        }

        ID3D11ShaderResourceView* srv = nullptr;
        m_Device->CreateShaderResourceView(texture, &srvDesc, &srv);
        texture->Release();

        return Texture2D 
        {
            .Tex = (void*)texture,
            .View = (void*)srv,
            .Width = width,
            .Height = height,
            .Depth = depth
        };
    }

    void DirectX11Backend::UpdateTextureRegion(Texture2D tex, const RectU& region, const uint8* src, uint32 srcPitch, uint32 arraySlice)
    {
        if (tex.Tex == nullptr)
            return;

        D3D11_BOX dstBox =
        {
            .left   = (UINT)region.MinX,
            .top    = (UINT)region.MinY,
            .front  = 0,
            .right  = (UINT)region.MaxX,
            .bottom = (UINT)region.MaxY,
            .back   = 1
        };

        UINT subresource = D3D11CalcSubresource(0, arraySlice, 1);
        m_DeviceContext->UpdateSubresource((ID3D11Resource*)tex.Tex, subresource, &dstBox, src, srcPitch, 0);
    }

    void DirectX11Backend::DestroyTexture(Texture2D texture2D)
    {
        HAX_ASSERT(m_Device != nullptr && "API not initialized");

        if (texture2D.View != nullptr)
            ((ID3D11ShaderResourceView*)texture2D.View)->Release();
    }

    void DirectX11Backend::CompileVertexShader(const char* vertexShaderText)
    {
        HRESULT res;
        ID3DBlob* vertexShaderBlob;

        res = D3DCompile(vertexShaderText, strlen(vertexShaderText), nullptr, nullptr, nullptr, "main", "vs_4_0", 0, 0, &vertexShaderBlob, nullptr);
        HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to compile vertex shader: %08X", res);

        res = m_Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &m_VertexShader);
        HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to create vertex shader: %08X", res);

        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            {"PARAMS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (uint32)offsetof(RenderItem, Params14),  D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"PARAMS",  1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (uint32)offsetof(RenderItem, Params58),  D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"PARAMS",  2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (uint32)offsetof(RenderItem, Params912), D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"PARAM",   0, DXGI_FORMAT_R32_FLOAT,          0, (uint32)offsetof(RenderItem, Param13),   D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"PARAM",   1, DXGI_FORMAT_R32_FLOAT,          0, (uint32)offsetof(RenderItem, Param14),   D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"TYPE",    0, DXGI_FORMAT_R32_UINT,           0, (uint32)offsetof(RenderItem, Type),      D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"COLOR",   0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, (uint32)offsetof(RenderItem, Color1),    D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"COLOR",   1, DXGI_FORMAT_R8G8B8A8_UNORM,     0, (uint32)offsetof(RenderItem, Color2),    D3D11_INPUT_PER_INSTANCE_DATA, 1},
            {"SINCOS",  0, DXGI_FORMAT_R32G32_FLOAT,       0, (uint32)offsetof(RenderItem, Sin),       D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };

        res = m_Device->CreateInputLayout(layout, _countof(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_InputLayout);
        HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to create input layout: %08X", res);

        vertexShaderBlob->Release();
    }

    void DirectX11Backend::CompilePixelShader(const char* pixelShaderText)
    {
        HRESULT res;
        ID3DBlob* pixelShaderBlob;

        res = D3DCompile(pixelShaderText, strlen(pixelShaderText), nullptr, nullptr, nullptr, "main", "ps_4_0", 0, 0, &pixelShaderBlob, nullptr);
        HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to compile pixel shader: %08X", res);

        res = m_Device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &m_PixelShader);
        HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to create pixel shader: %08X", res);

        pixelShaderBlob->Release();
    }

    struct VERTEX_CONSTANT_BUFFER_DX11
    {
        float MVP[4][4];
    };

    void DirectX11Backend::CreateConstantBuffers()
    {
        D3D11_BUFFER_DESC desc =
        {
            .ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER_DX11),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
            .MiscFlags = 0
        };

        HRESULT res = m_Device->CreateBuffer(&desc, nullptr, &m_MatrixContantBuffer);
        HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to create const buffer: %08X", res);
    }

    void DirectX11Backend::CreateRenderStates()
    {
        // Blend
        {
            D3D11_BLEND_DESC desc =
            {
                .AlphaToCoverageEnable = false,
                .RenderTarget = 
                {
                    {
                        .BlendEnable = true,
                        .SrcBlend = D3D11_BLEND_ONE,
                        .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
                        .BlendOp = D3D11_BLEND_OP_ADD,
                        .SrcBlendAlpha = D3D11_BLEND_ONE,
                        .DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
                        .BlendOpAlpha = D3D11_BLEND_OP_ADD,
                        .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
                    }
                }
            };

            HRESULT res = m_Device->CreateBlendState(&desc, &m_BlendState);
            HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to create blend state: %08X", res);
        }

        // Rasterizer
        {
            D3D11_RASTERIZER_DESC desc =
            {
                .FillMode = D3D11_FILL_SOLID,
                .CullMode = D3D11_CULL_NONE,
                .DepthClipEnable = true,
                .ScissorEnable = true
            };

            HRESULT res = m_Device->CreateRasterizerState(&desc, &m_RasterizerState);
            HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to create rast state: %08X", res);
        }

        // Depth-stencil
        {
            D3D11_DEPTH_STENCIL_DESC desc =
            {
                .DepthEnable = false,
                .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO,
                .StencilEnable = false,
            };

            HRESULT res = m_Device->CreateDepthStencilState(&desc, &m_DepthStencilState);
            HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to create depth state: %08X", res);
        }

        // Sampler
        {
            D3D11_SAMPLER_DESC desc =
            {
                .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
                .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
                .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
                .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
                .MipLODBias = 0.0f,
                .ComparisonFunc = D3D11_COMPARISON_ALWAYS,
                .MinLOD = 0.0f,
                .MaxLOD = 0.0f
            };

            HRESULT res = m_Device->CreateSamplerState(&desc, &m_Sampler);
            HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to create sampler state: %08X", res);
        }
    }

    void DirectX11Backend::SetupRenderState(const Vector2& viewportSize)
    {
        D3D11_VIEWPORT vp =
        {
            .TopLeftX = 0.0f,
            .TopLeftY = 0.0f,
            .Width = viewportSize.X,
            .Height = viewportSize.Y,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f
        };
        m_DeviceContext->RSSetViewports(1, &vp);

        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT res = m_DeviceContext->Map(m_MatrixContantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to map const buffer: %08X", res);

            VERTEX_CONSTANT_BUFFER_DX11* constantBuffer = (VERTEX_CONSTANT_BUFFER_DX11*)mappedResource.pData;
            float L = 0;
            float R = viewportSize.X;
            float T = 0;
            float B = viewportSize.Y;
            float mvp[4][4] =
            {
                {2.0f / (R - L),    0.0f,               0.0f,   0.0f },
                {0.0f,              2.0f / (T - B),     0.0f,   0.0f },
                {0.0f,              0.0f,               0.5f,   0.0f },
                {(R + L) / (L - R), (T + B) / (B - T),  0.5f,   1.0f },
            };
            memcpy(&constantBuffer->MVP, mvp, sizeof(mvp));
            m_DeviceContext->Unmap(m_MatrixContantBuffer, 0);
        }

        uint32 stride = sizeof(RenderItem);
        uint32 offset = 0;
        m_DeviceContext->IASetInputLayout(m_InputLayout);
        m_DeviceContext->IASetVertexBuffers(0, 1, &m_InstanceBuffer, &stride, &offset);
        m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
        m_DeviceContext->VSSetConstantBuffers(0, 1, &m_MatrixContantBuffer);
        m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);
        m_DeviceContext->PSSetSamplers(0, 1, &m_Sampler);
        m_DeviceContext->GSSetShader(nullptr, nullptr, 0);
        m_DeviceContext->HSSetShader(nullptr, nullptr, 0);
        m_DeviceContext->DSSetShader(nullptr, nullptr, 0);
        m_DeviceContext->CSSetShader(nullptr, nullptr, 0);

        const float blend_factor[4] = {0.f, 0.f, 0.f, 0.f};
        m_DeviceContext->OMSetBlendState(m_BlendState, blend_factor, 0xffffffff);
        m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 0);
        m_DeviceContext->RSSetState(m_RasterizerState);

        m_DeviceContext->PSSetShaderResources(1, 1, (ID3D11ShaderResourceView**)&g_Context->AtlasArray.View);
        m_DeviceContext->PSSetShaderResources(2, 1, (ID3D11ShaderResourceView**)&g_Context->Bitmap.Texture.View);
    }

    void DirectX11Backend::CopyRenderItems(Vector<Layer*>& layers)
    {
        size_t totalItems = 0;
        for (Layer* layer : layers)
            totalItems += layer->RenderItems.Size();

        if (m_InstanceBuffer == nullptr || m_InstanceBufferSize < totalItems)
        {
            if (m_InstanceBuffer) 
            { 
                m_InstanceBuffer->Release();
                m_InstanceBuffer = nullptr;
            }

            m_InstanceBufferSize = totalItems + 100;

            D3D11_BUFFER_DESC desc =
            {
                .ByteWidth = (uint32)m_InstanceBufferSize * (uint32)sizeof(RenderItem),
                .Usage = D3D11_USAGE_DYNAMIC,
                .BindFlags = D3D11_BIND_VERTEX_BUFFER,
                .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
            };

            HRESULT res = m_Device->CreateBuffer(&desc, nullptr, &m_InstanceBuffer);
            HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to create instance buffer: %08X", res);

            #ifdef _DEBUG
            StringBuilder<> sb;
            sb.AppendF("Instance buffer resized to %zu\n", m_InstanceBufferSize);
            ::OutputDebugStringA(sb.CStr());
            #endif
        }

        D3D11_MAPPED_SUBRESOURCE resource;
        HRESULT res = m_DeviceContext->Map(m_InstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
        HAX_PANIC(res == S_OK, g_Context->Logger, L"Unable to map instance buffer: %08X", res);

        size_t offset = 0;
        for (Layer* layer : layers)
        {
            memcpy((uint8*)resource.pData + offset, layer->RenderItems.Data(), layer->RenderItems.Size() * sizeof(RenderItem));
            offset += layer->RenderItems.Size() * sizeof(RenderItem);
        }
        m_DeviceContext->Unmap(m_InstanceBuffer, 0);
    }

    void DirectX11Backend::RenderLayers(Vector<Layer*>& layers)
    {
        size_t instanceOffset = 0;
        for (Layer* layer : layers)
        {
            for (RenderBatch& batch : layer->RenderBatches)
            {
                if (batch.ActionMask & RenderBatchAction_SetClipRect)
                {
                    Rect clipRect = batch.ClipRect.Value();
                    RECT rect
                    {
                        .left =   (LONG)clipRect.Min.X,
                        .top =    (LONG)clipRect.Min.Y,
                        .right =  (LONG)clipRect.Max.X,
                        .bottom = (LONG)clipRect.Max.Y
                    };
                    m_DeviceContext->RSSetScissorRects(1, &rect);
                }

                if (batch.ActionMask & RenderBatchAction_SetTexture)
                {
                    m_DeviceContext->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&batch.Texture);
                }

                m_DeviceContext->DrawInstanced(6, (uint32)batch.InstancesNum, 0, (uint32)instanceOffset);
                instanceOffset += batch.InstancesNum;
            }
        }
    }

    extern void Initialize(Handle hwnd);

    void Initialize(Handle hwnd, ID3D11Device* device)
    {
        HAX_ASSERT(g_Context == nullptr && "Context already initialized");

        g_Context = New<Context>();
        Initialize(hwnd);

        DirectX11Backend* graphicsApi = New<DirectX11Backend>(g_Context->GeneralAlloc, *g_Context, device);
        g_Context->Backend = graphicsApi;
        g_Context->Bitmap.Texture = g_Context->Backend->CreateTexture(TextureFormat::R8_UNorm, g_Context->Bitmap.Pixels, kBitmapSize, kBitmapSize, 1);
    }

    const char* g_VertexShaderText = R"(
cbuffer vertexBuffer : register(b0)
{
    float4x4 ProjectionMatrix;
};

struct VS_INPUT      
{
    float4 params14 : PARAMS0;
    float4 params58 : PARAMS1;
    float4 params912 : PARAMS2;
    float param13 : PARAM0;
    float param14 : PARAM1;

    float4 color1 : COLOR0;
    float4 color2 : COLOR1;

    float2 sincos: SINCOS0;
    uint type : TYPE0;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;

    float4 params14 : PARAMS0;
    float4 params58 : PARAMS1;
    float4 params912 : PARAMS2;
    float param13 : PARAM0;
    float param14 : PARAM1;

    float4 color1 : COLOR0;
    float4 color2 : COLOR1;

    float2 pixPos : TEXCOORD0;
    float2 sincos: SINCOS0;
    uint type : TYPE0;
};

float2 rotate(float2 p, float2 c, float2 sc)
{
    float2 d = p - c;
    return c + float2(
        d.x * sc.y + d.y * sc.x,
       -d.x * sc.x + d.y * sc.y
    );
}

float2 calcEllipse(VS_INPUT input, uint vertId)
{
    float2 center = input.params14.xy;
    float2 r = input.params14.zw + 1.0;

    float2 tl = rotate(center - r, center, input.sincos);
    float2 tr = rotate(float2(center.x + r.x, center.y - r.y), center, input.sincos);
    float2 br = rotate(center + r, center, input.sincos);
    float2 bl = rotate(float2(center.x - r.x, center.y + r.y), center, input.sincos);

    float2 rotated[6] = { tl, tr, bl, bl, tr, br };
    return rotated[vertId];
}

float2 calcRect(VS_INPUT input, uint vertId)
{
    float2 tl = input.params14.xy - 1.0;
    float2 br = input.params14.zw + 1.0;

    float2 center = (tl + br) * 0.5;
    float2 tr = rotate(float2(br.x, tl.y), center, input.sincos);
    float2 bl = rotate(float2(tl.x, br.y), center, input.sincos);
    tl = rotate(tl, center, input.sincos);
    br = rotate(br, center, input.sincos);

    float2 rotated[6] = { tl, tr, bl, bl, tr, br };
    return rotated[vertId];
}

float2 calcGlyphRect(VS_INPUT input, uint vertId)
{
    float2 tl = input.params14.xy;
    float2 br = input.params14.zw;

    float2 center = (tl + br) * 0.5;
    float2 tr = rotate(float2(br.x, tl.y), center, input.sincos);
    float2 bl = rotate(float2(tl.x, br.y), center, input.sincos);
    tl = rotate(tl, center, input.sincos);
    br = rotate(br, center, input.sincos);

    float2 rotated[6] = { tl, tr, bl, bl, tr, br };
    return rotated[vertId];
}

float2 calcTriangle(VS_INPUT input, uint vertId)
{
    float2 a = input.params14.xy;
    float2 b = input.params14.zw;
    float2 c = input.params58.xy;
    float th = input.params58.z; 
    float padding = th + 2.0;

    float2 center = (a + b + c) / 3.0;
    a = rotate(a, center, input.sincos);
    b = rotate(b, center, input.sincos);
    c = rotate(c, center, input.sincos);

    float2 p[3] = { a, b, c };
    float2 current = p[vertId];
    
    float2 prev = p[(vertId + 2) % 3];
    float2 next = p[(vertId + 1) % 3];
    
    float2 dir1 = normalize(current - prev);
    float2 dir2 = normalize(current - next);
    
    float2 outDir = normalize(dir1 + dir2);
    
    float cosA = dot(dir1, dir2);
    float sinA = sqrt(max(0.0, 1.0 - cosA * cosA));
    float expandDist = padding / max(0.1, sinA); 

    return current + outDir * expandDist;
}

float2 calcLine(VS_INPUT input, uint vertId)
{
    float2 a = input.params14.xy;
    float2 b = input.params14.zw;
    float th = input.params58.x;

    float2 center = (a + b) / 2.0;
    a = rotate(a, center, input.sincos);
    b = rotate(b, center, input.sincos);

    float2 dir = normalize(b - a);
    float2 normal = float2(-dir.y, dir.x) * ((th + 1.0) / 2.0);

    float2 tl = a - normal;
    float2 bl = a + normal;
    float2 tr = b - normal;
    float2 br = b + normal;

    float2 rotated[6] = { tl, tr, bl, bl, tr, br };
    return rotated[vertId];
}

VS_OUTPUT main(VS_INPUT input, uint vertId : SV_VertexID)
{
    input.sincos.x = -input.sincos.x;

    VS_OUTPUT output;

    switch (input.type)
    {
        case 0: output.pos = float4(0.0f, 0.0f, 0.0f, 1.0f); break;
        case 1: output.pixPos = calcEllipse(input, vertId); break;
        case 7:
        {
            float2 vert_to_uv[6] = { {0,0}, {1,0}, {0,1}, {0,1}, {1,0}, {1,1} };
    
            float2 uvMin = input.params58.xy;
            float2 uvMax = input.params58.zw;
    
            input.params58.xy = lerp(uvMin, uvMax, vert_to_uv[vertId]);
            output.pixPos = calcGlyphRect(input, vertId);
            break;
        }
        case 6:
        {
            float2 vert_to_uv[6] = { {0,0}, {1,0}, {0,1}, {0,1}, {1,0}, {1,1} };
    
            float2 uvMin = input.params58.xy;
            float2 uvMax = input.params58.zw;
    
            input.params58.xy = lerp(uvMin, uvMax, vert_to_uv[vertId]);
            input.params14.xy += 2.0;
            input.params14.zw -= 2.0;
            output.pixPos = calcRect(input, vertId);
            break;
        }
        case 5:
        {
            float2 vert_to_uv[6] = { {0,0}, {1,0}, {0,1}, {0,1}, {1,0}, {1,1} };
    
            float2 uvMin = input.params58.xy;
            float2 uvMax = input.params58.zw;
    
            input.params58.xy = lerp(uvMin, uvMax, vert_to_uv[vertId]);
            input.params14.xy += 2.0;
            input.params14.zw -= 2.0;
            output.pixPos = calcRect(input, vertId);
            break;
        }
        case 2: 
            float2 a = input.params14.xy; float2 b = input.params14.zw;
            input.params14.xy = min(a,b); input.params14.zw = max(a,b);
            output.pixPos = calcRect(input, vertId); break;
        case 3: output.pixPos = calcTriangle(input, vertId); break;
        case 4: output.pixPos = calcLine(input, vertId); break;
    }

    output.params14 = input.params14;
    output.params58 = input.params58;
    output.params912 = input.params912;
    output.param13 = input.param13;
    output.param14 = input.param14;
    output.color1 = input.color1;
    output.color2 = input.color2;
    output.sincos = input.sincos;
    output.type = input.type;
    output.pos = mul(ProjectionMatrix, float4(output.pixPos, 0.f, 1.f));

    return output;
}
)";

    const char* g_PixelShaderText = R"(
struct PS_INPUT
{
    float4 pos : SV_POSITION;

    float4 params14 : PARAMS0;
    float4 params58 : PARAMS1;
    float4 params912 : PARAMS2;
    float param13 : PARAM0;
    float param14 : PARAM1;

    float4 color1 : COLOR0;
    float4 color2 : COLOR1;

    float2 pixPos : TEXCOORD0;
    float2 sincos: SINCOS;
    uint type : TYPE0;
};

float2 fastMap(float2 p, float2 center, float2 sincos) 
{
    float2 d = p - center;
    return center + float2(
        d.x * sincos.y + d.y * sincos.x,
       -d.x * sincos.x + d.y * sincos.y
    );
}

float sdEllipse(float2 p, float2 center, float2 r)
{
    float2 q = p - center;
    
    q += 1e-6f; 

    float k1 = length(q / r);
    float k2 = length(q / (r * r));
    return (k1 - 1.0) * k1 / k2;
}

float4 shaderEllipse(PS_INPUT input)
{
    float2 p      = input.pixPos;
    float2 center = input.params14.xy;
    float2 r      = input.params14.zw;
    float  th     = input.params58.x;

    float2 pRot = fastMap(p, center, input.sincos);

    float d  = sdEllipse(pRot, center, r);
    float aa = fwidth(p) * 0.7;

    float s_d = abs(d + th * 0.5f) - th * 0.5f;
    float s_mask = smoothstep(aa, -aa, s_d) * step(0.0001f, th);
    
    float f_mask = smoothstep(aa, -aa, d);
    f_mask = saturate(f_mask - s_mask);

    float4 fill = float4(input.color1.rgb * input.color1.a * f_mask, input.color1.a * f_mask);
    float4 stroke = float4(input.color2.rgb * input.color2.a * s_mask, input.color2.a * s_mask);

    return stroke + fill;
}

float sdRect(float2 p, float2 a, float2 b, float4 r)
{
    float2 halfSize = (b - a) * 0.5;
    float2 center = a + halfSize;
    p -= center;

    float2 s = step(0.0, p);
    float rad = lerp(lerp(r.x, r.y, s.x), lerp(r.w, r.z, s.x), s.y);

    float2 d = abs(p) - halfSize + rad;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rad;
}

float4 shaderRect(PS_INPUT input)
{
    float2 p = input.pixPos;
    float2 a = input.params14.xy;
    float2 b = input.params14.zw;
    float4 r = input.params58;
    float4 th = input.params912.x;

    float2 halfSize = abs(b - a) * 0.5;
    float2 center   = a + halfSize;
    float2 pRot     = fastMap(p, center, input.sincos);

    float d  = sdRect(pRot, a, b, r);
    float aa = fwidth(p) * 0.5;

    float s_d = abs(d + th * 0.5) - th * 0.5;
    float s_mask = smoothstep(aa, -aa, s_d) * step(0.0001, th);
    
    float f_mask = smoothstep(aa, -aa, d);
    f_mask = saturate(f_mask - s_mask); 

    float4 fill = float4(input.color1.rgb * input.color1.a * f_mask, input.color1.a * f_mask);
    float4 stroke = float4(input.color2.rgb * input.color2.a * s_mask, input.color2.a * s_mask);

    return stroke + fill;
}

float sdTriangle(float2 p, float2 a, float2 b, float2 c)
{
    float2 e0 = b-a, e1 = c-b, e2 = a-c;
    float2 v0 = p -a, v1 = p -b, v2 = p -c;
    float2 pq0 = v0 - e0*clamp( dot(v0,e0)/dot(e0,e0), 0.0, 1.0 );
    float2 pq1 = v1 - e1*clamp( dot(v1,e1)/dot(e1,e1), 0.0, 1.0 );
    float2 pq2 = v2 - e2*clamp( dot(v2,e2)/dot(e2,e2), 0.0, 1.0 );
    float s = sign( e0.x*e2.y - e0.y*e2.x );
    float2 d = min(min(float2(dot(pq0,pq0), s*(v0.x*e0.y-v0.y*e0.x)),
                     float2(dot(pq1,pq1), s*(v1.x*e1.y-v1.y*e1.x))),
                     float2(dot(pq2,pq2), s*(v2.x*e2.y-v2.y*e2.x)));
    return -sqrt(d.x)*sign(d.y);
}

float4 shaderTriangle(PS_INPUT input)
{
    float2 p  = input.pixPos;
    float2 a  = input.params14.xy;
    float2 b  = input.params14.zw;
    float2 c  = input.params58.xy;
    float th  = input.params58.z;

    float2 center = (a + b + c) / 3.0f;
    float2 pRot   = fastMap(p, center, input.sincos);

    float d  = sdTriangle(pRot, a, b, c);
    float aa = fwidth(p) * 0.5;

    float s_d = abs(d + th * 0.5f) - th * 0.5f;
    float s_mask = smoothstep(aa, -aa, s_d) * step(0.0001f, th);
    
    float f_mask = smoothstep(aa, -aa, d);
    f_mask = saturate(f_mask - s_mask);

    float4 fill = float4(input.color1.rgb * input.color1.a * f_mask, input.color1.a * f_mask);
    float4 stroke = float4(input.color2.rgb * input.color2.a * s_mask, input.color2.a * s_mask);

    return stroke + fill;
}

float sdSegment(float2 p, float2 a, float2 b)
{
    float2 pa = p - a;
    float2 ba = b - a;
    float h = saturate(dot(pa, ba) / dot(ba, ba));
    return length(pa - ba * h);
}

float4 shaderLine(PS_INPUT input)
{
    float2 p = input.pixPos;

    float2 a  = input.params14.xy;
    float2 b  = input.params14.zw;
    float  th = input.params58.x;

    float4 fillColor = input.color1;

    float2 center = (a + b) * 0.5;
    float2 pRot   = fastMap(p, center, input.sincos);

    float d = sdSegment(pRot, a, b) - th * 0.5;

    float aa = fwidth(p) * 0.5;

    float coverage = smoothstep(aa, -aa, d);

    float4 finalColor = fillColor;
    finalColor.a *= coverage;
    finalColor.rgb *= finalColor.a;

    return finalColor;
}

sampler sampler0 : register(s0);
Texture2D texture0 : register(t0);
Texture2DArray fontTexture : register(t1);
Texture2D atlasTexture : register(t2);
float4 shaderImage(PS_INPUT input)
{
    float2 p = input.pixPos;
    float2 a = input.params14.xy - float2(1, 1);
    float2 b = input.params14.zw + float2(1, 1);

    float r = input.params912.x;
    float4 r4 = float4(r, r, r, r);

    float4 src = texture0.Sample(sampler0, input.params58.xy);
    float2 halfSize = abs(b - a) * 0.5;
    float2 center = a + halfSize;
    float2 pRot = fastMap(p, center, input.sincos);
    
    float d = sdRect(pRot, a, b, r);
    float aa = fwidth(p) * 0.5;

    float mask = smoothstep(aa, -aa, d);
    
    float4 finalColor = src * input.color1;
    finalColor.a *= mask;
    finalColor.rgb *= finalColor.a;

    return finalColor;
}

float4 shaderMsdfGlyph(PS_INPUT input)
{
    float3 uv = float3(input.params58.xy, input.params912.x);
    float pxRange = input.params912.y;
    float2 texSize = input.params912.zw;
    float weight = input.param13;

    float outlineFactor = 0.2 * step(0.01, input.param14); 
    float4 strokeColor = input.color2;

    float3 msd = fontTexture.Sample(sampler0, uv).rgb; 
    float d_raw = max(min(msd.r, msd.g), min(max(msd.r, msd.g), msd.b));
    float d = d_raw + weight;

    float2 dest = 1.0 / fwidth(uv.xy); 
    float px_size = max(0.5 * dot((pxRange / texSize), dest), 1.0);
    
    float f_mask = saturate((d - 0.5) * px_size + 0.5);
    float s_mask = saturate((d - (0.5 - outlineFactor)) * px_size + 0.5);
    
    s_mask *= step(0.01, d_raw); 
    
    float o_mask = saturate(s_mask - f_mask);

    float4 fill = float4(input.color1.rgb * input.color1.a * f_mask, input.color1.a * f_mask);
    float4 stroke = float4(strokeColor.rgb * strokeColor.a * o_mask, strokeColor.a * o_mask);

    return fill + stroke;
}

float4 shaderGlyph(PS_INPUT input) 
{
    float2 uv = input.params58.xy;
    float a = atlasTexture.Sample(sampler0, uv).r;

    float4 col = input.color1;
    col.a *= a;
    col.rgb *= col.a;
    return col;
}

float4 main(PS_INPUT input) : SV_Target
{    
    input.sincos.x = -input.sincos.x;
    switch (input.type)
    {
        case 1: return shaderEllipse(input);
        case 2: return shaderRect(input);
        case 3: return shaderTriangle(input);
        case 4: return shaderLine(input);
        case 5: return shaderImage(input);
        case 6: return shaderMsdfGlyph(input);
        case 7: return shaderGlyph(input);
    }
    return float4(0,0,0,0);
}
)";
}