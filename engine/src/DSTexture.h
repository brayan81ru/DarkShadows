#pragma once
#include <vector>
#include <string>
#include <cstdint>
namespace DSEngine {
    class DSTexture {
    public:
        // Texture formats
        enum class Format {
            UNKNOWN = 0,
            R8,
            RG8,
            RGB8,
            RGBA8,
            // Add more formats as needed
        };

        // Construction/destruction
        DSTexture();
        ~DSTexture();

        // Creation methods
        static DSTexture* CreateFromFile(const std::string& path, bool flipVertically = true);
        static DSTexture* CreateEmpty(uint32_t width, uint32_t height, Format format);
        static DSTexture* CreateFromMemory(const uint8_t* data, uint32_t width, uint32_t height, Format format);

        // Save/Load operations
        bool SaveToFile(const std::string& path) const;
        bool LoadFromFile(const std::string& path);

        // Mipmap operations
        bool GenerateMipmaps();
        bool SetMipLevel(uint32_t level, const void* data, size_t size);
        bool RemoveMipmaps();

        // Accessors
        uint32_t GetWidth(uint32_t mipLevel = 0) const;
        uint32_t GetHeight(uint32_t mipLevel = 0) const;
        Format GetFormat() const { return m_format; }
        uint32_t GetMipLevels() const { return static_cast<uint32_t>(m_mipmaps.size()); }
        const uint8_t* GetPixels(uint32_t mipLevel = 0) const;
        size_t GetPixelDataSize(uint32_t mipLevel = 0) const;

        // Format conversion
        static uint32_t GetChannelCount(Format format);
        static size_t GetPixelSize(Format format);

    private:
        struct MipLevel {
            uint32_t width;
            uint32_t height;
            std::vector<uint8_t> data;
        };

        // DST file format structures
#pragma pack(push, 1)
        struct DSTHeader {
            char magic[4] = {'D', 'S', 'T', '\0'}; // "DST\0"
            uint16_t version = 1;                   // Format version
            uint16_t format;                        // Format enum value
            uint32_t width;                         // Base width
            uint32_t height;                        // Base height
            uint32_t mipLevels;                     // Number of mipmaps
            uint32_t flags = 0;                     // Additional flags
        };

        struct DSTMipInfo {
            uint32_t width;
            uint32_t height;
            uint32_t dataSize;
            uint32_t dataOffset;
        };
#pragma pack(pop)

        // Texture data
        Format m_format;
        std::vector<MipLevel> m_mipmaps;
        uint32_t m_flags;

        // Private methods
        bool ValidateMipLevel(uint32_t level) const;
        bool LoadFromSTB(const std::string& path, bool flipVertically);
    };
}