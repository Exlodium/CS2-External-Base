#pragma once

class CMapParser
{
public:
    void Setup(std::string strMapName)
    {
		std::string strMapPath = std::filesystem::path(Config::m_fsMapPath / strMapName).string() + X(".rtx");
        std::ifstream stream(strMapPath, std::ios::in | std::ios::binary);

        stream.seekg(0, std::ios::end);
        std::streamsize llFileSize = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::size_t uNumElements = llFileSize / sizeof(Triangle);
        m_vecTriangles.resize(uNumElements);

        if (!stream.read(reinterpret_cast<char*>(m_vecTriangles.data()), llFileSize))
            throw std::runtime_error(X("Failed to read file: ") + strMapName + X(".rtx"));

        stream.close();

        m_pKDTree = BuildKDTree(m_vecTriangles);
        std::vector<Triangle>().swap(m_vecTriangles);

        m_bSetup = true;
    }

    void Destroy() { m_pKDTree->DeleteKDTree(m_pKDTree); m_bSetup = false; }
    bool IsVisible(Vector vecRayOrigin, Vector vecRayEnd) { return !RayIntersectsKDTree(m_pKDTree, vecRayOrigin, vecRayEnd); }

    std::vector<Triangle> m_vecTriangles{};
    KDNode* m_pKDTree = nullptr;

	bool m_bSetup = false;
};

inline CMapParser g_MapParser;