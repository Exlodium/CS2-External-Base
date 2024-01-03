#pragma once

#include <cstdint>
#include <limits>
#include <cstdint>

class CUtlMemoryPool
{
public:
	int Count() const
	{
		return m_nBlocksAllocated;
	}

	int PeakCount() const
	{
		return m_nPeakAlloc;
	}

	int BlockSize() const
	{
		return m_nBlockSize;
	}
protected:
	class CBlob
	{
	public:
		CBlob* m_pPrev;
		CBlob* m_pNext;
		int m_nNumBytes;
		char m_Data[1];
		char m_Padding[3];
	};

	int m_nBlockSize;
	int m_nBlocksPerBlob;
	int m_nGrowMode;
	int m_nBlocksAllocated;
	int m_nPeakAlloc;

	unsigned short m_nAlignment;
	unsigned short m_nNumBlobs;

	void* m_pHeadOfFreeList;
	void* m_pAllocOwner;
	CBlob m_BlobHead;
};

using UtlTSHashHandle_t = std::uintptr_t;

inline unsigned HashIntConventional(const int n)
{
	unsigned hash = 0xAAAAAAAA + (n & 0xFF);
	hash = (hash << 5) + hash + ((n >> 8) & 0xFF);
	hash = (hash << 5) + hash + ((n >> 16) & 0xFF);
	hash = (hash << 5) + hash + ((n >> 24) & 0xFF);

	return hash;
}

template< int nBucketCount, class tKey = std::uintptr_t >
class CUtlTSHashGenericHash
{
public:
	static int Hash(const tKey& Key, int nBucketMask)
	{
		int nHash = HashIntConventional(std::uintptr_t(Key));
		if (nBucketCount <= UINT16_MAX)
		{
			nHash ^= (nHash >> 16);
		}

		if (nBucketCount <= UINT8_MAX)
		{
			nHash ^= (nHash >> 8);
		}

		return (nHash & nBucketMask);
	}

	static bool Compare(const tKey& lhs, const tKey& rhs)
	{
		return lhs == rhs;
	}
};

template< class tElement, int nBucketCount, class tKey = std::uintptr_t, class tHashFuncs = CUtlTSHashGenericHash< nBucketCount, tKey >, int nAlignment = 0 >
class CUtlTSHash
{
	static constexpr int nBucketMask = nBucketCount - 1;
public:
	static constexpr UtlTSHashHandle_t InvalidHandle()
	{
		return static_cast<UtlTSHashHandle_t>(0);
	}

	UtlTSHashHandle_t Find(tKey uiKey)
	{
		int iBucket = tHashFuncs::Hash(uiKey, nBucketCount);
		const HashBucket_t& hashBucket = m_aBuckets[iBucket];
		const UtlTSHashHandle_t hHash = Find(uiKey, hashBucket.m_pFirst, nullptr);
		return hHash ? hHash : Find(uiKey, hashBucket.m_pFirstUncommited, hashBucket.m_pFirst);
	}

	int Count() const
	{
		return m_EntryMemory.Count();
	}

	int GetElements(int nFirstElement, int nCount, UtlTSHashHandle_t* pHandles) const
	{
		int nIndex = 0;
		for (int nBucketIndex = 0; nBucketIndex < nBucketCount; nBucketIndex++)
		{
			const HashBucket_t& hashBucket = m_aBuckets[nBucketIndex];

			HashFixedData_t* pElement = hashBucket.m_pFirstUncommited;

			for (; pElement; pElement = pElement->GetNext())
			{
				if (--nFirstElement >= 0)
					continue;

				pHandles[nIndex++] = reinterpret_cast<UtlTSHashHandle_t>(pElement);

				if (nIndex >= nCount)
					return nIndex;
			}
		}

		return nIndex;
	}

	tElement Element(UtlTSHashHandle_t hHash)
	{
		return  ((HashFixedData_t*)(hHash))->GetData();
	}

	const tElement& Element(UtlTSHashHandle_t hHash) const
	{
		return reinterpret_cast<HashFixedData_t*>(hHash)->m_Data;
	}

	tElement& operator[](UtlTSHashHandle_t hHash)
	{
		return reinterpret_cast<HashFixedData_t*>(hHash)->m_Data;
	}

	const tElement& operator[](UtlTSHashHandle_t hHash) const
	{
		return reinterpret_cast<HashFixedData_t*>(hHash)->m_Data;
	}

	tKey GetID(UtlTSHashHandle_t hHash) const
	{
		return reinterpret_cast<HashFixedData_t*>(hHash)->m_uiKey;
	}

private:
	template< typename tData >
	struct HashFixedDataInternal_t
	{
		tKey m_uiKey;
		HashFixedDataInternal_t< tData >* m_pNext;
		tData m_Data;

		tData GetData() {
			return g_Memory.Read< tData >(std::uintptr_t(this) + 0x10);
		}

		HashFixedDataInternal_t< tData >* GetNext() {
			return g_Memory.Read< HashFixedDataInternal_t< tData >* >(std::uintptr_t(this) + 0x20);
		}
	};

	using HashFixedData_t = HashFixedDataInternal_t< tElement >;

	struct HashBucket_t
	{
	private:
		[[ maybe_unused ]]
		std::byte m_pad0[0x18];
	public:
		HashFixedData_t* m_pFirst;
		HashFixedData_t* m_pFirstUncommited;
	};

	UtlTSHashHandle_t Find(tKey uiKey, HashFixedData_t* pFirstElement, HashFixedData_t* pLastElement)
	{
		for (HashFixedData_t* pElement = pFirstElement; pElement != pLastElement; pElement = pElement->m_pNext)
		{
			if (tHashFuncs::Compare(pElement->m_uiKey, uiKey))
				return reinterpret_cast<UtlTSHashHandle_t>(pElement);
		}

		return InvalidHandle();
	}

	CUtlMemoryPool m_EntryMemory;
	HashBucket_t m_aBuckets[nBucketCount];
	bool m_bNeedsCommit;
};