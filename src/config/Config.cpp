#include "../Precompiled.h"

bool Config::Setup(std::string_view szDefaultFileName)
{
	if (!std::filesystem::is_directory(fsPath))
	{
		std::filesystem::remove(fsPath);
		if (!std::filesystem::create_directories(fsPath))
			return false;
	}

	// create default config
	if (!Save(szDefaultFileName))
		return false;

	// load default config
	if (!Load(szDefaultFileName))
		return false;

	// refresh file lists
	Refresh();

	return true;
}

bool Config::Save(std::string_view szFileName)
{
	// check for extension if it is not our replace it
	std::filesystem::path fsFilePath(szFileName);
	if (fsFilePath.extension() != X(".json"))
		fsFilePath.replace_extension(X(".json"));

	// get utf-8 full path to config
	const std::string szFile = std::filesystem::path(fsPath / fsFilePath).string();
	nlohmann::json config = { };

	try
	{
		for (auto& variable : vecVariables)
		{
			nlohmann::json entry = { };

			// save hashes to compare it later
			entry[X("name-id")] = variable.uNameHash;
			entry[X("type-id")] = variable.uTypeHash;

			// get current variable
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

				// store RGBA as sub-node
				nlohmann::json sub = { };

				// fill node with all color values
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

				// store keybinds as a subnode
				nlohmann::json sub = { };

				// fill node with all key values
				sub.push_back(keybindVariable.m_iKey);
				sub.push_back(keybindVariable.m_iMode);

				entry[X("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<bool>"):
			{
				const auto& vecBools = variable.Get<std::vector<bool>>();

				// store vector values as sub-node
				nlohmann::json sub = { };

				// fill node with all vector values
				for (const auto&& bValue : vecBools)
					sub.push_back(static_cast<bool>(bValue));

				entry[X("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<int>"):
			{
				const auto& vecInts = variable.Get<std::vector<int>>();

				// store vector values as sub-node
				nlohmann::json sub = { };

				// fill node with all vector values
				for (const auto& iValue : vecInts)
					sub.push_back(iValue);

				entry[X("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<float>"):
			{
				const auto& vecFloats = variable.Get<std::vector<float>>();

				// store vector values as sub-node
				nlohmann::json sub = { };

				// fill node with all vector values
				for (const auto& flValue : vecFloats)
					sub.push_back(flValue);

				entry[X("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<Color>"):
			{
				const auto vecColors = variable.Get<std::vector<Color>>();
				// store vector values as sub-node
				nlohmann::json sub = { };

				// fill node with all vector values
				for (const auto& colValue : vecColors)
					sub.push_back(colValue.GetHex());

				entry[X("value")] = sub.dump();
				break;
			}
			default:
				break;
			}

			// add current variable to config
			config.push_back(entry);
		}
	}
	catch (const nlohmann::detail::exception& ex)
	{
		Logging::PushConsoleColor(FOREGROUND_RED);
		Logging::Print(X("[error] json save failed: {}"), ex.what());
		Logging::PopConsoleColor();
		return false;
	}

	// open output config file
	std::ofstream ofsOutFile(szFile, std::ios::out | std::ios::trunc);

	if (!ofsOutFile.good())
		return false;

	try
	{
		// write stored variables
		ofsOutFile << config.dump(4);
		ofsOutFile.close();
	}
	catch (std::ofstream::failure& ex)
	{
		Logging::PushConsoleColor(FOREGROUND_RED);
		Logging::Print(X("[error] failed to save configuration: {}"), ex.what());
		Logging::PopConsoleColor();
		return false;
	}

	Logging::Print(X("saved configuration at: {}"), szFile);
	return true;
}

bool Config::Load(std::string_view szFileName)
{
	// get utf-8 full path to config
	const std::string szFile = std::filesystem::path(fsPath / szFileName).string();
	nlohmann::json config = { };

	// open input config file
	std::ifstream ifsInputFile(szFile, std::ios::in);

	if (!ifsInputFile.good())
		return false;

	try
	{
		// parse saved variables
		config = nlohmann::json::parse(ifsInputFile, nullptr, false);

		// check is json parse failed
		if (config.is_discarded())
			return false;

		ifsInputFile.close();
	}
	catch (std::ifstream::failure& ex)
	{
		Logging::PushConsoleColor(FOREGROUND_RED);
		Logging::Print(X("[error] failed to load configuration: {}"), ex.what());
		Logging::PopConsoleColor();
		return false;
	}

	try
	{
		for (const auto& variable : config)
		{
			const std::size_t nIndex = GetVariableIndex(variable[X("name-id")].get<FNV1A_t>());

			// check is variable exist
			if (nIndex == C_INVALID_VARIABLE)
				continue;

			// get variable
			auto& entry = vecVariables.at(nIndex);

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
					// check is item out of bounds
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
					// check is item out of bounds
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
					// check is item out of bounds
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
					// check is item out of bounds
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
		Logging::PushConsoleColor(FOREGROUND_RED);
		Logging::Print(X("[error] json load failed: {}"), ex.what());
		Logging::PopConsoleColor();
		return false;
	}

	Logging::Print(X("loaded configuration at: {}"), szFile);
	return true;
}

void Config::Remove(std::string_view szFileName)
{
	const std::string szFile = std::filesystem::path(fsPath / szFileName).string();

	// unable delete default config
	if (szFileName.compare(X("Default.json")) == 0)
		return;

	if (std::filesystem::remove(szFile))
		Logging::Print(X("removed configuration at: {}"), szFile);
}

void Config::Refresh()
{
	vecFileNames.clear();

	for (const auto& it : std::filesystem::directory_iterator(fsPath))
	{
		if (it.path().filename().extension() == X(".json"))
		{
			Logging::Print(X("found configuration file: {}"), it.path().filename().string());
			vecFileNames.push_back(it.path().filename().string());
		}
	}
}

std::size_t Config::GetVariableIndex(const FNV1A_t uNameHash)
{
	for (std::size_t i = 0U; i < vecVariables.size(); i++)
	{
		if (vecVariables.at(i).uNameHash == uNameHash)
			return i;
	}

	return C_INVALID_VARIABLE;
}

std::filesystem::path Config::GetWorkingPath()
{
	std::filesystem::path fsWorkingPath;
	// get path to user documents
	if (PWSTR pszPathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0UL, nullptr, &pszPathToDocuments)))
	{
		fsWorkingPath.assign(pszPathToDocuments);
		fsWorkingPath.append(X("External Base"));
		CoTaskMemFree(pszPathToDocuments);
	}

	return fsWorkingPath;
}
