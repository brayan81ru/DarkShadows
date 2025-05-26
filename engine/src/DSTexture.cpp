#include "DSTexture.h"
#include <fstream>
#include <algorithm>
#include "DSTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb/stb_image.h"
#include "../third_party/stb/stb_image_resize2.h"

namespace DSEngine {
    DSTexture* DSTexture::CreateFromFile(const std::string& path) {
        int w, h, c;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 0);
        if (!data) return nullptr;

        auto* tex = new DSTexture();
        tex->m_width = w;
        tex->m_height = h;
        tex->m_channels = c;

        // Store base mip level
        size_t size = w * h * c;
        tex->m_mipmaps.emplace_back(data, data + size);

        stbi_image_free(data);
        return tex;
    }

    bool DSTexture::GenerateMipmaps() {

        if (m_mipmaps.empty()) return false;

        int w = m_width;
        int h = m_height;

        while (w > 1 || h > 1) {
            w = std::max(1, w / 2);
            h = std::max(1, h / 2);

            std::vector<uint8_t> mip_data(w * h * m_channels);
            stbir_resize_uint8_srgb(
                m_mipmaps.back().data(), m_width, m_height, 0,
                mip_data.data(), w, h, 0, STBIR_RGBA);

            m_mipmaps.push_back(std::move(mip_data));
        }

        return true;
    }

    bool DSTexture::SaveToFile(const std::string& path) {
        std::ofstream file(path, std::ios::binary);
        if (!file) return false;

        DSTHeader header;
        header.width = m_width;
        header.height = m_height;
        header.channels = m_channels;
        header.mip_levels = static_cast<uint32_t>(m_mipmaps.size());
        header.data_offset = sizeof(DSTHeader) + (header.mip_levels * sizeof(DSTMipInfo));

        // Write header
        file.write(reinterpret_cast<char*>(&header), sizeof(header));

        // Write mip info
        uint32_t offset = 0;
        for (size_t i = 0; i < m_mipmaps.size(); i++) {
            DSTMipInfo info;
            info.width = std::max(1u, m_width >> i);
            info.height = std::max(1u, m_height >> i);
            info.size = static_cast<uint32_t>(m_mipmaps[i].size());
            info.offset = offset;

            file.write(reinterpret_cast<char*>(&info), sizeof(info));
            offset += info.size;
        }

        // Write pixel data
        for (const auto& mip : m_mipmaps) {
            file.write(reinterpret_cast<const char*>(mip.data()), mip.size());
        }

        return true;
    }
}