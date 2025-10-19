#pragma once

class CMapParser
{
public:
    void Setup(std::string strMapName)
    {
        std::string strMapPath = std::filesystem::path(Config::m_fsMapPath / strMapName).string() + X(".tri");
        std::ifstream stream(strMapPath, std::ios::in | std::ios::binary);

        if (!stream.good())
            return;

        stream.seekg(0, std::ios::end);
        std::streamsize llFileSize = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::size_t uNumElements = llFileSize / sizeof(Triangle);
        m_vecTriangles.resize(uNumElements);

        if (!stream.read(reinterpret_cast<char*>(m_vecTriangles.data()), llFileSize))
            throw std::runtime_error(X("Failed to read file: ") + strMapName + X(".tri"));

        stream.close();

        m_pKDTree = BuildKDTree(m_vecTriangles);
        std::vector<Triangle>().swap(m_vecTriangles);

        m_bSetup = true;
        m_uMapNameHash = FNV1A::Hash(strMapName.c_str());
    }

    void VerifyMapNameHash(std::string strMapName)
    {
        if (strMapName.starts_with(X("de_")) || strMapName.starts_with(X("cs_")) || strMapName.starts_with(X("ar_")))
        {
            const auto path = Config::m_fsMapPath / (strMapName + X(".tri"));

            if (!std::filesystem::exists(path)) {
                m_bSetup = false;
                return;
            }

            if (FNV1A::Hash(strMapName.c_str()) != m_uMapNameHash)
            {
                Setup(strMapName);

                if (m_bSetup)
                    std::cout << X("succesfully parsed map: ") << strMapName << std::endl;
                else
                    std::cout << X("failed to parsed map: ") << strMapName << std::endl;
            }
        }
        else
        {
            m_bSetup = false;
        }
    }

    void Destroy() { m_pKDTree->DeleteKDTree(m_pKDTree); m_bSetup = false; }
    bool IsVisible(Vector vecRayOrigin, Vector vecRayEnd) { return !RayIntersectsKDTree(m_pKDTree, vecRayOrigin, vecRayEnd); }

    std::vector<Triangle> m_vecTriangles{};
    KDNode* m_pKDTree = nullptr;

    bool m_bSetup = false;
    FNV1A_t m_uMapNameHash = 0U;
};

inline CMapParser g_MapParser;