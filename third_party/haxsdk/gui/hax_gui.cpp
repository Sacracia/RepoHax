
#define HAXGUI_INCLUDE_INTERNAL
#include "hax_gui.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <wincodec.h>
#include <dwrite_3.h>
#include <wrl/client.h>
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

using Microsoft::WRL::ComPtr;

namespace Hax::Gui
{
    Allocator g_StateAlloc;
}

namespace Hax::Gui
{
    HotData G;

    struct ContainerState
    {
        Rect Bounds;
        struct
        {
            Vector2     Offset = { -1.f, -1.f };
            float       TargetOffsetY = -1.f;
            float       LastTimeHeldX, LastTimeHeldY;
            ScrollStyle Style;
        } Scroll;
    };

    struct MsdfGlyph
    {
        uint32 Codepoint;
        uint32 Image;
        struct {float L, B, R, T;} PlaneBounds, ImageBounds;
        Vector2 Advance;
    };

    struct MsdfFont
    {
        HashMap<uint32, MsdfGlyph*> Glyphs;

        float                   Size;
        float                   DistanceRange;
        float                   EmSize;
        float                   Ascender;
        float                   Descender;
        float                   LineHeight;

        struct
        {
            Span<const uint8>   Data;
            float               IndexInArray;
            uint32              Width;
            uint32              Height;
        } Atlas;
    };

    struct Glyph
    {
        bool Baked;

        float OffX, OffY;
        int W, H;
        Vector2 UV0, UV1;
        float Advance;
    };

    class WoffLoader;
    struct Font
    {
        IDWriteFontFace* FontFace;
        WoffLoader* WoffLoader;
        HashMap<Pair<char16, uint32>, Glyph> Glyphs;
        HashMap<Pair<char16, char16>, float> Kernings;

        float Ascent, Descent, LineHeight, CapHeight;
        float DesignUnitsPerEm;
    };

    struct MsdfFontDecoder
    {
        MsdfFontDecoder(Span<const uint8> data) : m_Binary(data.Data()), m_BinarySize(data.Size()), m_ReadCursor(data.Data()) {}

        MsdfFont* Decode();

        void ReadBytes(uint8* target, size_t nBytes);
        void ReadString(String* str, size_t nBytes);
        void AlignCursor();

    private:
        const uint8* m_Binary;
        uint64 m_BinarySize;

        const uint8* m_ReadCursor;
        uint32 m_Checksum = ~0u;
    };

    static uint32 Crc32Update(uint32 crc, uint8 x)
    {
        static const uint32 s_Crc32Table[256] =
        {
            0x00000000u, 0x77073096u, 0xee0e612cu, 0x990951bau, 0x076dc419u, 0x706af48fu, 0xe963a535u, 0x9e6495a3u,
            0x0edb8832u, 0x79dcb8a4u, 0xe0d5e91eu, 0x97d2d988u, 0x09b64c2bu, 0x7eb17cbdu, 0xe7b82d07u, 0x90bf1d91u,
            0x1db71064u, 0x6ab020f2u, 0xf3b97148u, 0x84be41deu, 0x1adad47du, 0x6ddde4ebu, 0xf4d4b551u, 0x83d385c7u,
            0x136c9856u, 0x646ba8c0u, 0xfd62f97au, 0x8a65c9ecu, 0x14015c4fu, 0x63066cd9u, 0xfa0f3d63u, 0x8d080df5u,
            0x3b6e20c8u, 0x4c69105eu, 0xd56041e4u, 0xa2677172u, 0x3c03e4d1u, 0x4b04d447u, 0xd20d85fdu, 0xa50ab56bu,
            0x35b5a8fau, 0x42b2986cu, 0xdbbbc9d6u, 0xacbcf940u, 0x32d86ce3u, 0x45df5c75u, 0xdcd60dcfu, 0xabd13d59u,
            0x26d930acu, 0x51de003au, 0xc8d75180u, 0xbfd06116u, 0x21b4f4b5u, 0x56b3c423u, 0xcfba9599u, 0xb8bda50fu,
            0x2802b89eu, 0x5f058808u, 0xc60cd9b2u, 0xb10be924u, 0x2f6f7c87u, 0x58684c11u, 0xc1611dabu, 0xb6662d3du,
            0x76dc4190u, 0x01db7106u, 0x98d220bcu, 0xefd5102au, 0x71b18589u, 0x06b6b51fu, 0x9fbfe4a5u, 0xe8b8d433u,
            0x7807c9a2u, 0x0f00f934u, 0x9609a88eu, 0xe10e9818u, 0x7f6a0dbbu, 0x086d3d2du, 0x91646c97u, 0xe6635c01u,
            0x6b6b51f4u, 0x1c6c6162u, 0x856530d8u, 0xf262004eu, 0x6c0695edu, 0x1b01a57bu, 0x8208f4c1u, 0xf50fc457u,
            0x65b0d9c6u, 0x12b7e950u, 0x8bbeb8eau, 0xfcb9887cu, 0x62dd1ddfu, 0x15da2d49u, 0x8cd37cf3u, 0xfbd44c65u,
            0x4db26158u, 0x3ab551ceu, 0xa3bc0074u, 0xd4bb30e2u, 0x4adfa541u, 0x3dd895d7u, 0xa4d1c46du, 0xd3d6f4fbu,
            0x4369e96au, 0x346ed9fcu, 0xad678846u, 0xda60b8d0u, 0x44042d73u, 0x33031de5u, 0xaa0a4c5fu, 0xdd0d7cc9u,
            0x5005713cu, 0x270241aau, 0xbe0b1010u, 0xc90c2086u, 0x5768b525u, 0x206f85b3u, 0xb966d409u, 0xce61e49fu,
            0x5edef90eu, 0x29d9c998u, 0xb0d09822u, 0xc7d7a8b4u, 0x59b33d17u, 0x2eb40d81u, 0xb7bd5c3bu, 0xc0ba6cadu,
            0xedb88320u, 0x9abfb3b6u, 0x03b6e20cu, 0x74b1d29au, 0xead54739u, 0x9dd277afu, 0x04db2615u, 0x73dc1683u,
            0xe3630b12u, 0x94643b84u, 0x0d6d6a3eu, 0x7a6a5aa8u, 0xe40ecf0bu, 0x9309ff9du, 0x0a00ae27u, 0x7d079eb1u,
            0xf00f9344u, 0x8708a3d2u, 0x1e01f268u, 0x6906c2feu, 0xf762575du, 0x806567cbu, 0x196c3671u, 0x6e6b06e7u,
            0xfed41b76u, 0x89d32be0u, 0x10da7a5au, 0x67dd4accu, 0xf9b9df6fu, 0x8ebeeff9u, 0x17b7be43u, 0x60b08ed5u,
            0xd6d6a3e8u, 0xa1d1937eu, 0x38d8c2c4u, 0x4fdff252u, 0xd1bb67f1u, 0xa6bc5767u, 0x3fb506ddu, 0x48b2364bu,
            0xd80d2bdau, 0xaf0a1b4cu, 0x36034af6u, 0x41047a60u, 0xdf60efc3u, 0xa867df55u, 0x316e8eefu, 0x4669be79u,
            0xcb61b38cu, 0xbc66831au, 0x256fd2a0u, 0x5268e236u, 0xcc0c7795u, 0xbb0b4703u, 0x220216b9u, 0x5505262fu,
            0xc5ba3bbeu, 0xb2bd0b28u, 0x2bb45a92u, 0x5cb36a04u, 0xc2d7ffa7u, 0xb5d0cf31u, 0x2cd99e8bu, 0x5bdeae1du,
            0x9b64c2b0u, 0xec63f226u, 0x756aa39cu, 0x026d930au, 0x9c0906a9u, 0xeb0e363fu, 0x72076785u, 0x05005713u,
            0x95bf4a82u, 0xe2b87a14u, 0x7bb12baeu, 0x0cb61b38u, 0x92d28e9bu, 0xe5d5be0du, 0x7cdcefb7u, 0x0bdbdf21u,
            0x86d3d2d4u, 0xf1d4e242u, 0x68ddb3f8u, 0x1fda836eu, 0x81be16cdu, 0xf6b9265bu, 0x6fb077e1u, 0x18b74777u,
            0x88085ae6u, 0xff0f6a70u, 0x66063bcau, 0x11010b5cu, 0x8f659effu, 0xf862ae69u, 0x616bffd3u, 0x166ccf45u,
            0xa00ae278u, 0xd70dd2eeu, 0x4e048354u, 0x3903b3c2u, 0xa7672661u, 0xd06016f7u, 0x4969474du, 0x3e6e77dbu,
            0xaed16a4au, 0xd9d65adcu, 0x40df0b66u, 0x37d83bf0u, 0xa9bcae53u, 0xdebb9ec5u, 0x47b2cf7fu, 0x30b5ffe9u,
            0xbdbdf21cu, 0xcabac28au, 0x53b39330u, 0x24b4a3a6u, 0xbad03605u, 0xcdd70693u, 0x54de5729u, 0x23d967bfu,
            0xb3667a2eu, 0xc4614ab8u, 0x5d681b02u, 0x2a6f2b94u, 0xb40bbe37u, 0xc30c8ea1u, 0x5a05df1bu, 0x2d02ef8du,
        };
        return s_Crc32Table[uint8(x ^ crc)] ^ crc >> 8;
    }
    static Glyph& FindOrLoadGlyph(Font& font, char16 sym, float sz);
    static Glyph& FindOrLoadGlyph(Font& font, char16 sym, uint32 sz);
    static float GetKerning(Font& font, char16 sym1, char16 sym2);
    void MsdfFontDecoder::ReadBytes(uint8* target, size_t nBytes)
    {
        HAX_ASSERT(m_ReadCursor + nBytes <= m_Binary + m_BinarySize);

        if (target != nullptr)
            memcpy(target, m_ReadCursor, nBytes);

        for (uint32 i = 0; i < nBytes; ++i)
        {
            m_Checksum = Crc32Update(m_Checksum, *m_ReadCursor);
            ++m_ReadCursor;
        }
    }

    void MsdfFontDecoder::AlignCursor()
    {
        size_t nBytesRead = m_ReadCursor - m_Binary;
        if (nBytesRead % 4 > 0)
            this->ReadBytes(nullptr, 4 - nBytesRead % 4);
    }

    void MsdfFontDecoder::ReadString(String* str, size_t nChars)
    {
        if (nChars > 0)
        {
            if (str != nullptr)
            {
                str->Clear();
                str->Reserve(nChars + 1);

                this->ReadBytes((uint8*)str->Data(), nChars + 1);
                str->Last() = '\0';
            }
            else
                this->ReadBytes(nullptr, nChars + 1);

            this->AlignCursor();
        }
    }

    struct FontHeader
    {
        char Tag[16];
        uint32 MagicNo;
        uint32 Version;
        uint32 Flags;
        uint32 RealType;
        uint32 Reserved[4];

        uint32 MetadataFormat;
        uint32 MetadataLength;
        uint32 VariantCount;
        uint32 VariantsLength;
        uint32 ImageCount;
        uint32 ImagesLength;
        uint32 AppendixCount;
        uint32 AppendicesLength;
        uint32 Reserved2[8];
    };

    struct FontVariant
    {
        struct Metrics
        {
            float fontSize;
            float distanceRange;

            float emSize;
            float ascender, descender;
            float lineHeight;
            float underlineY, underlineThickness;

            float distanceRangeMiddle;
            float reserved[23];
        };

        uint32 flags;
        uint32 weight;
        uint32 codepointType;
        uint32 imageType;
        uint32 fallbackVariant;
        uint32 fallbackGlyph;
        uint32 reserved[6];
        Metrics metrics;
        uint32 nameLength;
        uint32 metadataLength;
        uint32 glyphCount;
        uint32 kernPairCount;
    };

    enum Encoding
    {
        IMAGE_UNKNOWN_ENCODING = 0,
        IMAGE_RAW_BINARY = 1,
        IMAGE_BMP = 4,
        IMAGE_TIFF = 5,
        IMAGE_PNG = 8,
        IMAGE_TGA = 9
    };

    enum Orientation
    {
        ORIENTATION_TOP_DOWN = 1,
        ORIENTATION_BOTTOM_UP = -1
    };

    enum PixelFormat
    {
        PIXEL_UNKNOWN = 0,
        PIXEL_BOOLEAN1 = 1,
        PIXEL_UNSIGNED8 = 8,
        PIXEL_FLOAT32 = 32
    };

    enum Type
    {
        IMAGE_NONE = 0,
        IMAGE_SRGB_IMAGE = 1,
        IMAGE_LINEAR_MASK = 2,
        IMAGE_MASKED_SRGB_IMAGE = 3,
        IMAGE_SDF = 4,
        IMAGE_PSDF = 5,
        IMAGE_MSDF = 6,
        IMAGE_MTSDF = 7,
        IMAGE_MIXED_CONTENT = 255
    };

    struct ImageHeader
    {
        enum Type
        {
            IMAGE_NONE = 0,
            IMAGE_SRGB_IMAGE = 1,
            IMAGE_LINEAR_MASK = 2,
            IMAGE_MASKED_SRGB_IMAGE = 3,
            IMAGE_SDF = 4,
            IMAGE_PSDF = 5,
            IMAGE_MSDF = 6,
            IMAGE_MTSDF = 7,
            IMAGE_MIXED_CONTENT = 255
        };

        uint32 flags;
        uint32 encoding;
        uint32 width, height;
        uint32 channels;
        uint32 pixelFormat;
        uint32 imageType;
        uint32 rowLength;
        int orientation;
        uint32 childImages;
        uint32 textureFlags;
        uint32 reserved[3];
        uint32 metadataLength;
        uint32 dataLength;
    };

    struct AppendixHeader
    {
        uint32 metadataLength;
        uint32 dataLength;
    };

    struct FontFooter
    {
        uint32 salt;
        uint32 magicNo;
        uint32 reserved[4];
        uint32 totalLength;
        uint32 checksum;
    };

    enum CodepointType
    {
        CP_UNSPECIFIED = 0,
        CP_UNICODE = 1,
        CP_INDEXED = 2,
        CP_ICONOGRAPHIC = 14
    };

    struct MsdfKernPair
    {
        CodepointType Codepoint1, Codepoint2;
        struct { float h, v; } Advance;
    };

    MsdfFont* MsdfFontDecoder::Decode()
    {
        Context& ctx = *g_Context;
        MsdfFont* font = New<MsdfFont>(ctx.FontAlloc);

        uint32 variantLen = 0;
        uint32 imageLen = 0;
        uint32 appendixLen = 0;

        // Read header
        {
            FontHeader header{};

            this->ReadBytes((uint8*)&header, sizeof(header));

            HAX_ASSERT(memcmp(header.Tag, "ARTERY/FONT\0\0\0\0\0", sizeof(header.Tag)) == 0);
            HAX_ASSERT(header.MagicNo == 0x4d276a5cu);
            HAX_ASSERT(header.RealType == 0x14u);

            this->ReadString(nullptr, header.MetadataLength);

            HAX_ASSERT(header.VariantCount == 1);
            HAX_ASSERT(header.ImageCount == 1);
            HAX_ASSERT(header.AppendixCount == 0);

            variantLen = header.VariantsLength;
            imageLen = header.ImagesLength;
            appendixLen = header.AppendicesLength;
        }

        const uint8* savedPtr = m_ReadCursor;

        // Read variant
        {
            FontVariant variant{};
            this->ReadBytes((uint8*)&variant, sizeof(variant));

            HAX_ASSERT(variant.codepointType == CP_UNICODE);
            HAX_ASSERT(variant.imageType == IMAGE_MSDF);
            HAX_ASSERT(variant.kernPairCount == 0);

            font->Size = variant.metrics.fontSize;
            font->DistanceRange = variant.metrics.distanceRange;
            font->EmSize = variant.metrics.emSize;
            font->Ascender = variant.metrics.ascender;
            font->Descender = variant.metrics.descender;
            font->LineHeight = variant.metrics.lineHeight;

            this->ReadString(nullptr, variant.nameLength);
            this->ReadString(nullptr, variant.metadataLength);

            for (uint32 i = 0; i < variant.glyphCount; ++i)
            {
                MsdfGlyph* glyph = (MsdfGlyph*)m_ReadCursor;
                font->Glyphs.Insert(glyph->Codepoint, glyph);

                this->ReadBytes(nullptr, sizeof(MsdfGlyph));
            }

            this->ReadBytes(nullptr, variant.kernPairCount * sizeof(MsdfKernPair));
        }

        HAX_ASSERT((m_ReadCursor - savedPtr) == variantLen);
        savedPtr = m_ReadCursor;

        // Read image
        {
            ImageHeader header{};
            this->ReadBytes((uint8*)&header, sizeof(header));

            HAX_ASSERT(header.encoding == IMAGE_PNG);
            HAX_ASSERT(header.channels == 3);
            HAX_ASSERT(header.imageType == IMAGE_MSDF);

            this->ReadString(nullptr, header.metadataLength);
            font->Atlas.Data = Span(m_ReadCursor, header.dataLength);
            font->Atlas.Height = header.height;
            font->Atlas.Width = header.width;

            this->ReadBytes(nullptr, header.dataLength);
            this->AlignCursor();
        }

        HAX_ASSERT((m_ReadCursor - savedPtr) == imageLen);
        savedPtr = m_ReadCursor;

        // Read footer
        {
            FontFooter footer{};
            this->ReadBytes((uint8*)&footer, sizeof(footer) - sizeof(footer.checksum));
            HAX_ASSERT(footer.magicNo == 0x55ccb363u);

            uint32 checksum = m_Checksum;
            this->ReadBytes((uint8*)&footer.checksum, sizeof(footer.checksum));
            HAX_ASSERT(footer.checksum == checksum);
            HAX_ASSERT((m_ReadCursor - m_Binary) == footer.totalLength);
        }

        return font;
    }

    static Layer* FindLayerById(size_t id);
    void Initialize(Handle hwnd);
    static void InitKeyNames();
    static void ProcessWndMsgs(Context& ctx);
    static void ResetLayer(Layer& layer);
    static void UpdateViewportSize(Viewport& viewport);
    static void UpdateViewportScale(Viewport& viewport);
    static void UpdateWheelScroll(size_t id, ContainerState& state, float maxHeight, float maxOffset);
    static bool UpdateMouseIcon();
    static void UpdateKeyboardInputs();
    static MsdfFont& GetMsdfFontFromHandle(MsdfFontHandle handle);
    static Vector2 AddMsdfGlyphToLayer(Layer& layer, MsdfFont& font, char16 sym, const Vector2& pos, float size, const MsdfGlyphParams& params);
    static Vector2 AddGlyphToLayer(Layer& layer, Glyph& glyph, const Vector2& pos, float size, const GlyphParams& params = {});
    static FontHandle LoadWoffFont(Span<const uint8> rawData);

    Context* g_Context;
    static Hax::String g_KeyToName[256];

    void BeginFrame()
    {
        HAX_ASSERT(g_Context != nullptr && "Context not set");

        {
            Timer& timer = g_Context->Timer;

            int64 totalTicksThisFrame = 0;
            ::QueryPerformanceCounter((LARGE_INTEGER*)&totalTicksThisFrame);

            timer.DeltaTime = (float)(totalTicksThisFrame - timer.TotalTicksLastFrame) / timer.TicksPerSecond;
            timer.Time += timer.DeltaTime;
            timer.TotalTicksLastFrame = totalTicksThisFrame;

            timer.FramerateSecPerFrameAccum += timer.DeltaTime - timer.FramerateSecPerFrame[timer.FramerateSecPerFrameIdx];
            timer.FramerateSecPerFrame[timer.FramerateSecPerFrameIdx] = timer.DeltaTime;
            timer.FramerateSecPerFrameCount = Max(timer.FramerateSecPerFrameCount, ++timer.FramerateSecPerFrameIdx);
            timer.FramerateSecPerFrameIdx %= (int)_countof(timer.FramerateSecPerFrame);
        }

        {
            Mouse& mouse = g_Context->Mouse;
            mouse.PrevLmbDown = mouse.LmbDown;
            mouse.DeltaPos = mouse.DeltaWheel = Vector2();
            mouse.PrevIcon = mouse.Icon;
            mouse.Icon = MouseIcon_Arrow;
            mouse.PrevDownDuration = mouse.DownDuration;
            if (mouse.DownDuration >= 0.f) 
                mouse.DownDuration += g_Context->Timer.DeltaTime;
        }

        {
            Viewport& viewport = g_Context->Viewport;
            UpdateViewportSize(viewport);
        }

        {
            g_Context->LayerStack.Clear();
            g_Context->CurrentLayer = g_Context->DefaultLayer;

            for (Layer* layer : g_Context->Layers)
                ResetLayer(*layer);
        }

        g_Context->KeysPressedThisFrame.Clear();
        g_Context->CharsPressedThisFrame.Clear();
        ProcessWndMsgs(*g_Context);

        {
            Interaction& inter = g_Context->Interaction;

            inter.ThisFrame.HoveredItem = inter.NextFrame.HoveredItem;
            inter.ThisFrame.HoveredContainer = inter.NextFrame.HoveredContainer;
            inter.ThisFrame.ActiveItemId = inter.NextFrame.ActiveItemId;
            inter.NextFrame.ActiveItemId = kInvalidId;
            inter.ClickedItemId = kInvalidId;

            if (g_Context->Mouse.IsLmbJustPressed())
            {
                inter.ThisFrame.ActiveItemId = inter.ThisFrame.HoveredItem.Id;
                inter.FocusedItemId = inter.ThisFrame.ActiveItemId;
            }

            if (g_Context->Mouse.IsLmbJustReleased())
            {
                if (inter.ThisFrame.ActiveItemId == inter.ThisFrame.HoveredItem.Id)
                    inter.ClickedItemId = inter.ThisFrame.ActiveItemId;
                inter.ThisFrame.ActiveItemId = kInvalidId;
            }

            inter.NextFrame.HoveredItem = {};
            inter.NextFrame.HoveredContainer = {};
        }

        UpdateKeyboardInputs();

        {
            Timer& timer = g_Context->Timer;
            G.Time = timer.Time;
            G.DeltaTime = timer.DeltaTime;
            G.Framerate = timer.FramerateSecPerFrameCount / timer.FramerateSecPerFrameAccum;

            Interaction& inter = g_Context->Interaction;
            G.ActiveItemId = inter.ThisFrame.ActiveItemId;
            G.HoveredItemId = inter.ThisFrame.HoveredItem.Id;
            G.ClickedItemId = inter.ClickedItemId;
            G.FocusedItemId = inter.FocusedItemId;

            G.MousePos = g_Context->Mouse.Pos;
            G.MouseDeltaPos = g_Context->Mouse.DeltaPos;

            G.ViewportSize = g_Context->Viewport.Size;
            G.ScaleFactor = g_Context->Viewport.ScaleFactor;
        }
    }

    void EndFrame()
    {
        for (Layer* layer : g_Context->Layers)
        {
            HAX_ASSERT(layer->ContainerStack.Empty() && "Begin / End container mismatch!");
            HAX_ASSERT(layer->ClipRectStack.Empty() && "Push / Pop cliprect mismatch!");
            HAX_ASSERT(layer->RotationStack.Empty() && "Begin / End rotation mismatch!");
            HAX_ASSERT(layer->LayoutStack.Empty() && "Begin / End layout mismatch!");
        }

        if (!g_Context->Bitmap.DirtyRect.IsInverted())
        {
            auto& bitmap = g_Context->Bitmap;
            uint8* src = bitmap.Pixels + kBitmapSize * bitmap.DirtyRect.MinY + bitmap.DirtyRect.MinX;
            g_Context->Backend->UpdateTextureRegion(bitmap.Texture, bitmap.DirtyRect, src, kBitmapSize);

            bitmap.DirtyRect.MinX =  bitmap.DirtyRect.MinY = (uint32)-1;
            bitmap.DirtyRect.MaxX =  bitmap.DirtyRect.MaxY = 0;
        }

        std::sort(g_Context->Layers.begin(), g_Context->Layers.end(), [](Layer* l1, Layer* l2) { return l1->ZOrder < l2->ZOrder; });
        g_Context->Backend->Render();
    }

    void Shutdown()
    {
        HAX_ASSERT(g_Context != nullptr);

        for (auto& p : g_Context->StatePools)                   { Delete(g_StateAlloc, p.Value); }
        for (Layer* layer : g_Context->Layers)                  { Delete(g_Context->GeneralAlloc, layer); }
        for (MsdfFont* font : g_Context->MsdfFonts)             { Delete(g_Context->FontAlloc, font); }
        for (size_t i = 0; i < g_Context->Fonts.Size(); ++i)    { UnloadFont((FontHandle)i); }
        for (size_t i = 0; i < g_Context->Textures.Size(); ++i) { UnloadImage((TextureHandle)i); }

        g_Context->Layers.ClearFree();
        g_Context->LayerStack.ClearFree();
        g_Context->Fonts.ClearFree();
        g_Context->MsdfFonts.ClearFree();
        g_Context->Textures.ClearFree();
        g_Context->StatePools.ClearFree();
        g_Context->KeysPressedThisFrame.ClearFree();
        g_Context->CharsPressedThisFrame.ClearFree();

        g_Context->Backend->DestroyTexture(g_Context->Bitmap.Texture);
        Free(g_Context->FontAlloc, g_Context->Bitmap.Pixels);

        g_Context->DWriteFactory->UnregisterFontFileLoader(g_Context->DWriteFontFileLoader);
        g_Context->DWriteFontFileLoader->Release();
        g_Context->DWriteFactory->Release();

        if (g_Context->ShouldCoUninitialize)
            CoUninitialize();

        Delete(g_Context->GeneralAlloc, g_Context->Backend);

        #ifdef _DEBUG
        StringBuilder<> sb;
        sb.AppendF("Memory leaks:\nFonts - %zu\nGeneral - %zu\nState pools - %zu\n", 
            g_Context->FontAlloc.TotalAllocated, g_Context->GeneralAlloc.TotalAllocated, g_StateAlloc.TotalAllocated);
        ::OutputDebugStringA(sb.CStr());
        #endif

        Delete(g_Context);
        g_Context = nullptr;
    }

    bool Initialized()
    {
        return g_Context != nullptr;
    }

    void SetLogFile(LogFile* logFile)
    {
        HAX_ASSERT(g_Context != nullptr);
        g_Context->Logger = logFile;
    }

    void SetMouseIcon(MouseIcon icon)
    {
        HAX_ASSERT(g_Context != nullptr);
        g_Context->Mouse.Icon = icon;
    }

    Handle GetMouseTexture()
    {
        HAX_ASSERT(g_Context != nullptr);
        MouseIcon icon = g_Context->Mouse.PrevIcon;

        if (icon == MouseIcon_Default)
            return (Handle)::GetCursor();

        if (icon == MouseIcon_None)
            return 0;

        HCURSOR hCustomCursor = (HCURSOR)g_Context->Mouse.Textures[icon];
        if (hCustomCursor != nullptr)
            return (Handle)hCustomCursor;

        LPTSTR winIcon = IDC_ARROW;
        switch (icon)
        {
            case MouseIcon_Arrow:        winIcon = IDC_ARROW; break;
            case MouseIcon_TextInput:    winIcon = IDC_IBEAM; break;
            case MouseIcon_ResizeAll:    winIcon = IDC_SIZEALL; break;
            case MouseIcon_ResizeEW:     winIcon = IDC_SIZEWE; break;
            case MouseIcon_ResizeNS:     winIcon = IDC_SIZENS; break;
            case MouseIcon_ResizeNESW:   winIcon = IDC_SIZENESW; break;
            case MouseIcon_ResizeNWSE:   winIcon = IDC_SIZENWSE; break;
            case MouseIcon_Hand:         winIcon = IDC_HAND; break;
            case MouseIcon_Wait:         winIcon = IDC_WAIT; break;
            case MouseIcon_Progress:     winIcon = IDC_APPSTARTING; break;
            case MouseIcon_NotAllowed:   winIcon = IDC_NO; break;
        }
        return (Handle)::LoadCursorW(nullptr, winIcon);
    }

    bool IsLmbReleased()
    {
        HAX_ASSERT(g_Context != nullptr);
        return !g_Context->Mouse.LmbDown;
    }

    bool IsLmbJustReleased()
    {
        HAX_ASSERT(g_Context != nullptr);
        return g_Context->Mouse.IsLmbJustReleased();
    }

    bool IsLmbJustPressed()
    {
        HAX_ASSERT(g_Context != nullptr);
        return g_Context->Mouse.IsLmbJustPressed();
    }

    bool IsLmbPressed()
    {
        HAX_ASSERT(g_Context != nullptr);
        return g_Context->Mouse.LmbDown;
    }

    void SetMouseTexture(MouseIcon icon, Handle tex)
    {
        HAX_ASSERT(g_Context != nullptr);
        g_Context->Mouse.Textures[icon] = tex;
    }

    void SetMouseTextureFromRes(MouseIcon icon, Handle hModule, int res)
    {
        HAX_ASSERT(g_Context != nullptr);
        g_Context->Mouse.Textures[icon] = (Handle)::LoadCursor((HMODULE)hModule, MAKEINTRESOURCE(res));
    }

    //

    long HandleWndMsg(void* hwnd, uint32 msg, uintptr wParam, uint64 lParam)
    {
        if (g_Context == nullptr)
            return 0;

        /*if (msg == WM_SETCURSOR)
        {
            if (LOWORD(lParam) == HTCLIENT)
                return 1;
            return 0;
        }*/

        WndMsg newMsg
        {
            .Hwnd = (HWND)hwnd,
            .Msg = msg,
            .WParam = wParam,
            .LParam = lParam
        };
        
        switch (msg)
        {
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK:
            case WM_LBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_CHAR:
                g_Context->WndMsgQueue.Write(newMsg);
        }

        return 0;
    }

    // Layer

    void CreateLayer(WStringView name, int zOrder)
    {
        HAX_ASSERT(g_Context != nullptr);

        size_t id = Hash(name);

        for (Layer* layer : g_Context->Layers)
        {
            HAX_ASSERT(layer->Id != id && "Layer name already occupied");
            HAX_ASSERT(layer->ZOrder != zOrder && "Layer zorder already occupied");
        }

        Layer* newLayer = New<Layer>(g_Context->GeneralAlloc);
        newLayer->Id = id;
        newLayer->ZOrder = zOrder;
        ResetLayer(*newLayer);

        g_Context->Layers.PushBack(newLayer);

        if (g_Context->CurrentLayer == nullptr)
            g_Context->CurrentLayer = newLayer;
    }

    void SwitchLayer(WStringView name)
    {
        HAX_ASSERT(g_Context != nullptr);

        size_t id = Hash(name);
        Layer* layer = FindLayerById(id);

        HAX_ASSERT(layer != nullptr && "Layer not found");

        g_Context->LayerStack.PushBack(g_Context->CurrentLayer);
        g_Context->CurrentLayer = layer;
    }

    void RestoreLayer()
    {
        HAX_ASSERT(!g_Context->LayerStack.Empty() && "Layer stack empty");

        g_Context->CurrentLayer = g_Context->LayerStack.Last();
        g_Context->LayerStack.PopBack();
    }

    static Layer& GetCurrentLayer()
    {
        HAX_ASSERT(g_Context != nullptr);
        return *g_Context->CurrentLayer;
    }

    static void AddRenderItemToLayer(Layer& layer, RenderItem& item, const RenderItemRes& resources = {})
    {
        layer.RenderItems.PushBack(item);

        RenderBatch newBatch{};

        if (!layer.RenderBatches.Empty())
        {
            const RenderBatch& prevBatch = layer.RenderBatches.Last();

            newBatch = prevBatch;
            newBatch.ActionMask = RenderBatchAction_None;
            newBatch.InstancesNum = 0;
        }

        if (resources.Image != nullptr && resources.Image != newBatch.Texture)
        {
            newBatch.Texture = resources.Image;
            newBatch.ActionMask |= RenderBatchAction_SetTexture;
        }

        if (newBatch.ClipRect != layer.CurrentClipRect)
        {
            newBatch.ClipRect = layer.CurrentClipRect;
            newBatch.ActionMask |= RenderBatchAction_SetClipRect;
        }

        if (newBatch.ActionMask != RenderBatchAction_None)
            layer.RenderBatches.PushBack(newBatch);

        layer.RenderBatches.Last().InstancesNum++;
    }

    void DrawRect(const Vector2& a, const Vector2& b, const RectParams& params)
    {
        Layer& currentLayer = GetCurrentLayer();

        const bool colorsTransparent = params.FillColor.IsTransparent() && params.BorderColor.IsTransparent();
        if (colorsTransparent || IsDrawingSkipped())
            return;

        Vector4 r = params.Rounding; 
        Vector2 size = { b.X - a.X, b.Y - a.Y };

        float scale = 1.0f;
        auto checkSide = [&](float r1, float r2, float sideLen) { if (sideLen > 0.001f && (r1 + r2) > sideLen) scale = Min(scale, sideLen / (r1 + r2)); };
        checkSide(r.X, r.Y, size.X);
        checkSide(r.W, r.Z, size.X);
        checkSide(r.X, r.W, size.Y);
        checkSide(r.Y, r.Z, size.Y);
        if (scale < 1.0f)
            r.X *= scale; r.Y *= scale; r.Z *= scale; r.W *= scale;

        RenderItem item{};
        item.Color1 = params.FillColor;
        item.Color2 = params.BorderColor;
        item.Type = RenderItemType::Rect;
        item.Sin = currentLayer.CurrentRotation.Sin;
        item.Cos = currentLayer.CurrentRotation.Cos;
        item.Rect.Min = a;
        item.Rect.Max = b;
        item.Rect.R = r;
        item.Rect.BorderTh = params.BorderTh;

        AddRenderItemToLayer(currentLayer, item);
    }

    void DrawEllipse(const Vector2& c, const Vector2& r, const EllipseParams& params)
    {
        Layer& currentLayer = GetCurrentLayer();

        const bool colorsTransparent = params.FillColor.IsTransparent() && params.BorderColor.IsTransparent();
        if (colorsTransparent || IsDrawingSkipped())
            return;

        RenderItem item{};
        item.Type = RenderItemType::Ellipse;
        item.Color1 = params.FillColor;
        item.Color2 = params.BorderColor;
        item.Sin = currentLayer.CurrentRotation.Sin;
        item.Cos = currentLayer.CurrentRotation.Cos;
        item.Ellipse.Center = c;
        item.Ellipse.R = r;
        item.Ellipse.BorderTh = params.BorderTh;

        AddRenderItemToLayer(currentLayer, item);
    }

    void DrawCircle(const Vector2& c, float r, const CircleParams& params)
    {
        DrawEllipse(c, Vector2(r, r), params);
    }

    void DrawTriangle(const Vector2& a, const Vector2& b, const Vector2& c, const TriangleParams& params)
    {
        Layer& currentLayer = GetCurrentLayer();

        const bool colorsTransparent = params.FillColor.IsTransparent() && params.BorderColor.IsTransparent();
        if (colorsTransparent || IsDrawingSkipped())
            return;

        RenderItem item{};
        item.Type = RenderItemType::Triangle;
        item.Color1 = params.FillColor;
        item.Color2 = params.BorderColor;
        item.Sin = currentLayer.CurrentRotation.Sin;
        item.Cos = currentLayer.CurrentRotation.Cos;
        item.Triangle.A = a;
        item.Triangle.B = b;
        item.Triangle.C = c;
        item.Triangle.BorderTh = params.BorderTh;

        AddRenderItemToLayer(currentLayer, item);
    }

    void DrawLine(Vector2 a, Vector2 b, const LineParams& params)
    {
        Layer& currentLayer = GetCurrentLayer();

        if (params.Th <= 0.f || IsDrawingSkipped())
            return;

        a.X = Floor(a.X) + 0.5f;
        a.Y = Floor(a.Y) + 0.5f;
        b.X = Floor(b.X) + 0.5f;
        b.Y = Floor(b.Y) + 0.5f;

        RenderItem item{};
        item.Type = RenderItemType::Line;
        item.Color1 = params.FillColor;
        item.Sin = currentLayer.CurrentRotation.Sin;
        item.Cos = currentLayer.CurrentRotation.Cos;
        item.Line.A = a;
        item.Line.B = b;
        item.Line.Th = params.Th;

        AddRenderItemToLayer(currentLayer, item);
    }

    void DrawImage(TextureHandle hImage, const Vector2& a, const Vector2& b, const ImageParams& params)
    {
        Layer& currentLayer = GetCurrentLayer();

        Texture2D image = g_Context->Textures[(size_t)hImage];

        if (image.View == nullptr || a.X >= b.X || a.Y >= b.Y || IsDrawingSkipped())
            return;

        RenderItem item{};
        item.Type = RenderItemType::Image;
        item.Sin = currentLayer.CurrentRotation.Sin;
        item.Cos = currentLayer.CurrentRotation.Cos;
        item.Color1 = params.BgColor;
        item.Image.A = a;
        item.Image.B = b;
        item.Image.UVmin = params.UVmin;
        item.Image.UVmax = params.UVmax;
        item.Image.R = params.R;

        AddRenderItemToLayer(currentLayer, item, {.Image = image.View});
    }

    static Vector2 AddGlyphToLayer(Layer& layer, Glyph& glyph, const Vector2& pos, float size, const GlyphParams& params)
    {
        Vector2 tl; tl.X = pos.X + glyph.OffX;
                    tl.Y = pos.Y + glyph.OffY;
        Vector2 br; br.X = tl.X + (float)glyph.W;
                    br.Y = tl.Y + (float)glyph.H;

        RenderItem item{};
        item.Type = RenderItemType::Glyph;
        item.Sin = layer.CurrentRotation.Sin;
        item.Cos = layer.CurrentRotation.Cos;
        item.Color1 = params.Color;
        item.Glyph.A = Round(tl);
        item.Glyph.B = Round(br);
        item.Glyph.UVmin = glyph.UV0;
        item.Glyph.UVmax = glyph.UV1;

        AddRenderItemToLayer(layer, item);

        Vector2 resultPos = pos;
        resultPos.X += Round(glyph.Advance * params.Spacing);
        return resultPos;
    }

    void DrawString(MsdfFontHandle hFont, WStringView text, const Vector2& pos, float size, const MsdfTextParams& params)
    {
        Layer& currentLayer = GetCurrentLayer();

        if (size < 1.f || IsDrawingSkipped())
            return;

        MsdfFont& font = GetMsdfFontFromHandle(hFont);

        Vector2 glyphPos = Trunc(pos);
        for (char16 wc : text)
        {
            if (wc == '\n')
            {
                glyphPos.X = pos.X;
                glyphPos.Y += font.LineHeight * size;
                continue;
            }

            glyphPos = AddMsdfGlyphToLayer(currentLayer, font, wc, glyphPos, size, params);
        }
    }

    void DrawString(FontHandle hFont, WStringView text, const Vector2& pos, float size, const TextParams& params)
    {
        Layer& currentLayer = GetCurrentLayer();

        if (size < 1.f || IsDrawingSkipped())
            return;

        Font& font = *g_Context->Fonts[(size_t)hFont];

        Vector2 glyphPos = Round(pos);
        size_t len = text.Length();
        for (size_t i = 0; i < len; ++i)
        {
            char16 wc = text[i];

            if (wc == '\n')
            {
                glyphPos = Round(Vector2(pos.X, glyphPos.Y + font.LineHeight * size));
                continue;
            }

            Glyph& glyph = FindOrLoadGlyph(font, wc, size);
            glyphPos = AddGlyphToLayer(currentLayer, glyph, glyphPos, size, params);

            /*if (i + 1 < len)
                glyphPos.X += GetKerning(font, wc, text[i + 1]) * size;*/
        }
    }

    struct StringEditState { size_t CursorPos; float BlinkAnim; float LastTime; };

    bool StringEdit(size_t id, FontHandle hFont, wchar_t* buf, size_t bufSize, float fontH, const Rect& bounds, const StringEditParams& params)
    {
        HAX_ASSERT(g_Context != nullptr);

        if (fontH < 1.f)
            return false;

        fontH = Round(fontH);
        uint32 size = (uint32)fontH;

        size_t len = wcslen(buf);
        const Vector2 textSize = CalcTextSize(hFont, WStringView(buf, len), fontH);

        Rect area;
        area.Min = bounds.Min;
        area.Max = area.Min + Max(bounds.GetSize(), textSize);
        
        StringEditState& state = GetState<StringEditState>(id);

        auto res = Interact(id, area);
        bool clicked = res.Pressed;
        bool focused = res.Focused;

        Vector2 textPos;
        textPos.X = Round(bounds.Min.X);
        textPos.Y = Round(bounds.Min.Y + (area.GetHeight() - textSize.Y) / 2.f);

        if (buf[0] == '\0' && !params.Hint.Empty())
            DrawString(hFont, params.Hint, textPos, fontH, {.Color = params.HintColor});

        state.CursorPos = Min(state.CursorPos, len);
        const Vector2 mousePos = GetMousePos();
        if (clicked)
            state.CursorPos = (mousePos.X <= textPos.X) ? 0 : len;

        Font& font = *g_Context->Fonts[(size_t)hFont];
        float caretVisualX = textPos.X;
        bool changed = false;

        if (focused)
        {
            float totalTime = (float)GetTime();

            state.BlinkAnim += GetDeltaTime();

            if (IsKeyDownRepeat(VK_DELETE) && state.CursorPos < len)
            {
                wmemmove(&buf[state.CursorPos], &buf[state.CursorPos + 1], len - state.CursorPos);
                state.BlinkAnim = 0;
                --len;
                changed = true;
            }

            if (IsKeyDownRepeat(VK_BACK) && state.CursorPos > 0)
            {
                wmemmove(&buf[state.CursorPos - 1], &buf[state.CursorPos], len - state.CursorPos + 1);
                state.CursorPos--;
                state.BlinkAnim = 0;
                --len;
                changed = true;
            }

            if (IsKeyDownRepeat(VK_LEFT) && state.CursorPos > 0)
            {
                state.CursorPos--;
                state.BlinkAnim = 0;
                changed = true;
            }

            if (IsKeyDownRepeat(VK_RIGHT) && state.CursorPos < len)
            {
                state.BlinkAnim = 0;
                state.CursorPos++;
                changed = true;
            }

            size_t extraSpace = bufSize - len - 1;
            for (size_t i = 0; i < g_Context->CharsPressedThisFrame.Size() && i < extraSpace; ++i)
            {
                wmemmove(&buf[state.CursorPos + 1], &buf[state.CursorPos], len - state.CursorPos + 1);
                buf[state.CursorPos++] = g_Context->CharsPressedThisFrame[i];
                state.BlinkAnim = 0;
                len++;
                changed = true;
            }

            if (changed)
                state.LastTime = totalTime;
        }

        for (size_t i = 0; i <= len; ++i)
        {
            if (i == state.CursorPos)
                caretVisualX = textPos.X;

            if (i < len)
            {
                char16 sym = buf[i];
                Glyph& glyph = FindOrLoadGlyph(font, sym, fontH);
                float charWidth = glyph.Advance;

                if (clicked && mousePos.X >= textPos.X && mousePos.X < (textPos.X + charWidth))
                {
                    state.CursorPos = i + 1;
                    state.BlinkAnim = 0;

                    if (mousePos.X < (textPos.X + charWidth * 0.5f))
                    {
                        state.CursorPos = i;
                        caretVisualX = textPos.X;
                    }
                }

                textPos = AddGlyphToLayer(GetCurrentLayer(), glyph, textPos, fontH, {.Color = params.TextColor});
            }
        }

        if (focused)
        {
            if (bool cursorVisible = Mod(state.BlinkAnim, 1.20f) <= 0.80f)
            {
                Vector2 a{caretVisualX, textPos.Y};
                Vector2 b{caretVisualX, textPos.Y + textSize.Y};

                DrawLine(a, b, {.FillColor = params.CaretColor});
            }
        }

        return changed;
    }

    // Skip

    void PushSkipDrawing()
    {
        HAX_ASSERT(g_Context != nullptr);
        g_Context->CurrentLayer->SkipCounter++;
    }

    void PopSkipDrawing(uint32 count)
    {
        HAX_ASSERT(g_Context != nullptr);
        int& counter = g_Context->CurrentLayer->SkipCounter;
        HAX_ASSERT(counter > 0 && "Begin / End skip mismatch");
        counter--;
    }

    bool IsDrawingSkipped()
    {
        Layer& currentLayer = GetCurrentLayer();
        return currentLayer.SkipCounter > 0 || currentLayer.CurrentClipRect.IsInverted();
    }

    void BeginRotation(float angleRad)
    {
        Layer& currentLayer = GetCurrentLayer();
        Rotation& currentRotation = currentLayer.CurrentRotation;

        currentLayer.RotationStack.PushBack(currentRotation);

        currentRotation.Angle += angleRad;
        currentRotation.Sin = Sin(currentRotation.Angle);
        currentRotation.Cos = Cos(currentRotation.Angle);
    }

    void EndRotation()
    {
        Layer& currentLayer = GetCurrentLayer();

        HAX_ASSERT(!currentLayer.RotationStack.Empty() && "Begin / End rotation mismatch!");
        currentLayer.CurrentRotation = currentLayer.RotationStack.Last();
        currentLayer.RotationStack.PopBack();
    }

    void PushClipRect(const Rect& clipRect)
    {
        Layer& currentLayer = GetCurrentLayer();

        currentLayer.ClipRectStack.PushBack(currentLayer.CurrentClipRect); 
        currentLayer.CurrentClipRect = currentLayer.CurrentClipRect.Intersect(clipRect); 
    }

    void PopClipRect()
    {
        Layer& currentLayer = GetCurrentLayer();

        HAX_ASSERT(!currentLayer.ClipRectStack.Empty()); 
        currentLayer.CurrentClipRect = currentLayer.ClipRectStack.Last(); 
        currentLayer.ClipRectStack.PopBack();
    }

    static void BeginLayout(float spacing, LayoutType type)
    {
        Layer& layer = GetCurrentLayer();
        Layout& currentLayout = layer.CurrentLayout;

        layer.LayoutStack.PushBack(currentLayout);
        currentLayout.Bounds = Rect(currentLayout.CursorPos, currentLayout.CursorPos);
        currentLayout.Type = type;
        currentLayout.Spacing = spacing;
    }

    static void EndLayout(LayoutType type)
    {
        Layer& currentLayer = GetCurrentLayer();
        Layout& currentLayout = currentLayer.CurrentLayout;

        HAX_ASSERT(currentLayout.Type == type);
        HAX_ASSERT(!currentLayer.LayoutStack.Empty() && "Begin / End layout mismatch!");

        const Vector2 size = currentLayout.Bounds.GetSize();

        currentLayout = currentLayer.LayoutStack.Last();
        PlaceItem(size);
        currentLayer.LayoutStack.PopBack();
    }

    void BeginVertical(float spacing)
    {
        BeginLayout(spacing, LayoutType::Vertical);
    }

    void EndVertical()
    {
        EndLayout(LayoutType::Vertical);
    }

    void BeginHorizontal(float spacing)
    {
        BeginLayout(spacing, LayoutType::Horizontal);
    }

    void EndHorizontal()
    {
        EndLayout(LayoutType::Horizontal);
    }

    void PlaceItem(const Vector2& size)
    {
        Layout& currentLayout = GetCurrentLayer().CurrentLayout;

        Rect itemBounds;
        itemBounds.Min = currentLayout.CursorPos;
        itemBounds.Max = itemBounds.Min + size;
        currentLayout.Bounds.Add(itemBounds);

        Space(currentLayout.Spacing + (currentLayout.IsHorizontal() ? size.X : size.Y));
    }

    void Space(float pixels)
    {
        Layout& currentLayout = GetCurrentLayer().CurrentLayout;

        if (currentLayout.IsHorizontal()) { currentLayout.CursorPos.X += pixels; }
        if (currentLayout.IsVertical())   { currentLayout.CursorPos.Y += pixels; }
    }

    Vector2 GetCursorPos()
    {
        return GetCurrentLayer().CurrentLayout.CursorPos;
    }

    void SetCursorPos(const Vector2& pos)
    {
        Layout& layout = GetCurrentLayer().CurrentLayout;
        HAX_ASSERT(layout.IsRoot() && "SetCursorPos must be used in root layers");
        layout.CursorPos = pos;
    }

    void VerticalLine(float th, const Color& color, float size)
    {
        if (size <= 0.f) 
            size = GetContentRegionAvail().Y + size;

        Rect bounds;
        bounds.Min = GetCursorPos();
        bounds.Max = bounds.Min + Vector2(th, size);

        PlaceItem(bounds.GetSize());
        if (IsItemVisible(bounds))
            DrawRect(bounds.Min, bounds.Max, {.FillColor = color});
    }

    void HorizontalLine(float th, const Color& color, float size)
    {
        if (size <= 0.f) 
            size = GetContentRegionAvail().X + size;

        Rect bounds;
        bounds.Min = GetCursorPos();
        bounds.Max = bounds.Min + Vector2(size, th);

        PlaceItem(bounds.GetSize());
        if (IsItemVisible(bounds))
            DrawRect(bounds.Min, bounds.Max, { .FillColor = color });
    }

    Rect GetLayoutBounds()
    {
        return g_Context->CurrentLayer->CurrentLayout.Bounds;
    }

    float GetLayoutSpacing()
    {
        return g_Context->CurrentLayer->CurrentLayout.Spacing;
    }

    void ResetCursor()
    {
        Layout& currentLayout = GetCurrentLayer().CurrentLayout;
        currentLayout.CursorPos = currentLayout.Bounds.Min;
    }

    bool IsItemVisible(const Rect& bounds)
    {
        Layer& currentLayer = GetCurrentLayer();
        return !IsDrawingSkipped() && currentLayer.CurrentClipRect.Intersects(bounds);
    }

    void BeginContainer(size_t id, const ContainerParams& params)
    {
        Layer& currentLayer = GetCurrentLayer();
        Container& currentContainer = currentLayer.CurrentContainer;

        // Calculate bounds
        Vector2 cursorPos = GetCursorPos();
        Vector2 spaceLeft = currentContainer.Bounds.Max - cursorPos;

        Vector2 size{params.W, params.H};
        if (size.X <= 0.f) size.X = spaceLeft.X + size.X;
        if (size.Y <= 0.f) size.Y = spaceLeft.Y + size.Y;

        uint32 flags = ContainerFlag_None;
        if (params.Clip)            { flags |= ContainerFlag_Clip; }
        if (params.ScrollX)         { flags |= ContainerFlag_ScrollX; }
        if (params.ScrollY)         { flags |= ContainerFlag_ScrollY; }
        if (params.ScrollVisible)   { flags |= ContainerFlag_ScrollVisible; }
        if (params.FitX)            { flags |= ContainerFlag_FitX; flags &= ~ContainerFlag_ScrollX; }
        if (params.FitY)            { flags |= ContainerFlag_FitY; flags &= ~ContainerFlag_ScrollY; }

        Rect bounds = Rect(cursorPos, cursorPos + size);

        Vector2 layoutPos = cursorPos;

        if (flags > ContainerFlag_Clip)
        {
            HAX_ASSERT(id > 0);
            auto& state = GetState<ContainerState>(id);
            state.Scroll.Style = params.Style;

            if (flags & ContainerFlag_FitX)
                bounds.Max.X = bounds.Min.X + state.Bounds.GetSize().X;
            if (flags & ContainerFlag_FitY)
                bounds.Max.Y = bounds.Min.Y + state.Bounds.GetSize().Y;

            auto& scroll = state.Scroll;
            if ((flags & ContainerFlag_ScrollY) != 0 && scroll.Offset.Y >= 0.f)
            {
                currentLayer.CurrentScrollId = id; 

                Vector2 mousePos = g_Context->Mouse.Pos;
                if (currentLayer.CurrentClipRect.Contains(mousePos) && bounds.Contains(mousePos))
                {
                    auto& hoveredCont = g_Context->Interaction.NextFrame.HoveredContainer;

                    if (currentLayer.ZOrder >= hoveredCont.ZOrder)
                    {
                        hoveredCont.Id = id;
                        hoveredCont.ZOrder = currentLayer.ZOrder;
                    }
                }

                bounds.Max.X -= params.Style.TrackWidth * G.ScaleFactor;
                layoutPos.Y -= Floor(scroll.Offset.Y);
            }

            if ((flags & ContainerFlag_ScrollX) != 0 && scroll.Offset.X >= 0.f)
            {
                currentLayer.CurrentScrollId = id; 

                bounds.Max.Y -= params.Style.TrackWidth * G.ScaleFactor;
                layoutPos.X -= Floor(scroll.Offset.X);
                flags |= ContainerFlag_ScrollX;
            }
        }

        currentLayer.LayoutStack.PushBack(currentLayer.CurrentLayout);

        Layout& currentLayout = currentLayer.CurrentLayout;
        currentLayout.CursorPos = layoutPos;
        currentLayout.Bounds = Rect(layoutPos, layoutPos);
        currentLayout.Type = LayoutType::Container;

        currentLayer.ContainerStack.PushBack(currentContainer);
        currentContainer.Bounds = bounds;
        currentContainer.Id = id;
        currentContainer.Flags = flags;

        if ((flags & ContainerFlag_Clip))
            PushClipRect(bounds);
    }

    Vector2 GetContentRegionAvail()
    {
        Layer& layer = *g_Context->CurrentLayer;
        return layer.CurrentContainer.Bounds.Max - layer.CurrentLayout.CursorPos;
    }

    void ScrollYTo(float posY)
    {
        size_t id = g_Context->CurrentLayer->CurrentScrollId;
        if (id == kInvalidId)
            return;

        ContainerState& state = GetState<ContainerState>(id);

        float minY = state.Bounds.Min.Y;
        float maxY = state.Bounds.Max.Y;

        if (posY > minY && posY < maxY)
            state.Scroll.TargetOffsetY = posY - minY;
    }

    void EndContainer()
    {
        Layer& currentLayer = GetCurrentLayer();
        Layout& currentLayout = currentLayer.CurrentLayout;
        Container& currentContainer = currentLayer.CurrentContainer;

        HAX_ASSERT(currentLayout.Type == LayoutType::Container);

        ContainerState* state = nullptr;
        if (currentContainer.RequiresState())
            state = &GetState<ContainerState>(currentContainer.Id);

        if (state)
            state->Bounds = currentLayout.Bounds;

        Rect contentBounds = currentLayout.Bounds;
        float maxHeight = currentContainer.Bounds.GetSize().Y;
        float maxWidth = currentContainer.Bounds.GetSize().X;
        float height = contentBounds.GetSize().Y;
        float width = contentBounds.GetSize().X;
        bool yScrollVisible = (height > maxHeight && currentContainer.ScrollY()) || currentContainer.ScrollVisible();
        bool xScrollVisible = (width > maxWidth && currentContainer.ScrollX()) || currentContainer.ScrollVisible();
        Vector2 realSize = currentContainer.Bounds.GetSize();
        if (yScrollVisible)
            realSize.X += state->Scroll.Style.TrackWidth;
        if (xScrollVisible)
            realSize.Y += state->Scroll.Style.TrackWidth;

        currentLayout = currentLayer.LayoutStack.Last();
        currentLayer.LayoutStack.PopBack();
        PlaceItem(realSize);

        if ((currentContainer.Flags & ContainerFlag_Clip))
            PopClipRect();

        // Scroll
        if (currentContainer.ScrollX() || currentContainer.ScrollY())
        {
            auto& scroll = state->Scroll;

            size_t internalId = (size_t)&scroll;

            float trackWidth = Max(3.f, state->Scroll.Style.TrackWidth * G.ScaleFactor);
            float padding = Max(0.f, Min(state->Scroll.Style.ThumbPadding * G.ScaleFactor, (trackWidth - 1) / 2.f));
            float thumbWidth = trackWidth - padding * 2.f;

            // ScrollY
            if (currentLayer.CurrentContainer.ScrollY())
            {
                if (yScrollVisible)
                {
                    height = Max(height, maxHeight);

                    bool firstFrameVisible = scroll.Offset.Y < 0.f;
                    scroll.Offset.Y = Max(0.f, Min(scroll.Offset.Y, height - maxHeight));

                    Rect trackBounds;
                    trackBounds.Min = currentLayer.CurrentContainer.Bounds.GetTR();
                    trackBounds.Max = currentLayer.CurrentContainer.Bounds.GetBR() + Vector2(trackWidth, 0.f);

                    Rect thumbBounds;
                    thumbBounds.Min = trackBounds.Min + Vector2(padding, scroll.Offset.Y / height * trackBounds.GetSize().Y + padding);
                    thumbBounds.Max = thumbBounds.Min + Vector2(thumbWidth, maxHeight / height * trackBounds.GetSize().Y - padding * 2.f);

                    InterRes trackHitTest = Interact(internalId, trackBounds);
                    InterRes thumbHitTest = Interact(internalId + 1, thumbBounds);

                    if (!firstFrameVisible)
                    {
                        Color trackColor = state->Scroll.Style.TrackCol;
                        DrawRect(trackBounds.Min, trackBounds.Max, {.BorderColor = trackColor, .FillColor = trackColor});

                        Color thumbColorHeld = state->Scroll.Style.ThumbActiveCol;
                        Color thumbColorHovered = state->Scroll.Style.ThumbHovCol;
                        Color thumbColor = thumbHitTest.Active ? thumbColorHeld : thumbHitTest.Hovered ? thumbColorHovered : state->Scroll.Style.ThumbCol;
                        DrawRect(thumbBounds.Min, thumbBounds.Max, {.FillColor = thumbColor, .Rounding = 5_px});
                    }

                    UpdateWheelScroll(currentContainer.Id, *state, maxHeight, height - maxHeight);

                    if (trackHitTest.Pressed && (float)g_Context->Timer.Time - scroll.LastTimeHeldY > 0.2f)
                    {
                        scroll.LastTimeHeldY = (float)g_Context->Timer.Time;

                        float delta = 0.f;
                        if (g_Context->Mouse.Pos.Y < thumbBounds.Min.Y) delta = -maxHeight;
                        if (g_Context->Mouse.Pos.Y > thumbBounds.Max.Y) delta = maxHeight;

                        scroll.Offset.Y += delta;
                        scroll.TargetOffsetY += delta;
                    }

                    if (thumbHitTest.Active)
                    {
                        float delta = g_Context->Mouse.DeltaPos.Y / trackBounds.GetSize().Y * height;
                        scroll.Offset.Y += delta;
                        scroll.TargetOffsetY += delta;
                    }

                    scroll.Offset.Y = Min(scroll.Offset.Y, height - maxHeight);
                    scroll.Offset.Y = Max(scroll.Offset.Y, 0.f);

                    scroll.TargetOffsetY = Max(scroll.TargetOffsetY, 0.f);
                    scroll.TargetOffsetY = Min(scroll.TargetOffsetY, height - maxHeight);
                }
                else
                {
                    scroll.Offset.Y = -1;
                    scroll.TargetOffsetY = -1;
                }
            }

            // ScrollX
            if (currentContainer.ScrollX())
            {
                if (xScrollVisible)
                {
                    width = Max(width, maxWidth);

                    bool firstFrameVisible = scroll.Offset.X < 0.f;
                    scroll.Offset.X = Max(0.f, Min(scroll.Offset.X, width - maxWidth));

                    Rect trackBounds;
                    trackBounds.Min = currentContainer.Bounds.GetBL();
                    trackBounds.Max = currentContainer.Bounds.GetBR() + Vector2(0.f, trackWidth);

                    Rect thumbBounds;
                    thumbBounds.Min = trackBounds.Min + Vector2(scroll.Offset.X / width * trackBounds.GetSize().X + padding, padding);
                    thumbBounds.Max = thumbBounds.Min + Vector2(maxWidth / width * trackBounds.GetSize().X - padding * 2.f, thumbWidth);

                    InterRes trackHitTest = Interact(internalId + 2, trackBounds);
                    InterRes thumbHitTest = Interact(internalId + 3, thumbBounds);

                    if (!firstFrameVisible)
                    {
                        Color trackColor = state->Scroll.Style.TrackCol;
                        DrawRect(trackBounds.Min, trackBounds.Max, {.FillColor = trackColor});

                        Color thumbColorHeld = state->Scroll.Style.ThumbActiveCol;
                        Color thumbColorHovered = state->Scroll.Style.ThumbHovCol;
                        Color thumbColor = thumbHitTest.Active ? thumbColorHeld : thumbHitTest.Hovered ? thumbColorHovered : state->Scroll.Style.ThumbCol;
                        DrawRect(thumbBounds.Min, thumbBounds.Max, {.FillColor = thumbColor, .Rounding = 5_px});
                    }

                    if (trackHitTest.Pressed && (float)g_Context->Timer.Time - scroll.LastTimeHeldX > 0.2f)
                    {
                        scroll.LastTimeHeldX = (float)g_Context->Timer.Time;

                        if (g_Context->Mouse.Pos.X < thumbBounds.Min.X) scroll.Offset.X -= maxWidth;
                        if (g_Context->Mouse.Pos.X > thumbBounds.Max.X) scroll.Offset.X += maxWidth;
                    }

                    if (thumbHitTest.Active)
                        scroll.Offset.X += g_Context->Mouse.DeltaPos.X / trackBounds.GetSize().X * width;

                    scroll.Offset.X = Max(scroll.Offset.X, 0.f);
                    scroll.Offset.X = Min(scroll.Offset.X, width - maxWidth);
                }
                else
                    scroll.Offset.X = -1;
            }
        }

        currentContainer = currentLayer.ContainerStack.Last();
        currentLayer.ContainerStack.PopBack();
    }

    Rect GetContainerBounds()
    {
        return g_Context->CurrentLayer->CurrentContainer.Bounds;
    }

    Span<const uint8> GetResourceData(Handle resModule, int resId, const char16* resType)
    {
        HRSRC resInfo = ::FindResource((HMODULE)resModule, MAKEINTRESOURCE(resId), resType);
        HAX_ASSERT(resInfo);

        HGLOBAL resMem = ::LoadResource((HMODULE)resModule, resInfo);
        HAX_ASSERT(resMem);

        const uint8* resData = (uint8*)::LockResource(resMem);
        HAX_ASSERT(resData);

        DWORD resDataSize = ::SizeofResource((HMODULE)resModule, resInfo);
        return Span(resData, resDataSize);
    }

    Vector2 CalcTextSize(FontHandle hFont, WStringView text, float size, float spacing)
    {
        HAX_ASSERT(g_Context != nullptr);

        Font& font = *g_Context->Fonts[(size_t)hFont];
        size = Round(size);

        Vector2 textSize = {0.f, Round(font.LineHeight * size)};
        float curLineWidth = 0.f;

        size_t len = text.Length();
        for (size_t i = 0; i < len; ++i)
        {
            char16 wc = text[i];

            if (wc == '\n')
            {
                textSize.X = Max(textSize.X, curLineWidth);
                textSize.Y += font.LineHeight * size;
                curLineWidth = 0.f;
                continue;
            }

            const Glyph& glyph = FindOrLoadGlyph(font, wc, (uint32)size);

            if (i + 1 == len)
            {
                curLineWidth += glyph.OffX + (float)glyph.W;
            }
            else
            {
                curLineWidth += glyph.Advance * spacing;
            }
        }

        textSize.X = Max(textSize.X, curLineWidth);
        return textSize;
    }

    Vector2 CalcTextSize(MsdfFontHandle hFont, WStringView text, float size, float spacing)
    {
        MsdfFont& font = GetMsdfFontFromHandle(hFont);

        Vector2 textSize{0.f, font.LineHeight * size};

        float curLineWidth = 0.f;

        for (char16 wc : text)
        {
            if (wc == '\n')
            {
                textSize.X = Max(textSize.X, curLineWidth);
                textSize.Y += font.LineHeight * size;
                curLineWidth = 0.f;
                continue;
            }

            MsdfGlyph* const* it = font.Glyphs.Get(wc);
            if (it == nullptr)
                continue;

            MsdfGlyph* glyph = *it;
            curLineWidth += glyph->Advance.X * size * spacing;
        }

        textSize.X = Max(textSize.X, curLineWidth);
        return textSize;
    }

    Vector2 GetGlyphSize(FontHandle hFont, char16 sym, float h)
    {
        HAX_ASSERT(g_Context != nullptr);

        Font& font = *g_Context->Fonts[(size_t)hFont];
        h = Round(h);

        const Glyph& glyph = FindOrLoadGlyph(font, sym, (uint32)h);
        return Vector2((float)glyph.W, (float)glyph.H);
    }

    FontMetrics GetFontMetrics(FontHandle hFont, float fontH)
    {
        HAX_ASSERT(g_Context != nullptr);

        Font& font = *g_Context->Fonts[(size_t)hFont];
        fontH = Round(fontH);

        return FontMetrics
        {
            .Ascent = font.Ascent * fontH,
            .CapHeight = font.Ascent * fontH,
            .Descent = font.Descent * fontH,
            .LineHeight = font.LineHeight * fontH
        };
    }

    float GetFontLineHeight(FontHandle hFont, float fontH)
    {
        HAX_ASSERT(g_Context != nullptr);

        Font& font = *g_Context->Fonts[(size_t)hFont];
        return font.LineHeight * fontH;
    }

    float GetFontAscent(FontHandle hFont, float fontH)
    {
        HAX_ASSERT(g_Context != nullptr);

        Font& font = *g_Context->Fonts[(size_t)hFont];
        return font.Ascent * fontH;
    }

    float GetFontCapHeight(FontHandle hFont, float fontH)
    {
        HAX_ASSERT(g_Context != nullptr);

        Font& font = *g_Context->Fonts[(size_t)hFont];
        return font.CapHeight * fontH;
    }

    float GetFontLineHeight(MsdfFontHandle hFont, float fontH)
    {
        return GetMsdfFontFromHandle(hFont).LineHeight * fontH;
    }

    FontHandle LoadFont(Span<const uint8> rawData)
    {
        HAX_ASSERT(g_Context != nullptr);
        IDWriteFactory5* factory = g_Context->DWriteFactory;
        IDWriteInMemoryFontFileLoader* loader = g_Context->DWriteFontFileLoader;

        DWRITE_CONTAINER_TYPE containerType = factory->AnalyzeContainerType(rawData.Data(), (UINT32)rawData.Size());
        if (containerType == DWRITE_CONTAINER_TYPE_WOFF || containerType == DWRITE_CONTAINER_TYPE_WOFF2)
            return LoadWoffFont(rawData);

        ComPtr<IDWriteFontFile> pFontFile = nullptr;
        HRESULT res = loader->CreateInMemoryFontFileReference(factory, rawData.Data(), (UINT32)rawData.Size(), nullptr, &pFontFile);
        HAX_ASSERT(SUCCEEDED(res));
        
        IDWriteFontFile* files[] = {pFontFile.Get()};
        IDWriteFontFace* pFontFace = nullptr;
        res = factory->CreateFontFace(DWRITE_FONT_FACE_TYPE_UNKNOWN, 1, files, 0, DWRITE_FONT_SIMULATIONS_NONE, &pFontFace);
        HAX_ASSERT(SUCCEEDED(res));

        DWRITE_FONT_METRICS metrics;
        pFontFace->GetMetrics(&metrics);
        const float invEm = 1.0f / (float)metrics.designUnitsPerEm;

        Font* newFont = New<Font>(g_Context->FontAlloc);
        newFont->FontFace = pFontFace;
        newFont->Ascent = metrics.ascent * invEm;
        newFont->Descent = metrics.descent * invEm;
        newFont->CapHeight = metrics.capHeight * invEm;
        newFont->LineHeight = (metrics.ascent + metrics.descent + metrics.lineGap) * invEm;
        newFont->DesignUnitsPerEm =  (float)metrics.designUnitsPerEm;

        g_Context->Fonts.PushBack(newFont);
        return (FontHandle)(g_Context->Fonts.Size() - 1);
    }

    class WoffLoader : public IDWriteFontFileLoader 
    {
    public:
        WoffLoader(IDWriteFontFileStream* s) : m_Stream(s)    { m_Stream->AddRef(); }
        ~WoffLoader()                                         { m_Stream->Release(); }

        HRESULT STDMETHODCALLTYPE   CreateStreamFromKey(const void*, UINT32, IDWriteFontFileStream** stream) override   { *stream = m_Stream; m_Stream->AddRef(); return S_OK; }
        HRESULT STDMETHODCALLTYPE   QueryInterface(REFIID riid, void** ppv) override                                    { if (riid == __uuidof(IUnknown) || riid == __uuidof(IDWriteFontFileLoader)) { *ppv = this; return S_OK; } return E_NOINTERFACE; }
        ULONG STDMETHODCALLTYPE     AddRef() override                                                                   { return ++m_RefCount; }
        ULONG STDMETHODCALLTYPE     Release() override                                                                  { ULONG count = --m_RefCount; if (count == 0) { Free(g_Context->FontAlloc, this); return 0; } return count; }

    private:
        IDWriteFontFileStream*      m_Stream;
        ULONG                       m_RefCount = 1;
    };

    static FontHandle LoadWoffFont(Span<const uint8> rawData)
    {
        HAX_ASSERT(g_Context != nullptr);
        IDWriteFactory5* factory = g_Context->DWriteFactory;

        DWRITE_CONTAINER_TYPE containerType = factory->AnalyzeContainerType(rawData.Data(), (UINT32)rawData.Size());
        ComPtr<IDWriteFontFile> pFontFile = nullptr;

        HAX_ASSERT(containerType == DWRITE_CONTAINER_TYPE_WOFF || containerType == DWRITE_CONTAINER_TYPE_WOFF2);
        
        ComPtr<IDWriteFontFileStream> unpackedStream = nullptr;
        HRESULT res = factory->UnpackFontFile(containerType, rawData.Data(), (UINT32)rawData.Size(), &unpackedStream);
        HAX_ASSERT(SUCCEEDED(res));

        WoffLoader* loader = New<WoffLoader>(g_Context->FontAlloc, unpackedStream.Get());
        res = factory->RegisterFontFileLoader(loader);
        HAX_ASSERT(SUCCEEDED(res));

        ComPtr<IDWriteFontFile> fontFile = nullptr;
        res = factory->CreateCustomFontFileReference(loader, sizeof(void*), loader, &fontFile);
        HAX_ASSERT(SUCCEEDED(res));
        factory->UnregisterFontFileLoader(loader);

        IDWriteFontFace3* pFontFace = nullptr;
        IDWriteFontFile* files[] = {fontFile.Get()};
        res = factory->CreateFontFace(DWRITE_FONT_FACE_TYPE_TRUETYPE, 1, files, 0, DWRITE_FONT_SIMULATIONS_NONE, (IDWriteFontFace**)&pFontFace);
        HAX_ASSERT(SUCCEEDED(res));

        DWRITE_FONT_METRICS metrics;
        pFontFace->GetMetrics(&metrics);
        const float invEm = 1.0f / (float)metrics.designUnitsPerEm;

        Font* newFont = New<Font>(g_Context->FontAlloc);
        newFont->FontFace = pFontFace;
        newFont->Ascent = metrics.ascent * invEm;
        newFont->Descent = metrics.descent * invEm;
        newFont->CapHeight = metrics.capHeight * invEm;
        newFont->LineHeight = (metrics.ascent + metrics.descent + metrics.lineGap) * invEm;
        newFont->DesignUnitsPerEm =  (float)metrics.designUnitsPerEm;
        newFont->WoffLoader = loader;

        g_Context->Fonts.PushBack(newFont);
        return (FontHandle)(g_Context->Fonts.Size() - 1);
    }

    FontHandle LoadSystemFont(const char16* fontFamilyName, FontWeight weight, bool italic)
    {
        IDWriteFactory5* factory = g_Context->DWriteFactory;

        IDWriteFontCollection* pCollection = nullptr;
        HRESULT res = factory->GetSystemFontCollection(&pCollection);
        HAX_ASSERT(SUCCEEDED(res));

        uint32 index = 0;
        BOOL exists = 0;
        pCollection->FindFamilyName(fontFamilyName, &index, &exists);
        HAX_ASSERT(exists);

        IDWriteFontFamily* pFamily = nullptr;
        pCollection->GetFontFamily(index, &pFamily);

        IDWriteFont* pFont = nullptr;
        pFamily->GetFirstMatchingFont
        (
            (DWRITE_FONT_WEIGHT)weight,
            DWRITE_FONT_STRETCH_NORMAL,
            italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
            &pFont
        );

        IDWriteFontFace* pFontFace = nullptr;
        pFont->CreateFontFace(&pFontFace);

        DWRITE_FONT_METRICS metrics;
        pFontFace->GetMetrics(&metrics);
        const float invEm = 1.0f / (float)metrics.designUnitsPerEm;

        Font* newFont = New<Font>(g_Context->FontAlloc);
        newFont->FontFace = pFontFace;
        newFont->Ascent = (float)metrics.ascent * invEm;
        newFont->Descent = (float)metrics.descent * invEm;
        newFont->LineHeight = (float)(metrics.ascent + metrics.descent + metrics.lineGap) * invEm;

        pFont->Release();
        pFamily->Release();
        pCollection->Release();

        g_Context->Fonts.PushBack(newFont);
        return (FontHandle)(g_Context->Fonts.Size() - 1);
    }

    void UnloadFont(FontHandle hFont)
    {
        HAX_ASSERT(g_Context != nullptr);

        if (Font*& font = g_Context->Fonts[(size_t)hFont])
        {
            font->Glyphs.ClearFree();
            font->FontFace->Release();
            if (font->WoffLoader) font->WoffLoader->Release();
            Delete(g_Context->FontAlloc, font);
            font = nullptr;
        }
    }

    static float GetKerning(Font& font, char16 sym1, char16 sym2)
    {
        float* kerning = font.Kernings.Get({sym1, sym2});
        if (kerning != nullptr)
            return *kerning;

        IDWriteFontFace1* face1 = nullptr;
        font.FontFace->QueryInterface(__uuidof(IDWriteFontFace1), (void**)&face1);

        uint16 glyphIndices[2];
        uint32 codePoints[2] = {(uint32)sym1, (uint32)sym2};
        font.FontFace->GetGlyphIndices(codePoints, 2, glyphIndices);

        int adjustment = 0;
        face1->GetKerningPairAdjustments(1, glyphIndices, &adjustment);

        face1->Release();

        float k = (float)adjustment * (1.f / font.DesignUnitsPerEm);
        font.Kernings.Insert({sym1, sym2}, k);
        return k;
    }

    static Glyph& FindOrLoadGlyph(Font& font, char16 sym, uint32 size)
    {
        Glyph& glyph = font.Glyphs.FindOrAdd({sym, size});
        if (glyph.Baked) return glyph;

        IDWriteFontFace* face = font.FontFace;
        uint32 codePoint = (uint32)sym;
        uint16 glyphIndex;
        face->GetGlyphIndices(&codePoint, 1, &glyphIndex);

        DWRITE_GLYPH_RUN run{};
        run.fontFace = face;
        run.fontEmSize = (float)size;
        run.glyphCount = 1;
        run.glyphIndices = &glyphIndex;

        IDWriteGlyphRunAnalysis* analysis = nullptr;
        IDWriteFactory2* factory2 = nullptr;
        g_Context->DWriteFactory->QueryInterface(__uuidof(IDWriteFactory2), (void**)&factory2);

        factory2->CreateGlyphRunAnalysis
        (
            &run,
            nullptr,
            DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC, 
            DWRITE_MEASURING_MODE_NATURAL,
            DWRITE_GRID_FIT_MODE_DISABLED,
            DWRITE_TEXT_ANTIALIAS_MODE_GRAYSCALE,
            0.0f, 0.0f,
            &analysis
        );

        factory2->Release();

        RECT rect;
        analysis->GetAlphaTextureBounds(DWRITE_TEXTURE_ALIASED_1x1, &rect);

        uint32 w = rect.right - rect.left;
        uint32 h = rect.bottom - rect.top;
        auto& atlas = g_Context->Bitmap;

        if (w > 0 && h > 0)
        {
            if (atlas.CurX + w + 1 >= kBitmapSize) 
            {
                atlas.CurX = 1;
                atlas.CurY += atlas.LineH + 1;
                atlas.LineH = 0;
            }

            uint32 bufferSize = w * h;
            uint8* alphaBuffer = (uint8*)Alloc(g_Context->FontAlloc, bufferSize);
            memset(alphaBuffer, 0, bufferSize);

            analysis->CreateAlphaTexture(DWRITE_TEXTURE_ALIASED_1x1, &rect, alphaBuffer, bufferSize);

            for (uint32 y = 0; y < h; ++y)
                for (uint32 x = 0; x < w; ++x)
                    atlas.Pixels[(atlas.CurY + y) * kBitmapSize + (atlas.CurX + x)] = g_Context->GammaLUT[alphaBuffer[y * w + x]];
            Free(g_Context->FontAlloc, alphaBuffer);

            atlas.DirtyRect.MinX = Min(atlas.DirtyRect.MinX, atlas.CurX);
            atlas.DirtyRect.MinY = Min(atlas.DirtyRect.MinY, atlas.CurY);
            atlas.DirtyRect.MaxX = Max(atlas.DirtyRect.MaxX, atlas.CurX + w);
            atlas.DirtyRect.MaxY = Max(atlas.DirtyRect.MaxY, atlas.CurY + h);
        }

        DWRITE_FONT_METRICS fontMetrics;
        face->GetMetrics(&fontMetrics);
        DWRITE_GLYPH_METRICS glyphMetrics;
        face->GetGdiCompatibleGlyphMetrics
        (
            (float)size, 
            1.0f,
            nullptr,
            true,
            &glyphIndex, 
            1, 
            &glyphMetrics, 
            false
        );

        float scale = (float)size / (float)fontMetrics.designUnitsPerEm;

        glyph.W = (int)w;
        glyph.H = (int)h;
        glyph.Advance = Round((float)glyphMetrics.advanceWidth * scale);
        glyph.OffX = (float)rect.left;
        glyph.OffY = Round((font.Ascent * (float)size)) + (float)rect.top;

        float invSize = 1.0f / (float)kBitmapSize;
        glyph.UV0 = { (atlas.CurX + 0.5f) * invSize, (atlas.CurY + 0.5f) * invSize };
        glyph.UV1 = { (atlas.CurX + w - 0.5f) * invSize, (atlas.CurY + h - 0.5f) * invSize };

        atlas.CurX += w + 1;
        atlas.LineH = Max(atlas.LineH, h);
        glyph.Baked = true;

        analysis->Release();
        return glyph;
    }

    static Glyph& FindOrLoadGlyph(Font& font, char16 sym, float fSize)
    {
        fSize = Round(fSize);
        return FindOrLoadGlyph(font, sym, (uint32)fSize);
    }

    void BakeGlyphsInRange(FontHandle hFont, char16 from, char16 to, uint32 size)
    {
        HAX_ASSERT(g_Context != nullptr);

        Font& font = *g_Context->Fonts[(size_t)hFont];

        for (char16 sym = from; sym < to; ++sym)
            FindOrLoadGlyph(font, sym, size);
    }

    Texture2D GetFontAtlas()
    {
        return g_Context->Bitmap.Texture;
    }

    static void ConvertImageToBytes(Span<const uint8> rawImage, Vector<uint8>& outPixels, uint32& outW, uint32& outH)
    {
        ComPtr<IWICImagingFactory> pFactory;
        HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
        HAX_ASSERT(SUCCEEDED(hr) && "Failed to create WIC Factory");

        ComPtr<IWICStream> pWICStream;
        hr = pFactory->CreateStream(&pWICStream);
        HAX_ASSERT(SUCCEEDED(hr));

        hr = pWICStream->InitializeFromMemory(const_cast<BYTE*>(rawImage.Data()), (DWORD)rawImage.Size());
        HAX_ASSERT(SUCCEEDED(hr) && "Invalid image data in memory");

        ComPtr<IWICBitmapDecoder> pDecoder;
        hr = pFactory->CreateDecoderFromStream(pWICStream.Get(), NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);
        HAX_ASSERT(SUCCEEDED(hr) && "Failed to decode image");

        ComPtr<IWICBitmapFrameDecode> pFrame;
        hr = pDecoder->GetFrame(0, &pFrame);
        HAX_ASSERT(SUCCEEDED(hr));

        pFrame->GetSize(&outW, &outH);

        ComPtr<IWICFormatConverter> pConverter;
        hr = pFactory->CreateFormatConverter(&pConverter);
        HAX_ASSERT(SUCCEEDED(hr));

        hr = pConverter->Initialize
        (
            pFrame.Get(),
            GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone,
            NULL, 0.0f,
            WICBitmapPaletteTypeCustom
        );
        HAX_ASSERT(SUCCEEDED(hr));

        outPixels.Resize(outW * outH * 4);
        hr = pConverter->CopyPixels(NULL, outW * 4, (UINT)outPixels.Size(), outPixels.Data());
        HAX_ASSERT(SUCCEEDED(hr));
    }

    TextureHandle LoadImageFromMemory(Span<const uint8> data)
    {
        HAX_ASSERT(g_Context != nullptr);

        uint32 w, h;
        Vector<uint8> pixels;
        ConvertImageToBytes(data, pixels, w, h);

        Texture2D texture = g_Context->Backend->CreateTexture(TextureFormat::R8G8B8A8_UNorm, pixels.Data(), w, h, 1);
        g_Context->Textures.PushBack(texture);

        return (TextureHandle)(g_Context->Textures.Size() - 1);
    }

    Vector2 GetImageSize(TextureHandle hImage)
    {
        HAX_ASSERT(g_Context != nullptr);

        Texture2D texture = g_Context->Textures[(size_t)hImage];
        return {(float)texture.Width, (float)texture.Height};
    }

    void UnloadImage(TextureHandle hImage)
    {
        HAX_ASSERT(g_Context != nullptr);
        g_Context->Backend->DestroyTexture(g_Context->Textures[(size_t)hImage]);
    }

    bool IsKeyDown(uint8 vk)
    {
        return g_Context->KeysData[vk].Down;
    }

    bool IsKeyJustDown(uint8 vk)
    {
        return g_Context->KeysData[vk].DownDuration == 0.f;
    }

    bool IsKeyDownRepeat(uint8 vk, float delay, float interval)
    {
        KeyData& key = g_Context->KeysData[vk];

        if (!key.Down)
            return false;

        if (g_Context->KeysData[vk].DownDuration == 0.f)
            return true;

        if (key.DownDuration <= delay)
            return false;

        float currentRel = key.DownDuration - delay;
        float prevRel = key.DownDurationPrev - delay;

        int currentStep = (int)(currentRel / interval);
        int prevStep = (int)(prevRel / interval);

        return currentStep > prevStep;
    }

    bool IsKeyUp(uint8 vk)
    {
        return !g_Context->KeysData[vk].Down;
    }

    bool IsKeyJustUp(uint8 vk)
    {
        return g_Context->KeysData[vk].DownDurationPrev >= 0.f && !g_Context->KeysData[vk].Down;
    }

    WStringView GetKeyName(uint8 vk)
    {
        return g_KeyToName[vk].CStr();
    }

    const Vector<uint8>& GetJustPressedKeys()
    {
        HAX_ASSERT(g_Context != nullptr);
        return g_Context->KeysPressedThisFrame;
    }

    IStatePool** GetStatePool(size_t typeId)
    {
        return g_Context->StatePools.Get(typeId);
    }

    void AddStatePool(size_t typeId, IStatePool* pool)
    {
        g_Context->StatePools.Insert(typeId, pool);
    }

    Allocator* IterAllocators(void*& iter)
    {
        size_t& idx = (size_t&)iter;

        Allocator* allocators[] = {&g_GlobalAlloc, &g_StateAlloc, &g_Context->FontAlloc, &g_Context->GeneralAlloc};
        return idx < _countof(allocators) ? allocators[idx++] : nullptr;
    }
}

namespace Hax::Gui
{
    LinearColor LinearColor::Brighten(float factor) const
    {
        LinearColor target{1.f, 1.f, 1.f, A};
        return Lerp(*this, target, factor);
    }

    LinearColor LinearColor::Darken(float factor) const
    {
        LinearColor target{0.f, 0.f, 0.f, A};
        return Lerp(*this, target, factor);
    }
}

namespace Hax::Gui
{ 
    static Vector2 AddMsdfGlyphToLayer(Layer& layer, MsdfFont& font, char16 sym, const Vector2& pos, float size, const MsdfGlyphParams& params)
    {
        auto it = font.Glyphs.Get(sym);
        if (it == nullptr)
            return pos;

        MsdfGlyph* glyph = *it;

        Vector2 atlasSize = {(float)g_Context->AtlasArray.Width, (float)g_Context->AtlasArray.Height};

        Vector2 uvPos1 = Vector2(glyph->ImageBounds.L, font.Atlas.Height - glyph->ImageBounds.T) / atlasSize;
        Vector2 uvPos2 = Vector2(glyph->ImageBounds.R, font.Atlas.Height - glyph->ImageBounds.B) / atlasSize;

        float scale = size;
        Vector2 baseLine{pos.X, pos.Y + font.Ascender * size};
        Vector2 posTL = baseLine + Vector2(glyph->PlaneBounds.L, -glyph->PlaneBounds.T) * scale;
        Vector2 posBR = baseLine + Vector2(glyph->PlaneBounds.R, -glyph->PlaneBounds.B) * scale;

        RenderItem item{};
        item.Type = RenderItemType::MsdfGlyph;
        item.Sin = layer.CurrentRotation.Sin;
        item.Cos = layer.CurrentRotation.Cos;
        item.Color1 = params.Col;
        item.Color2 = params.OutCol;
        item.MsdfGlyph.A = posTL;
        item.MsdfGlyph.B = posBR;
        item.MsdfGlyph.UVmin = uvPos1;
        item.MsdfGlyph.UVmax = uvPos2;
        item.MsdfGlyph.Index = font.Atlas.IndexInArray;
        item.MsdfGlyph.PxRange = font.DistanceRange;
        item.MsdfGlyph.AtlasSize = {(float)font.Atlas.Width, (float)font.Atlas.Height};
        item.MsdfGlyph.Weight = params.Weight;
        item.MsdfGlyph.Outline = params.Outline ? 1.f : 0.f;

        AddRenderItemToLayer(layer, item);

        return pos + glyph->Advance * scale * params.Spacing;
    }
}

namespace Hax::Gui
{
    static void GenerateGammaLUT(uint8_t* table, float gamma, float contrast)
    {
        for (int i = 0; i < 256; ++i) 
            table[i] = (uint8)Clamp(255.f * std::pow(((float)i / 255.f), gamma), 0.f, 255.f);
    }

    static Layer* FindLayerById(size_t id)
    {
        for (Layer* layer : g_Context->Layers)
            if (layer->Id == id)
                return layer;

        return nullptr;
    }

    void Initialize(Handle hwnd)
    {        
        HAX_ASSERT(g_Context != nullptr && "Not initialized");
        Context& ctx = *g_Context;

        // Timer
        {
            Timer& timer = ctx.Timer;

            ::QueryPerformanceFrequency((LARGE_INTEGER*)&timer.TicksPerSecond);
            ::QueryPerformanceCounter((LARGE_INTEGER*)&timer.TotalTicksLastFrame);
        }

        // Viewport
        {
            Viewport& viewport = ctx.Viewport;
            viewport.Hwnd = hwnd;
            UpdateViewportSize(viewport);
            UpdateViewportScale(viewport);
        }

        // Layers
        {
            CreateLayer(L"Default", 0);
            ctx.DefaultLayer = ctx.Layers[0];
        }

        {
            ctx.Bitmap.Pixels = (uint8*)Alloc(ctx.FontAlloc, (size_t)kBitmapSize * kBitmapSize);
            memset(ctx.Bitmap.Pixels, 0, (size_t)kBitmapSize * kBitmapSize);
            ctx.Bitmap.DirtyRect.MinX = ctx.Bitmap.DirtyRect.MinY = (uint32)-1;
        }

        // DirectWrite
        {
            HRESULT res = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), (IUnknown**)&ctx.DWriteFactory);
            HAX_ASSERT(SUCCEEDED(res));

            {
                IDWriteRenderingParams* params = nullptr;
                res = ctx.DWriteFactory->CreateRenderingParams(&params);
                HAX_ASSERT(SUCCEEDED(res));

                //ctx.Gamma = params->GetGamma();
                //ctx.Contrast = params->GetEnhancedContrast();
                ctx.Gamma = 1.2f;

                GenerateGammaLUT(ctx.GammaLUT, ctx.Gamma, ctx.Contrast);

                params->Release();
            }

            res = ctx.DWriteFactory->CreateInMemoryFontFileLoader(&ctx.DWriteFontFileLoader);
            HAX_ASSERT(SUCCEEDED(res));

            res = ctx.DWriteFactory->RegisterFontFileLoader(ctx.DWriteFontFileLoader);
            HAX_ASSERT(SUCCEEDED(res));
        }

        InitKeyNames();

        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        ctx.ShouldCoUninitialize = (hr == S_OK);
    }

    static void InitKeyNames()
    {
        static bool s_Inited;
        if (s_Inited)
            return;

        s_Inited = true;

        g_KeyToName[0] = L"None";
        for (uint32 vk = 1; vk < 256; ++vk)
        {
            wchar_t buffer[64];

            uint32 scanCode = ::MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
            long lParam = (scanCode << 16);

            if (vk >= VK_PRIOR && vk <= VK_HELP)                        lParam |= (1 << 24);
            if (vk >= VK_LWIN  && vk <= VK_APPS)                        lParam |= (1 << 24);
            if (vk == VK_DIVIDE || vk == VK_RCONTROL || vk == VK_RMENU) lParam |= (1 << 24);

            if (::GetKeyNameTextW(lParam, buffer, 64) < 1)
                wsprintf(buffer, L"Key%d", vk);

            g_KeyToName[vk].Append(buffer);
        }
    }

    InterRes Interact(size_t id, const Rect& bounds)
    {
        HAX_ASSERT(g_Context != nullptr);
        Context& ctx = *g_Context;

        if (id == kInvalidId)
            return InterRes();

        if (id == ctx.Interaction.ThisFrame.ActiveItemId)
            ctx.Interaction.NextFrame.ActiveItemId = id;

        bool isOtherItemActive = ctx.Interaction.ThisFrame.ActiveItemId != kInvalidId && ctx.Interaction.ThisFrame.ActiveItemId != id;
        bool isBoundsHovered = bounds.Contains(ctx.Mouse.Pos) && ctx.CurrentLayer->CurrentClipRect.Contains(ctx.Mouse.Pos);
        if (!isOtherItemActive && isBoundsHovered)
        {
            auto& hoveredItem = ctx.Interaction.NextFrame.HoveredItem;

            if (ctx.CurrentLayer->ZOrder >= hoveredItem.ZOrder)
            {
                hoveredItem.Id = id;
                hoveredItem.ZOrder = ctx.CurrentLayer->ZOrder;
            }
        }

        InterRes res{};
        res.Active = ctx.Interaction.ThisFrame.ActiveItemId == id;
        res.Focused = ctx.Interaction.FocusedItemId == id;
        res.Hovered = ctx.Interaction.ThisFrame.HoveredItem.Id == id;
        res.Pressed = res.Active && res.Hovered;
        res.Clicked = ctx.Interaction.ClickedItemId == id;
        return res;
    }

    bool IsItemPressedRepeat(size_t id, float delay, float interval)
    {
        HAX_ASSERT(g_Context != nullptr);

        if (!IsItemPressed(id))
            return false;

        float downDur = g_Context->Mouse.DownDuration;

        if (downDur == 0.f)
            return true;

        if (downDur <= delay)
            return false;

        float currentRel = downDur - delay;
        float prevRel = g_Context->Mouse.PrevDownDuration - delay;

        int currentStep = (int)(currentRel / interval);
        int prevStep = (int)(prevRel / interval);

        return currentStep > prevStep;
    }

    static void ProcessWndMsgs(Context& ctx)
    {
        Mouse& mouse = ctx.Mouse;

        WndMsg message;
        while (ctx.WndMsgQueue.Read(message))
        {
            uint32 msg = message.Msg;
            uint64 lParam = message.LParam;
            uint64 vk = message.WParam;

            switch (msg)
            {
                case WM_MOUSEMOVE:
                {
                    Vector2 newPos = Vector2((float)GET_X_LPARAM(message.LParam), (float)GET_Y_LPARAM(message.LParam));
                    mouse.DeltaPos += (newPos - mouse.Pos);
                    mouse.Pos = newPos;
                    break;
                }
                case WM_LBUTTONDBLCLK:
                case WM_LBUTTONDOWN:
                    mouse.LmbDown = true;
                    mouse.DownDuration = 0.f;
                    break;
                case WM_LBUTTONUP:
                    mouse.LmbDown = false;
                    mouse.DownDuration = -1.f;
                    break;
                case WM_MOUSEWHEEL:
                    mouse.DeltaWheel.Y = (float)GET_WHEEL_DELTA_WPARAM(message.WParam) / (float)WHEEL_DELTA;
                    break;
                case WM_MOUSEHWHEEL:
                    mouse.DeltaWheel.X = -(float)GET_WHEEL_DELTA_WPARAM(message.WParam) / (float)WHEEL_DELTA;
                    break;
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                {
                    const bool isDown = (message.Msg == WM_KEYDOWN || message.Msg == WM_SYSKEYDOWN);
                    if (isDown)
                        ctx.KeysPressedThisFrame.PushBack((uint8)vk);

                    if (vk < 256)
                    {
                        ctx.KeysData[vk].Down = isDown;

                        if (vk == VK_SHIFT)
                        {
                            ctx.KeysData[VK_LSHIFT].Down = (GetKeyState(VK_LSHIFT) & 0x8000) != 0;
                            ctx.KeysData[VK_RSHIFT].Down = (GetKeyState(VK_RSHIFT) & 0x8000) != 0;
                        }
                        else if (vk == VK_CONTROL)
                        {
                            ctx.KeysData[VK_LCONTROL].Down = (GetKeyState(VK_LCONTROL) & 0x8000) != 0;
                            ctx.KeysData[VK_RCONTROL].Down = (GetKeyState(VK_RCONTROL) & 0x8000) != 0;
                        }
                        else if (vk == VK_MENU)
                        {
                            ctx.KeysData[VK_LMENU].Down = (GetKeyState(VK_LMENU) & 0x8000) != 0;
                            ctx.KeysData[VK_RMENU].Down = (GetKeyState(VK_RMENU) & 0x8000) != 0;
                        }
                    }
                    break;
                }
                case WM_CHAR:
                    if (vk >= 32)
                        ctx.CharsPressedThisFrame.PushBack((char16)vk);
            }
        }
    }

    static void ResetLayer(Layer& layer)
    {
        layer.RenderItems.Clear();
        layer.RenderBatches.Clear();
        layer.CurrentLayout.Bounds = Rect();
        layer.CurrentLayout.CursorPos = Vector2();
        layer.CurrentContainer.Bounds = layer.CurrentClipRect = Rect{{}, g_Context->Viewport.Size};
        layer.CurrentRotation = Rotation{.Angle = 0.f, .Sin = 0.f, .Cos = 1.f};
    }

    static void UpdateViewportSize(Viewport& viewport)
    {
        RECT rect = {};
        ::GetClientRect((HWND)viewport.Hwnd, &rect);
        viewport.Size = Vector2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
    }

    static void UpdateViewportScale(Viewport& viewport)
    {
        HMONITOR handle = ::MonitorFromWindow((HWND)viewport.Hwnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        if (::GetMonitorInfo(handle, &mi))
        {
            int screenHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
            viewport.ScaleFactor = screenHeight / kScalingBaseHeight;
        }
    }

    static void UpdateWheelScroll(size_t id, ContainerState& state, float maxHeight, float maxOffset)
    {
        HAX_ASSERT(g_Context != nullptr);
        Context& ctx = *g_Context;

        auto& scroll = state.Scroll;

        if (ctx.Interaction.ThisFrame.HoveredContainer.Id == id && ctx.Mouse.DeltaWheel.Y != 0.f)
        {
            scroll.TargetOffsetY -= ctx.Mouse.DeltaWheel.Y * maxHeight * 0.2f;
            scroll.TargetOffsetY = Max(scroll.TargetOffsetY, 0.f);
            scroll.TargetOffsetY = Min(scroll.TargetOffsetY, maxOffset);
        }

        const float stiffness = 15.0f; 

        float delta = scroll.TargetOffsetY - scroll.Offset.Y;

        if (Abs(delta) < 0.1f)
        {
            scroll.Offset.Y = scroll.TargetOffsetY;
            return;
        }

        scroll.Offset.Y += delta * (1.0f - expf(-stiffness * ctx.Timer.DeltaTime));
    }

    static bool UpdateMouseIcon()
    {
        ::SetCursor((HCURSOR)GetMouseTexture());
        return true;
    }

    static void UpdateKeyboardInputs()
    {
        HAX_ASSERT(g_Context != nullptr);
        Context& ctx = *g_Context;

        for (uint32 vk = 0; vk < 256; ++vk)
        {
            float& downDuration = ctx.KeysData[vk].DownDuration;
            ctx.KeysData[vk].DownDurationPrev = downDuration;
            downDuration = ctx.KeysData[vk].Down ? (downDuration < 0.f ? 0.f : (downDuration + GetDeltaTime())) : -1.f;
        }
    }

    Vector<MsdfFontHandle> LoadMsdfFonts(Span<const Span<const uint8>> arfonts)
    {
        HAX_ASSERT(g_Context != nullptr);

        static bool s_Called;
        HAX_ASSERT(s_Called == false);

        uint32 maxWidth = 0, maxHeight = 0;

        Vector<MsdfFontHandle> result;
        result.Reserve(arfonts.Size());

        for (size_t i = 0; i < arfonts.Size(); ++i)
        {
            MsdfFont* font = MsdfFontDecoder(arfonts[i]).Decode();
            g_Context->MsdfFonts.PushBack(font);

            maxWidth = Max(maxWidth, font->Atlas.Width);
            maxHeight = Max(maxHeight, font->Atlas.Height);

            MsdfFontHandle handle = (MsdfFontHandle)(g_Context->MsdfFonts.Size() - 1);
            result.PushBack(handle);
        }

        Texture2D array = g_Context->Backend->CreateTexture(TextureFormat::R8G8B8A8_UNorm, nullptr, maxWidth, maxHeight, (int)arfonts.Size());

        for (uint32 i = 0; i < arfonts.Size(); ++i)
        {
            MsdfFont* font = g_Context->MsdfFonts[i];
            font->Atlas.IndexInArray = (float)(int)i;

            uint32 w, h;
            Vector<uint8> pixels;
            ConvertImageToBytes(font->Atlas.Data, pixels, w, h);
            HAX_ASSERT(w == font->Atlas.Width && h == font->Atlas.Height);

            RectU region = {0, 0, w, h};
            g_Context->Backend->UpdateTextureRegion(array, region, pixels.Data(), font->Atlas.Width * 4, i);
        }

        g_Context->AtlasArray = array;
        s_Called = true;

        return result;
    }

    static MsdfFont& GetMsdfFontFromHandle(MsdfFontHandle handle)
    {
        MsdfFont* font = g_Context->MsdfFonts[(size_t)handle];
        HAX_ASSERT(font != nullptr);
        return *font;
    }

    /*void LoadRange(FontHandle hFont, Pair<uint32, uint32> range, size_t fontH)
    {
        for (uint32 i = range.First; i < range.Second; ++i)
            FindOrLoadGlyph(hFont, i, fontH);
    }*/
}