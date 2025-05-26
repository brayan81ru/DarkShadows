#pragma once
#include <string>
#include <vector>

namespace DSEngine {

    struct DSTHeader {
        char magic[4] = {'D', 'S', 'T', '\0'}; // "DST\0"
        uint32_t version = 1;                  // Format version
        uint32_t width;                        // Base width
        uint32_t height;                       // Base height
        uint32_t channels;                     // 1(R), 2(RG), 3(RGB), 4(RGBA)
        uint32_t mip_levels;                   // Number of mipmaps
        uint32_t flags;                        // Compression, srgb, etc.
        uint32_t data_offset;                  // Offset to pixel data
    };


    struct DSTMipInfo {
        uint32_t width;
        uint32_t height;
        uint32_t size;      // Uncompressed size
        uint32_t offset;    // Relative to data_offset
    };

    class DSTexture {
    public:
        ~DSTexture();

        // Creation
        static DSTexture* CreateFromFile(const std::string& path);
        static DSTexture* CreateEmpty(uint32_t width, uint32_t height, uint32_t channels);

        // Mipmap operations
        bool GenerateMipmaps();
        bool SetMipLevel(uint32_t level, const void* data);

        // Save/Load
        bool SaveToFile(const std::string& path);
        bool LoadFromFile(const std::string& path);

        // Accessors
        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }
        uint32_t GetChannels() const { return m_channels; }
        const void* GetPixels(uint32_t mip_level = 0) const;

    private:
        DSTexture(); // Private constructor

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_channels;
        std::vector<std::vector<uint8_t>> m_mipmaps;
    };
}// DSEngine


