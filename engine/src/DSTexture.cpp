#include "DSTexture.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>

// STB Image implementation
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "../third_party/stb/stb_image.h"

// STB Image Resize implementation
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../third_party/stb/stb_image_resize.h"

namespace DSEngine {
    DSTexture::DSTexture()
        : m_format(Format::UNKNOWN), m_flags(0) {}

    DSTexture::~DSTexture() {
        // Automatic cleanup via vector destructors
    }

    // Static creator methods
    DSTexture* DSTexture::CreateFromFile(const std::string& path, bool flipVertically) {
        DSTexture* texture = new DSTexture();
        if (!texture->LoadFromFile(path)) {
            delete texture;
            return nullptr;
        }
        return texture;
    }

    DSTexture* DSTexture::CreateEmpty(uint32_t width, uint32_t height, Format format) {
        DSTexture* texture = new DSTexture();
        texture->m_format = format;

        MipLevel baseLevel;
        baseLevel.width = width;
        baseLevel.height = height;
        baseLevel.data.resize(width * height * GetPixelSize(format));

        texture->m_mipmaps.push_back(std::move(baseLevel));
        return texture;
    }

    DSTexture* DSTexture::CreateFromMemory(const uint8_t* data, uint32_t width, uint32_t height, Format format) {
        DSTexture* texture = new DSTexture();
        texture->m_format = format;

        MipLevel baseLevel;
        baseLevel.width = width;
        baseLevel.height = height;
        const size_t dataSize = width * height * GetPixelSize(format);
        baseLevel.data.assign(data, data + dataSize);

        texture->m_mipmaps.push_back(std::move(baseLevel));
        return texture;
    }

    // File operations
    bool DSTexture::SaveToFile(const std::string& path) const {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        // Prepare header
        DSTHeader header;
        header.width = GetWidth();
        header.height = GetHeight();
        header.format = static_cast<uint16_t>(m_format);
        header.mipLevels = GetMipLevels();

        // Write header
        file.write(reinterpret_cast<const char*>(&header), sizeof(DSTHeader));

        // Calculate data offset (header + mip info array)
        uint32_t dataOffset = sizeof(DSTHeader) + (header.mipLevels * sizeof(DSTMipInfo));

        // Write mipmap info and accumulate data offset
        for (const auto& mip : m_mipmaps) {
            DSTMipInfo info;
            info.width = mip.width;
            info.height = mip.height;
            info.dataSize = static_cast<uint32_t>(mip.data.size());
            info.dataOffset = dataOffset;

            file.write(reinterpret_cast<const char*>(&info), sizeof(DSTMipInfo));
            dataOffset += info.dataSize;
        }

        // Write pixel data
        for (const auto& mip : m_mipmaps) {
            file.write(reinterpret_cast<const char*>(mip.data.data()), mip.data.size());
        }

        return true;
    }

    bool DSTexture::LoadFromFile(const std::string& path) {
        // Try loading as DST first
        std::ifstream file(path, std::ios::binary);
        if (file.is_open()) {
            DSTHeader header;
            file.read(reinterpret_cast<char*>(&header), sizeof(DSTHeader));

            // Verify magic number
            if (header.magic[0] != 'D' || header.magic[1] != 'S' || header.magic[2] != 'T') {
                file.close();
                return LoadFromSTB(path, true);
            }

            m_format = static_cast<Format>(header.format);
            m_flags = header.flags;

            // Read mipmap info
            std::vector<DSTMipInfo> mipInfos(header.mipLevels);
            file.read(reinterpret_cast<char*>(mipInfos.data()), header.mipLevels * sizeof(DSTMipInfo));

            // Read mipmap data
            m_mipmaps.resize(header.mipLevels);
            for (uint32_t i = 0; i < header.mipLevels; ++i) {
                MipLevel& mip = m_mipmaps[i];
                mip.width = mipInfos[i].width;
                mip.height = mipInfos[i].height;
                mip.data.resize(mipInfos[i].dataSize);

                file.seekg(mipInfos[i].dataOffset);
                file.read(reinterpret_cast<char*>(mip.data.data()), mip.data.size());
            }

            return true;
        }

        // Fall back to STB loader
        return LoadFromSTB(path, true);
    }

    // Mipmap operations
    bool DSTexture::GenerateMipmaps() {
        if (m_mipmaps.empty()) return false;

        // Clear existing mipmaps (except base level)
        if (m_mipmaps.size() > 1) {
            m_mipmaps.erase(m_mipmaps.begin() + 1, m_mipmaps.end());
        }

        const MipLevel& base = m_mipmaps[0];
        uint32_t width = base.width;
        uint32_t height = base.height;

        while (width > 1 || height > 1) {
            width = std::max(1u, width / 2);
            height = std::max(1u, height / 2);

            MipLevel newLevel;
            newLevel.width = width;
            newLevel.height = height;
            newLevel.data.resize(width * height * GetPixelSize(m_format));

            const MipLevel& prevLevel = m_mipmaps.back();
            stbir_resize_uint8(
                prevLevel.data.data(), prevLevel.width, prevLevel.height, 0,
                newLevel.data.data(), width, height, 0,
                GetChannelCount(m_format)
            );

            m_mipmaps.push_back(std::move(newLevel));
        }

        return true;
    }

    bool DSTexture::SetMipLevel(uint32_t level, const void* data, size_t size) {
        if (!ValidateMipLevel(level) || !data) return false;

        MipLevel& mip = m_mipmaps[level];
        if (size != mip.width * mip.height * GetPixelSize(m_format)) {
            return false;
        }

        std::memcpy(mip.data.data(), data, size);
        return true;
    }

    bool DSTexture::RemoveMipmaps() {
        if (m_mipmaps.size() <= 1) return false;
        m_mipmaps.resize(1);
        return true;
    }

    // Accessors
    uint32_t DSTexture::GetWidth(uint32_t mipLevel) const {
        return ValidateMipLevel(mipLevel) ? m_mipmaps[mipLevel].width : 0;
    }

    uint32_t DSTexture::GetHeight(uint32_t mipLevel) const {
        return ValidateMipLevel(mipLevel) ? m_mipmaps[mipLevel].height : 0;
    }

    const uint8_t* DSTexture::GetPixels(uint32_t mipLevel) const {
        return ValidateMipLevel(mipLevel) ? m_mipmaps[mipLevel].data.data() : nullptr;
    }

    size_t DSTexture::GetPixelDataSize(uint32_t mipLevel) const {
        return ValidateMipLevel(mipLevel) ? m_mipmaps[mipLevel].data.size() : 0;
    }

    // Static format helpers
    uint32_t DSTexture::GetChannelCount(Format format) {
        switch (format) {
            case Format::R8: return 1;
            case Format::RG8: return 2;
            case Format::RGB8: return 3;
            case Format::RGBA8: return 4;
            default: return 0;
        }
    }

    size_t DSTexture::GetPixelSize(Format format) {
        switch (format) {
            case Format::R8: return 1;
            case Format::RG8: return 2;
            case Format::RGB8: return 3;
            case Format::RGBA8: return 4;
            default: return 0;
        }
    }

    // Private methods
    bool DSTexture::ValidateMipLevel(uint32_t level) const {
        return level < m_mipmaps.size();
    }

    bool DSTexture::LoadFromSTB(const std::string& path, bool flipVertically) {
        stbi_set_flip_vertically_on_load(flipVertically);

        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data) return false;

        // Determine format
        switch (channels) {
            case 1: m_format = Format::R8; break;
            case 2: m_format = Format::RG8; break;
            case 3: m_format = Format::RGB8; break;
            case 4: m_format = Format::RGBA8; break;
            default:
                stbi_image_free(data);
                return false;
        }

        // Create base mip level
        MipLevel baseLevel;
        baseLevel.width = width;
        baseLevel.height = height;
        const size_t dataSize = width * height * channels;
        baseLevel.data.assign(data, data + dataSize);
        m_mipmaps.push_back(std::move(baseLevel));

        stbi_image_free(data);
        return true;
    }
}