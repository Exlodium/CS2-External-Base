#pragma once

struct BoundingBox
{
    bool Intersects(const Vector& vecRayOrigin, const Vector& vecRayEnd) const
    {
        Vector vecDirection = (vecRayEnd - vecRayOrigin).NormalizeGarbage();

        float t1 = (m_vecMin.x - vecRayOrigin.x) / vecDirection.x;
        float t2 = (m_vecMax.x - vecRayOrigin.x) / vecDirection.x;
        float t3 = (m_vecMin.y - vecRayOrigin.y) / vecDirection.y;
        float t4 = (m_vecMax.y - vecRayOrigin.y) / vecDirection.y;
        float t5 = (m_vecMin.z - vecRayOrigin.z) / vecDirection.z;
        float t6 = (m_vecMax.z - vecRayOrigin.z) / vecDirection.z;

        float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
        float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

        if (tmax < 0)
            return false;

        if (tmin > tmax)
            return false;

        return true;
    }

    Vector m_vecMin{};
    Vector m_vecMax{};
};

struct Triangle
{
    bool Intersects(Vector vecRayOrigin, Vector vecRayEnd) const
    {
        constexpr float EPSILON = 0.0000001f;

        Vector edge1{}, edge2{}, h{}, s{}, q{};
        float a = 0.0f, f = 0.0f, u = 0.0f, v = 0.0f, t = 0.0f;

        edge1 = m_vecPoint2 - m_vecPoint1;
        edge2 = m_vecPoint3 - m_vecPoint1;

        h = (vecRayEnd - vecRayOrigin).CrossProduct(edge2);
        a = edge1.DotProduct(h);

        if (a > -EPSILON && a < EPSILON)
            return false;

        f = 1.0 / a;
        s = vecRayOrigin - m_vecPoint1;
        u = f * s.DotProduct(h);

        if (u < 0.0 || u > 1.0)
            return false;

        q = s.CrossProduct(edge1);
        v = f * (vecRayEnd - vecRayOrigin).DotProduct(q);

        if (v < 0.0 || u + v > 1.0)
            return false;

        t = f * edge2.DotProduct(q);

        if (t > EPSILON && t < 1.0)
            return true;

        return false;
    }

    Vector m_vecPoint1{};
    Vector m_vecPoint2{};
    Vector m_vecPoint3{};
};

struct KDNode
{
    void DeleteKDTree(KDNode*& pNode)
    {
        if (!pNode)
            return;

        DeleteKDTree(pNode->m_pLeftNode);
        DeleteKDTree(pNode->m_pRightNode);

        delete pNode;
    }

    BoundingBox m_BoundingBox{};
    std::vector<Triangle> m_vecTraingles{};
    KDNode* m_pLeftNode = nullptr;
    KDNode* m_pRightNode = nullptr;
    int nAxis = 0;
};

inline bool RayIntersectsKDTree(KDNode* pNode, const Vector& vecRayOrigin, const Vector& vecRayEnd)
{
    if (!pNode)
        return false;

    if (!pNode->m_BoundingBox.Intersects(vecRayOrigin, vecRayEnd))
        return false;

    if (pNode->m_vecTraingles.size() > 0)
    {
        bool bHit = false;
        for (const auto& tri : pNode->m_vecTraingles)
        {
            if (tri.Intersects(vecRayOrigin, vecRayEnd))
                bHit = true;
        }
        return bHit;
    }

    const bool bHitLeftNode = RayIntersectsKDTree(pNode->m_pLeftNode, vecRayOrigin, vecRayEnd);
    const bool bHitRightNode = RayIntersectsKDTree(pNode->m_pRightNode, vecRayOrigin, vecRayEnd);

    return (bHitLeftNode || bHitRightNode);
}

inline BoundingBox CalculateBoundingBox(const std::vector<Triangle>& vecTriangles)
{
    BoundingBox box{};
    box.m_vecMin = box.m_vecMax = vecTriangles[0].m_vecPoint1;

    for (const Triangle& vecTriangle : vecTriangles)
    {
        for (const Vector& vecPoint : { vecTriangle.m_vecPoint1, vecTriangle.m_vecPoint2, vecTriangle.m_vecPoint3 })
        {
            box.m_vecMin.x = std::min(box.m_vecMin.x, vecPoint.x);
            box.m_vecMin.y = std::min(box.m_vecMin.y, vecPoint.y);
            box.m_vecMin.z = std::min(box.m_vecMin.z, vecPoint.z);
            box.m_vecMax.x = std::max(box.m_vecMax.x, vecPoint.x);
            box.m_vecMax.y = std::max(box.m_vecMax.y, vecPoint.y);
            box.m_vecMax.z = std::max(box.m_vecMax.z, vecPoint.z);
        }
    }
    return box;
}

inline KDNode* BuildKDTree(std::vector<Triangle>& vecTriangles, int nDepth = 0)
{
    if (vecTriangles.empty())
        return nullptr;

    KDNode* pNode = new KDNode();
    pNode->m_BoundingBox = CalculateBoundingBox(vecTriangles);
    pNode->nAxis = nDepth % 3;

    if (vecTriangles.size() <= 3)
    {
        pNode->m_vecTraingles = vecTriangles;
        return pNode;
    }

    auto Comparator = [nAxis = pNode->nAxis](const Triangle& a, const Triangle& b)
        {
            float flCenterA = 0, flCenterB = 0;
            switch (nAxis)
            {
            case 0:
                flCenterA = (a.m_vecPoint1.x + a.m_vecPoint2.x + a.m_vecPoint3.x) / 3;
                flCenterB = (b.m_vecPoint1.x + b.m_vecPoint2.x + b.m_vecPoint3.x) / 3;
                break;
            case 1:
                flCenterA = (a.m_vecPoint1.y + a.m_vecPoint2.y + a.m_vecPoint3.y) / 3;
                flCenterB = (b.m_vecPoint1.y + b.m_vecPoint2.y + b.m_vecPoint3.y) / 3;
                break;
            case 2:
                flCenterA = (a.m_vecPoint1.z + a.m_vecPoint2.z + a.m_vecPoint3.z) / 3;
                flCenterB = (b.m_vecPoint1.z + b.m_vecPoint2.z + b.m_vecPoint3.z) / 3;
                break;
            }

            return flCenterA < flCenterB;
        };

    std::nth_element(vecTriangles.begin(), vecTriangles.begin() + vecTriangles.size() / 2, vecTriangles.end(), Comparator);

    std::vector<Triangle> vecLeftTriangles(vecTriangles.begin(), vecTriangles.begin() + vecTriangles.size() / 2);
    std::vector<Triangle> vecRightTriangles(vecTriangles.begin() + vecTriangles.size() / 2, vecTriangles.end());

    pNode->m_pLeftNode = BuildKDTree(vecLeftTriangles, nDepth + 1);
    pNode->m_pRightNode = BuildKDTree(vecRightTriangles, nDepth + 1);

    return pNode;
}