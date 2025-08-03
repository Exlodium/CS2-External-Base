#include "../Includes.h"

bool Config::Setup(std::string_view szDefaultFileName)
{
	if (!std::filesystem::is_directory(m_fsConfigPath))
	{
		std::filesystem::remove(m_fsConfigPath);
		if (!std::filesystem::create_directories(m_fsConfigPath))
			return false;
	}

	if (!std::filesystem::is_directory(m_fsMapPath))
	{
		std::filesystem::remove(m_fsMapPath);
		if (!std::filesystem::create_directories(m_fsMapPath))
			return false;
	}

	if (!Save(szDefaultFileName))
		return false;

	if (!Load(szDefaultFileName))
		return false;

	Refresh();

	return true;
}

bool Config::Save(std::string_view szFileName)
{
	std::filesystem::path fsFilePath(szFileName);
	if (fsFilePath.extension() != X(".json"))
		fsFilePath.replace_extension(X(".json"));

	const std::string szFile = std::filesystem::path(m_fsConfigPath / fsFilePath).string();
	nlohmann::json config = { };

	try
	{
		for (auto& variable : m_vecVariables)
		{
			nlohmann::json entry = { };

			entry[X("name-id")] = variable.uNameHash;
			entry[X("type-id")] = variable.uTypeHash;

			switch (variable.uTypeHash)
			{
				case FNV1A::HashConst("int"):
				{
					entry[X("value")] = variable.Get<int>();
					break;
				}
				case FNV1A::HashConst("float"):
				{
					entry[X("value")] = variable.Get<float>();
					break;
				}
				case FNV1A::HashConst("bool"):
				{
					entry[X("value")] = variable.Get<bool>();
					break;
				}
				case FNV1A::HashConst("std::string"):
				{
					entry[X("value")] = variable.Get<std::string>();
					break;
				}
				case FNV1A::HashConst("Color"):
				{
					const auto& colVariable = variable.Get<Color>();

					nlohmann::json sub = { };

					sub.push_back(colVariable.Get<COLOR_R>());
					sub.push_back(colVariable.Get<COLOR_G>());
					sub.push_back(colVariable.Get<COLOR_B>());
					sub.push_back(colVariable.Get<COLOR_A>());

					entry[X("value")] = sub.dump();
					break;
				}
				case FNV1A::HashConst("KeyBind_t"):
				{
					const auto& keybindVariable = variable.Get<KeyBind_t>();

					nlohmann::json sub = { };

					sub.push_back(keybindVariable.m_uKey);
					sub.push_back(keybindVariable.m_uMode);

					entry[X("value")] = sub.dump();
					break;
				}
				case FNV1A::HashConst("std::vector<bool>"):
				{
					const auto& vecBools = variable.Get<std::vector<bool>>();

					nlohmann::json sub = { };

					for (const auto&& bValue : vecBools)
						sub.push_back(static_cast<bool>(bValue));

					entry[X("value")] = sub.dump();
					break;
				}
				case FNV1A::HashConst("std::vector<int>"):
				{
					const auto& vecInts = variable.Get<std::vector<int>>();

					nlohmann::json sub = { };

					for (const auto& iValue : vecInts)
						sub.push_back(iValue);

					entry[X("value")] = sub.dump();
					break;
				}
				case FNV1A::HashConst("std::vector<float>"):
				{
					const auto& vecFloats = variable.Get<std::vector<float>>();

					nlohmann::json sub = { };

					for (const auto& flValue : vecFloats)
						sub.push_back(flValue);

					entry[X("value")] = sub.dump();
					break;
				}
				case FNV1A::HashConst("std::vector<Color>"):
				{
					const auto vecColors = variable.Get<std::vector<Color>>();

					nlohmann::json sub = { };

					for (const auto& colValue : vecColors)
						sub.push_back(colValue.GetHex());

					entry[X("value")] = sub.dump();
					break;
				}
				default:
					break;
			}

			config.push_back(entry);
		}
	}
	catch (const nlohmann::detail::exception& ex)
	{
		std::cout << X("json save failed: ") << ex.what() << std::endl;
		return false;
	}

	std::ofstream ofsOutFile(szFile, std::ios::out | std::ios::trunc);
	if (!ofsOutFile.good())
		return false;

	try
	{
		ofsOutFile << config.dump(4);
		ofsOutFile.close();
	}
	catch (std::ofstream::failure& ex)
	{
		std::cout << X("failed to save configuration: ") << ex.what() << std::endl;
		return false;
	}

	std::cout << X("saved configuration: ") << szFile << std::endl;
	return true;
}

bool Config::Load(std::string_view szFileName)
{
	const std::string szFile = std::filesystem::path(m_fsConfigPath / szFileName).string();
	nlohmann::json config = { };

	std::ifstream ifsInputFile(szFile, std::ios::in);

	if (!ifsInputFile.good())
		return false;

	try
	{
		config = nlohmann::json::parse(ifsInputFile, nullptr, false);
		if (config.is_discarded())
			return false;

		ifsInputFile.close();
	}
	catch (std::ifstream::failure& ex)
	{
		std::cout << X("failed to load configuration: ") << ex.what() << std::endl;
		return false;
	}

	try
	{
		for (const auto& variable : config)
		{
			const std::size_t nIndex = GetVariableIndex(variable[X("name-id")].get<FNV1A_t>());
			if (nIndex == C_INVALID_VARIABLE)
				continue;

			auto& entry = m_vecVariables.at(nIndex);
			switch (variable[X("type-id")].get<FNV1A_t>())
			{
				case FNV1A::HashConst("bool"):
				{
					entry.Set<bool>(variable[X("value")].get<bool>());
					break;
				}
				case FNV1A::HashConst("float"):
				{
					entry.Set<float>(variable[X("value")].get<float>());
					break;
				}
				case FNV1A::HashConst("int"):
				{
					entry.Set<int>(variable[X("value")].get<int>());
					break;
				}
				case FNV1A::HashConst("std::string"):
				{
					entry.Set<std::string>(variable[X("value")].get<std::string>());
					break;
				}
				case FNV1A::HashConst("Color"):
				{
					const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());

					entry.Set<Color>(Color(
						vector.at(0).get<std::uint8_t>(),
						vector.at(1).get<std::uint8_t>(),
						vector.at(2).get<std::uint8_t>(),
						vector.at(3).get<std::uint8_t>()
					));

					break;
				}
				case FNV1A::HashConst("KeyBind_t"):
				{
					const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());

					entry.Set<KeyBind_t>(KeyBind_t(
						vector.at(0).get<unsigned int>(),
						vector.at(1).get<unsigned int>()
					));

					break;
				}
				case FNV1A::HashConst("std::vector<bool>"):
				{
					const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());
					auto& vecBools = entry.Get<std::vector<bool>>();

					for (std::size_t i = 0U; i < vector.size(); i++)
					{
						if (i < vecBools.size())
							vecBools.at(i) = vector.at(i).get<bool>();
					}

					break;
				}
				case FNV1A::HashConst("std::vector<int>"):
				{
					const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());
					auto& vecInts = entry.Get<std::vector<int>>();

					for (std::size_t i = 0U; i < vector.size(); i++)
					{
						if (i < vecInts.size())
							vecInts.at(i) = vector.at(i).get<int>();
					}

					break;
				}
				case FNV1A::HashConst("std::vector<float>"):
				{
					const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());
					auto& vecFloats = entry.Get<std::vector<float>>();

					for (std::size_t i = 0U; i < vector.size(); i++)
					{
						if (i < vecFloats.size())
							vecFloats.at(i) = vector.at(i).get<float>();
					}

					break;
				}
				case FNV1A::HashConst("std::vector<Color>"):
				{
					const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());
					auto& vecColors = entry.Get<std::vector<Color>>();

					for (std::size_t i = 0U; i < vector.size(); i++)
					{
						if (i < vecColors.size())
							vecColors.at(i) = Color::FromHex(vector.at(i).get<unsigned>());
					}

					break;
				}
				default:
					break;
			}
		}
	}
	catch (const nlohmann::detail::exception& ex)
	{
		std::cout << X("json load failed: ") << ex.what() << std::endl;
		return false;
	}

	std::cout << X("loaded configuration: ") << szFile << std::endl;
	return true;
}

void Config::Remove(std::string_view szFileName)
{
	const std::string szFile = std::filesystem::path(m_fsConfigPath / szFileName).string();

	if (szFileName.compare(X("Default.json")) == 0)
		return;

	if (std::filesystem::remove(szFile))
		std::cout << X("removed configuration: ") << szFile << std::endl;
}

void Config::Refresh()
{
	m_vecFileNames.clear();

	for (const auto& it : std::filesystem::directory_iterator(m_fsConfigPath))
	{
		if (it.path().filename().extension() == X(".json"))
		{
			std::cout << X("found configuration file: ") << it.path().filename().string() << std::endl;
			m_vecFileNames.push_back(it.path().filename().string());
		}
	}
}

std::size_t Config::GetVariableIndex(const FNV1A_t uNameHash)
{
	for (std::size_t i = 0U; i < m_vecVariables.size(); i++)
	{
		if (m_vecVariables.at(i).uNameHash == uNameHash)
			return i;
	}

	return C_INVALID_VARIABLE;
}

std::filesystem::path Config::GetWorkingPath()
{
	std::filesystem::path fsWorkingPath;
	if (PWSTR pszPathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0UL, nullptr, &pszPathToDocuments)))
	{
		fsWorkingPath.assign(pszPathToDocuments);
		fsWorkingPath.append(X("External Base"));
		CoTaskMemFree(pszPathToDocuments);
	}

	return fsWorkingPath;
}