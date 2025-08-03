#pragma once

#define ADD_SCHEMA(TYPE, NAME, VARIABLE, ADDITIONAL) \
[[nodiscard]] __forceinline TYPE NAME() noexcept { \
    static std::uintptr_t uOffset = SchemaSystem::m_mapSchemaOffsets[FNV1A::Hash(VARIABLE)]; \
    return g_Memory.ReadMemory<TYPE>(reinterpret_cast<std::uintptr_t>(this) + uOffset + ADDITIONAL); \
}

#define ADD_OFFSET(TYPE, NAME, OFFSET) \
[[nodiscard]] __forceinline TYPE NAME() noexcept { \
	static std::uintptr_t uOffset = OFFSET; \
	return g_Memory.ReadMemory<TYPE>(reinterpret_cast<std::uintptr_t>(this) + uOffset); \
}

#define SCHEMA_OFFSET(TYPE, NAME, FIELD, ADDITIONAL) ADD_SCHEMA(TYPE, NAME, FIELD, ADDITIONAL)
#define SCHEMA(TYPE, NAME, FIELD) ADD_SCHEMA(TYPE, NAME, FIELD, 0U)
#define OFFSET(TYPE, NAME, OFFSET) ADD_OFFSET(TYPE, NAME, OFFSET)

namespace SchemaSystem
{
	bool Setup();

	inline std::unordered_map<FNV1A_t, std::uint32_t> m_mapSchemaOffsets;
}