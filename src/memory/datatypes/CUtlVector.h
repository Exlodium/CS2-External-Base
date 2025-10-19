#pragma once

class CUtlVectorSimple
{
public:
	int m_nSize;
	MEM_PAD(0x4);
	std::uintptr_t m_pData;
};

class C_NetworkUtlVectorBaseSimple
{
public:
	int m_nSize;
	std::uintptr_t m_pData;
	MEM_PAD(0x8);
};