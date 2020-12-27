
//@Description: Used to read ini format files

#ifndef READWRITE_H_
#define READWRITE_H_
#include <unordered_map>
#include <cstring>
#include <sstream>
#include <fstream>
#include <exception>
#include <memory>
#include <vector>
#ifndef MAX_PATH
#define MAX_PATH 260
#endif // !MAX_PATH
/// <summary>
/// The main function belongs to a namespace
/// </summary>
namespace rwini
{
	enum class RwExceptionType
	{
		/// <summary>
		/// Base exception
		/// </summary>
		RwiniException = 0,
		/// <summary>
		/// Not found Key
		/// </summary>
		NotFoundKey = 1,
		/// <summary>
		/// Not found Section
		/// </summary>
		NotFoundSection = 2,
		/// <summary>
		/// File format error
		/// </summary>
		IniFormatError = 3,
		/// <summary>
		/// Parameter is null
		/// </summary>
		NullSectionOrKey = 4
	};
	/// <summary>
	/// Base RwiniException
	/// </summary>
	/// <returns></returns>
	class RwiniException : public std::exception
	{
	protected:
		std::string message;

	public:
#ifdef _MSC_VER
		/// <summary>
		/// _MSC_VER exception content
		/// </summary>
		/// <returns>Exception content</returns>
		virtual const char* what() const
		{
			return message.c_str();
		}
#elif __GNUC__
		/// <summary>
		/// GUNC exception content
		/// </summary>
		/// <returns>Exception content</returns>
		virtual const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT
		{
			return message.c_str();
		}
#endif //  _MSC_VER||__BORLANDC__
		virtual const rwini::RwExceptionType GetExceptionType() const
		{
			return RwExceptionType::RwiniException;
		}
	};
	/// <summary>
	/// Not found Key
	/// </summary>
	class NotFoundKey : public RwiniException
	{
	public:
		virtual const rwini::RwExceptionType GetExceptionType() const
		{
			return RwExceptionType::NotFoundKey;
		}
		NotFoundKey() { this->message = "Error : The specified Key was not found"; }
		NotFoundKey(std::string str)
		{
			this->message = "Error : " + str;
		}
	};
	/// <summary>
	/// Not found Section
	/// </summary>
	class NotFoundSection : public RwiniException
	{
	public:
		virtual const rwini::RwExceptionType GetExceptionType() const
		{
			return RwExceptionType::NotFoundSection;
		}
		NotFoundSection() { this->message = "Error : The specified Section was not found"; }
		NotFoundSection(std::string str)
		{
			this->message = "Error : " + str;
		}
	};
	/// <summary>
	/// Parameter is null
	/// </summary>
	class NullSectionOrKey : public RwiniException
	{
	public:
		virtual const rwini::RwExceptionType GetExceptionType() const
		{
			return RwExceptionType::NullSectionOrKey;
		}
		NullSectionOrKey(std::string str) { this->message = "Error : " + str + "Parameter is null"; }
	};
	/// <summary>
	/// File format error
	/// </summary>
	class IniFormatError : public RwiniException
	{
	public:
		virtual const rwini::RwExceptionType GetExceptionType() const
		{
			return RwExceptionType::IniFormatError;
		}
		IniFormatError() { this->message = "Error : File format error"; }
	};
	/// <summary>
	/// The main class
	/// </summary>
	class ReadWriteini
	{
	private:
		//File path
		char iniPath[MAX_PATH];
		//[section [key,value] ]
		std::shared_ptr<std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>> iniContent = std::make_shared<std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>>();

		/// <summary>
		/// Convert the parameter to string
		/// </summary>
		/// <typeparam name="Type">Raw data type</typeparam>
		/// <param name="str">data</param>
		/// <returns>Result string</returns>
		template <typename Type>
		std::string TypeToString(Type str) noexcept;

	public:
		/// <summary>
		/// Initializes the load file
		/// </summary>
		/// <param name="inipath">File path</param>
		/// <returns>None</returns>
		ReadWriteini(const char* inipath) noexcept(false);

		/// <summary>
		/// End processing
		/// </summary>
		~ReadWriteini();

		/// <summary>
		/// Find the value based on section and key
		/// </summary>
		/// <typeparam name="Type1">Section typename</typeparam>
		/// <typeparam name="Type2">Key typename</typeparam>
		/// <param name="tempsection">Section</param>
		/// <param name="tempkey">Key</param>
		/// <param name="value">Results the address</param>
		/// <returns>State of the query</returns>
		template <typename Type1, typename Type2>
		bool FindValue(const Type1& tempsection, const Type2& tempkey, char* value) noexcept(false);

		/// <summary>
		/// Finds the specified value and returns std::string, and throws a NotFoundKey or NotFoundSection exception if no section or key is found
		/// </summary>
		/// <typeparam name="Type1">Section typename</typeparam>
		/// <typeparam name="Type2">Key typename</typeparam>
		/// <param name="tempsection">Section</param>
		/// <param name="tempkey">Key</param>
		/// <returns>Return search results</returns>
		template <typename Type1, typename Type2>
		std::string FindValue(const Type1& tempsection, const Type2& tempkey) noexcept(false);

		/// <summary>
		/// Set value
		/// </summary>
		/// <typeparam name="Type1">Section typename </typeparam>
		/// <typeparam name="Type2">Key typename</typeparam>
		/// <typeparam name="Type3">Value typename</typeparam>
		/// <param name="tempsection">Section</param>
		/// <param name="tempkey">Key</param>
		/// <param name="tempvalue">value</param>
		/// <returns>Status</returns>
		template <typename Type1, typename Type2, typename Type3>
		bool SetValue(const Type1& tempsection, const Type2& tempkey, const Type3& value) noexcept(false);

		/// <summary>
		/// Set the key under section
		/// </summary>
		/// <typeparam name="Type1">Section typename</typeparam>
		/// <typeparam name="Type2">Old key typename</typeparam>
		/// <typeparam name="Type3">New Key typename</typeparam>
		/// <param name="tempsection">Section</param>
		/// <param name="tempoldkey">Old key</param>
		/// <param name="tempnewkey">New key</param>
		/// <returns>Status</returns>
		template <typename Type1, typename Type2, typename Type3>
		bool SetKey(const Type1& tempsection, const Type2& tempoldkey, const Type3& tempnewkey) noexcept(false);

		/// <summary>
		/// Insert Section
		/// </summary>
		/// <typeparam name="Type1">Section typename</typeparam>
		/// <param name="tempsection">Section</param>
		/// <returns>Status</returns>
		template <typename Type1>
		bool InsertSection(const Type1& tempsection);

		/// <summary>
		/// Insert the Key under Section
		/// </summary>
		/// <typeparam name="Type1">Section typename </typeparam>
		/// <typeparam name="Type2">Key typename</typeparam>
		/// <typeparam name="Type3">Value typename</typeparam>
		/// <param name="tempsection">Section</param>
		/// <param name="tempkey">Key</param>
		/// <param name="tempvalue">value</param>
		/// <returns>status</returns>
		template <typename Type1, typename Type2, typename Type3>
		bool InsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue) noexcept(false);

		/// <summary>
		/// If there is no parent tag, the parent tag is automatically added
		/// </summary>
		/// <typeparam name="Type1">Section typename</typeparam>
		/// <typeparam name="Type2">Key typename</typeparam>
		/// <typeparam name="Type3">Value typename</typeparam>
		/// <param name="tempsection">Section</param>
		/// <param name="tempkey">Key</param>
		/// <param name="tempvalue">value</param>
		/// <returns>Status</returns>
		template <typename Type1, typename Type2, typename Type3>
		bool AutoInsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue) noexcept(false);

		/// <summary>
		/// Delete section
		/// </summary>
		/// <typeparam name="Type1">The parameter types</typeparam>
		/// <param name="tempsection">The value of the parameter</param>
		/// <returns>status</returns>
		template <typename Type1>
		bool DeleteSection(const Type1& tempsection) noexcept(false);

		/// <summary>
		/// Delect Key
		/// </summary>
		/// <typeparam name="Type1">Section typename</typeparam>
		/// <typeparam name="Type2">Key typename</typeparam>
		/// <param name="tempsection">Section</param>
		/// <param name="tempkey">Key</param>
		/// <returns>Delete status</returns>
		template <typename Type1, typename Type2>
		bool DeleteKey(const Type1& tempsection, const Type2& tempkey);

		/// <summary>
		/// Written to the file
		/// </summary>
		/// <returns>Written status</returns>
		bool Writeini();

		/// <summary>
		/// To find the Section
		/// </summary>
		/// <param name="temp_section">Section typename</param>
		/// <returns>Section table</returns>
		std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>::iterator
			SeekSection(std::string temp_section) noexcept(false);

		/// <summary>
		/// Look up the Key under Section and return the Key table
		/// </summary>
		/// <param name="temp_section">Section typename</param>
		/// <param name="temp_key">Key typename</param>
		/// <returns>Key table</returns>
		std::unordered_map<std::string, std::vector<std::string>>::iterator
			SeekKey(std::string temp_section, std::string temp_key) noexcept(false);
	};
} // namespace rwini

#ifndef READWRITEINI_TEMPLATE_
#define READWRITEINI_TEMPLATE_

template <typename Type>
std::string rwini::ReadWriteini::TypeToString(Type str) noexcept
{
	std::stringstream tempStrStream;
	std::string goalStr;
	tempStrStream << str;
	goalStr = tempStrStream.str();
	return goalStr;
}

template <typename Type1, typename Type2>
bool rwini::ReadWriteini::FindValue(const Type1& tempsection, const Type2& tempkey, char* value) noexcept(false)
{
	//Converts the received argument to a string
	std::string section = TypeToString(tempsection);
	std::string key = TypeToString(tempkey);
	value[0] = '\0';
	std::unordered_map<std::string, std::vector<std::string>>::iterator tempiter;
	try
	{
		//Find the Key under Section
		tempiter = SeekKey(section, key);
	}
	catch (const rwini::NullSectionOrKey& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundSection& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundKey& e)
	{
		throw e;
	}
	//Gets the value of the Key
	const char* tempStr = tempiter->second.front().c_str();
	memcpy(value, tempStr, sizeof(char) * strlen(tempStr));
	value[strlen(tempStr) + 1] = '\0';
	return true;
}

template <typename Type1, typename Type2>
std::string rwini::ReadWriteini::FindValue(const Type1& tempsection, const Type2& tempkey) noexcept(false)
{
	//Converts the received argument to a string
	std::string section = TypeToString(tempsection);
	std::string key = TypeToString(tempkey);
	std::unordered_map<std::string, std::vector<std::string>>::iterator tempiter;
	try
	{
		tempiter = SeekKey(section, key);
	}
	catch (const rwini::NullSectionOrKey& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundSection& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundKey& e)
	{
		throw e;
	}
	return tempiter->second.front();
}

template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::SetValue(const Type1& tempsection, const Type2& tempkey, const Type3& tempvalue) noexcept(false)
{
	//Converts the received argument to a string
	std::string section = TypeToString(tempsection);
	std::string key = TypeToString(tempkey);
	std::string value = TypeToString(tempvalue);
	std::unordered_map<std::string, std::vector<std::string>>::iterator tempiter;
	try
	{
		tempiter = SeekKey(section, key);
	}
	catch (const rwini::NullSectionOrKey& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundSection& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundKey& e)
	{
		throw e;
	}
	tempiter->second[0] = value;
	return true;
}

template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::SetKey(const Type1& tempsection, const Type2& tempoldkey, const Type3& tempnewkey) noexcept(false)
{
	//Converts the received argument to a string
	std::string section = TypeToString(tempsection);
	std::string oldkey = TypeToString(tempoldkey);
	std::string newkey = TypeToString(tempnewkey);
	if (newkey.empty())
	{
		throw* (new NullSectionOrKey("new key:" + newkey));
	}
	try
	{
		auto tempiter = SeekKey(section, oldkey);
		const std::vector<std::string> tempvalue = tempiter->second;
		auto iter = SeekSection(section);
		iter->second.erase(tempiter);
		iter->second.insert(std::make_pair(newkey, tempvalue));
	}
	catch (const rwini::NullSectionOrKey& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundSection& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundKey& e)
	{
		throw e;
	}
	return true;
}

template <typename Type1>
bool rwini::ReadWriteini::InsertSection(const Type1& tempsection) noexcept(false)
{
	//Converts the received argument to a string
	std::string section = TypeToString(tempsection);
	if (section.empty())
	{
		throw* (new NullSectionOrKey("section"));
	}
	iniContent->insert(std::make_pair(section, std::unordered_map<std::string, std::vector<std::string>>()));
	return true;
}

template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::InsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue) noexcept(false)
{
	//Converts the received argument to a string
	std::string section = TypeToString(tempsection);
	std::string key = TypeToString(tempkey);

	std::string value = TypeToString(tempvalue);
	if (section.empty())
	{
		throw* (new NullSectionOrKey("section"));
	}
	if (key.empty())
	{
		throw* (new NullSectionOrKey("key"));
	}
	auto iter = iniContent->find(section);
	if (iter == iniContent->end())
	{
		throw* (new NotFoundSection("Not found section：" + section));
	}
	std::shared_ptr<std::vector<std::string>> temp_value = std::make_shared<std::vector<std::string>>();
	temp_value->push_back(value);
	iter->second.insert(std::make_pair(key, *temp_value));
	return true;
}

template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::AutoInsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue) noexcept(false)
{
	//Converts the received argument to a string
	std::string section = TypeToString(tempsection);
	std::string key = TypeToString(tempkey);
	std::string value = TypeToString(tempvalue);
	if (section.empty())
	{
		throw* (new NullSectionOrKey("section"));
	}
	if (key.empty())
	{
		throw* (new NullSectionOrKey("key"));
	}
	auto iter = iniContent->find(section);
	if (iter == iniContent->end())
	{
		this->InsertSection(section);
	}
	iter = iniContent->find(section);
	std::shared_ptr<std::vector<std::string>> temp_value = std::make_shared<std::vector<std::string>>();
	temp_value->push_back(value);
	iter->second.insert(std::make_pair(key, *temp_value));
	return true;
}

template <typename Type1>
bool rwini::ReadWriteini::DeleteSection(const Type1& tempsection) noexcept(false)
{
	//Converts the received argument to a string
	std::string section = TypeToString(tempsection);
	try
	{
		auto iter = SeekSection(section);
		iniContent->erase(iter);
	}
	catch (const rwini::NullSectionOrKey& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundSection& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundKey& e)
	{
		throw e;
	}
	return true;
}

template <typename Type1, typename Type2>
bool rwini::ReadWriteini::DeleteKey(const Type1& tempsection, const Type2& tempkey)
{
	//Converts the received argument to a string
	std::string section = TypeToString(tempsection);
	std::string key = TypeToString(tempkey);
	try
	{
		auto iter = SeekSection(section);
		auto tempiter = iter->second.find(key);
		iter->second.erase(tempiter);
	}
	catch (const rwini::NullSectionOrKey& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundSection& e)
	{
		throw e;
	}
	catch (const rwini::NotFoundKey& e)
	{
		throw e;
	}
	return true;
}
#endif // !READWRITEINI_TEMPLATE_

#ifndef READWRITEINI_REALIZE_
#define READWRITEINI_REALIZE_

rwini::ReadWriteini::ReadWriteini(const char* inipath) noexcept(false)
{
	memset(iniPath, '\0', sizeof(char) * MAX_PATH);
	memcpy(iniPath, inipath, sizeof(char) * strlen(inipath));

	std::ifstream iniFile;
	iniFile.open(iniPath, std::ios::in);
	std::string tempSection;
	std::string tempKey;
	std::string tempValue;
	std::string tempAnn;
	//Matching parentheses
	int tempSectionBracket = 0;
	enum class LineType
	{
		// The current line is section
		section = 1,
		// The current line is key
		key = 2,
		//The current line is value
		value = 3,
		//The current line is  annotation
		ann = 4,
		// The current line is not typed
		refuse = 5
	};
	std::shared_ptr<LineType> lineType = std::make_shared<LineType>();
	*lineType = LineType::refuse;
	//Temporarily stores one row of data
	std::string tempData;
	std::string temp_end_section;
	std::string temp_end_key;
	while (getline(iniFile, tempData))
	{
		if (tempData.length() == 0)
		{
			continue;
		}
		bool emptystr = true;
		for (char tempCheck : tempData)
		{
			if (tempCheck == ' ')
			{
				continue;
			}
			emptystr = false;
			if (tempCheck == '[')
			{
				*lineType = LineType::section;
			}
			else if (tempCheck == ';')
			{
				*lineType = LineType::ann;
			}
			else
			{
				*lineType = LineType::key;
			}
			break;
		}
		if (emptystr)
		{
			continue;
		}
		for (char tempCh : tempData)
		{
			if (*lineType != LineType::refuse && tempCh == ' ')
			{
				continue;
			}
			else if (*lineType == LineType::section && tempCh == '[')
			{
				*lineType = LineType::section;
				if (++tempSectionBracket >= 2)
				{
					throw* (new IniFormatError());
				}
			}
			else if (*lineType == LineType::section && tempCh == ']')
			{
				*lineType = LineType::refuse;
				if (--tempSectionBracket < 0)
				{
					throw* (new IniFormatError());
				}
			}
			else if (*lineType != LineType::ann && tempCh == ';')
			{
				*lineType = LineType::ann;
			}
			else if (*lineType != LineType::ann && *lineType != LineType::value && tempCh == '=')
			{
				*lineType = LineType::value;
			}
			else if (*lineType == LineType::ann)
			{
				tempAnn.append(TypeToString(tempCh));
			}
			else if (*lineType == LineType::section)
			{
				tempSection.append(TypeToString(tempCh));
			}
			else if (*lineType == LineType::key)
			{
				tempKey.append(TypeToString(tempCh));
			}
			else if (*lineType == LineType::value)
			{
				tempValue.append(TypeToString(tempCh));
			}
		}
		if (!tempSection.empty())
		{
			//Insert section
			iniContent->insert(std::make_pair(tempSection, std::unordered_map<std::string, std::vector<std::string>>()));
			temp_end_section = tempSection;
			temp_end_key.clear();
		}
		else if (!tempKey.empty())
		{
			std::vector<std::string> temp_add_ann;
			temp_add_ann.push_back(tempValue);
			if (!tempAnn.empty())
			{
				temp_add_ann.push_back(tempAnn);
			}
			iniContent->find(temp_end_section)->second.insert(std::make_pair(tempKey, temp_add_ann));
			temp_end_key = tempKey;
		}
		else if (!tempAnn.empty())
		{
			if (!temp_end_key.empty())
			{
				iniContent->find(temp_end_section)->second.find(temp_end_key)->second.push_back(tempAnn);
			}
		}
		tempSection.clear();
		tempKey.clear();
		tempValue.clear();
		tempAnn.clear();
		*lineType = LineType::refuse;
	}
	//Release enumerated data
	lineType.~shared_ptr();
	//Close file
	iniFile.close();
}

rwini::ReadWriteini::~ReadWriteini()
{
	iniContent.~shared_ptr();
}

bool rwini::ReadWriteini::Writeini()
{
	std::ofstream iniFile;
	iniFile.open(iniPath, std::ios::out | std::ios::trunc);

	for (auto&& i : *iniContent)
	{
		iniFile << "[" << i.first << "]" << std::endl;
		for (auto&& j : i.second)
		{
			iniFile << j.first << "=" << j.second.front() << std::endl;
			for (int k = 1; k < j.second.size(); k++)
			{
				if (j.second[k].front() != ';')
				{
					iniFile << ";";
				}
				iniFile << j.second[k] << std::endl;
			}
		}
		iniFile << std::endl;
	}
	iniFile.close();
	return true;
}

std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>::iterator
rwini::ReadWriteini::SeekSection(std::string temp_section) noexcept(false)
{
	if (temp_section.empty())
	{
		throw* (new NullSectionOrKey("section"));
	}
	auto iter = iniContent->find(temp_section);
	if (iter == iniContent->end())
	{
		throw* (new NotFoundSection("Not found section：" + temp_section));
	}
	return iter;
}

std::unordered_map<std::string, std::vector<std::string>>::iterator
rwini::ReadWriteini::SeekKey(std::string temp_section, std::string temp_key) noexcept(false)
{
	if (temp_section.empty())
	{
		throw* (new NullSectionOrKey("section"));
	}
	if (temp_key.empty())
	{
		throw* (new NullSectionOrKey("key"));
	}
	auto iter = iniContent->find(temp_section);
	if (iter == iniContent->end())
	{
		throw* (new NotFoundSection("Not found section：" + temp_section));
	}
	auto tempiter = iter->second.find(temp_key);
	if (tempiter == iter->second.end())
	{
		throw* (new NotFoundKey("Key " + temp_key + " is not found under section" + temp_section));
	}
	return tempiter;
}
#endif // READWRITEINI_TEMPLATE_
#endif // !READWRITE_H_