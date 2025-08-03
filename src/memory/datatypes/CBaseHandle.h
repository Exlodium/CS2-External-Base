#pragma once

#define INVALID_EHANDLE_INDEX 0xFFFFFFFF
#define ENT_ENTRY_MASK 0x7FFF
#define NUM_SERIAL_NUM_SHIFT_BITS 15
// @source: https://developer.valvesoftware.com/wiki/Entity_limit#Source_2_limits
#define ENT_MAX_NETWORKED_ENTRY 16384

class C_BaseEntity;
class CBaseHandle
{
public:
	CBaseHandle() noexcept :
		m_nIndex(INVALID_EHANDLE_INDEX) { }

	CBaseHandle(const int nEntry, const int nSerial) noexcept
	{
		m_nIndex = nEntry | (nSerial << NUM_SERIAL_NUM_SHIFT_BITS);
	}

	bool operator!=(const CBaseHandle& other) const noexcept
	{
		return m_nIndex != other.m_nIndex;
	}

	bool operator==(const CBaseHandle& other) const noexcept
	{
		return m_nIndex == other.m_nIndex;
	}

	bool operator<(const CBaseHandle& other) const noexcept
	{
		return m_nIndex < other.m_nIndex;
	}

	[[nodiscard]] bool IsValid() const noexcept
	{
		return m_nIndex != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int GetEntryIndex() const noexcept
	{
		return static_cast<int>(m_nIndex & ENT_ENTRY_MASK);
	}

	[[nodiscard]] int GetSerialNumber() const noexcept
	{
		return static_cast<int>(m_nIndex >> NUM_SERIAL_NUM_SHIFT_BITS);
	}

	[[nodiscard]] C_BaseEntity* Get() const;
private:
	std::uint32_t m_nIndex;
};

template <typename T>
class CHandle : public CBaseHandle
{
public:
	auto Get() const
	{
		return reinterpret_cast<T*>(CBaseHandle::Get());
	}
};