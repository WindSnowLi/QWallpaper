
/*
 * @Description: 用于读取ini配置文件
 * @version:0.0.2 preview
 * @Author: windSnowLi
 * @Date:2020/5/7
 * @LastEditors: windSnowLi
 * @LastEditTime:2020/11/9
 */

#ifndef READWRITE_H_
#define READWRITE_H_
#include <unordered_map>
 //为了兼容MSVC与MingW,在此引入了cstring与string
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <exception>
#include <memory> 
#ifndef MAX_PATH
#define MAX_PATH 260
#endif // !MAX_PATH
namespace rwini {
	enum class RwExceptionType {
		RwiniException = 0, NotFoundKey = 1, NotFoundSection = 2, IniFormatError = 3, NullSectionOrKey = 4
	};
	class RwiniException :public std::exception {
	protected:
		std::string message;
	public:
#ifdef  _MSC_VER
		virtual const char* what() const {
			return message.c_str();
		}
#elif	__GNUC__
		virtual const char* what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_USE_NOEXCEPT
		{
			return message.c_str();
		}
#endif //  _MSC_VER||__BORLANDC__
		virtual const rwini::RwExceptionType GetExceptionType() const {
			return RwExceptionType::RwiniException;
		}
	};

	class NotFoundKey : public RwiniException
	{
	public:

		virtual const rwini::RwExceptionType GetExceptionType() const {
			return RwExceptionType::NotFoundKey;
		}
		NotFoundKey() { this->message = "Error : 没有发现指定Key"; }
		NotFoundKey(std::string str) {
			this->message = "Error : " + str;
		}
	};


	class NotFoundSection : public RwiniException
	{
	public:
		virtual const rwini::RwExceptionType GetExceptionType() const {
			return RwExceptionType::NotFoundSection;
		}
		NotFoundSection() { this->message = "Error : 没有发现指定Section"; }
		NotFoundSection(std::string str) {
			this->message = "Error : " + str;
		}
	};

	class NullSectionOrKey : public RwiniException
	{
	public:
		virtual const rwini::RwExceptionType GetExceptionType() const {
			return RwExceptionType::NullSectionOrKey;
		}
		NullSectionOrKey(std::string str) { this->message = "Error : " + str + "参数为空"; }
	};
	class IniFormatError : public RwiniException
	{
	public:
		virtual const rwini::RwExceptionType GetExceptionType() const {
			return RwExceptionType::IniFormatError;
		}
		IniFormatError() { this->message = "Error : 文件格式错误"; }
	};
	class ReadWriteini {
	private:
		//配置文件路径全称
		char iniPath[MAX_PATH];
		//[section [key,value] ]
		std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>* iniContent = new std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>();
		//将接收到的参数转化为字符串
		template <typename Type>
		std::string TypeToString(Type str)
			noexcept;
	public:
		//构造函数,参数为ini文件完整路径
		ReadWriteini(const char* inipath)
			noexcept(false);

		//析构函数
		~ReadWriteini();

		//查找指定[section [key,value] ]
		template <typename Type1, typename Type2>
		bool FindValue(const Type1& tempsection, const Type2& tempkey, char* value)
			noexcept(false);;

		template<typename Type1, typename Type2>
		std::string FindValue(const Type1& tempsection, const Type2& tempkey)
			noexcept(false);

		//设置value
		template <typename Type1, typename Type2, typename Type3>
		bool SetValue(const Type1& tempsection, const Type2& tempkey, const Type3& value)
			noexcept(false);

		//设置key
		template <typename Type1, typename Type2, typename Type3>
		bool SetKey(const Type1& tempsection, const  Type2& tempoldkey, const  Type3& tempnewkey)
			noexcept(false);

		//插入section
		template <typename Type1>
		bool InsertSection(const Type1& tempsection);

		//插入key
		template <typename Type1, typename Type2, typename Type3>
		bool InsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue)
			noexcept(false);

		template<typename Type1, typename Type2, typename Type3>
		bool AutoInsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue)
			noexcept(false);

		//删除section
		template <typename Type1>
		bool DeleteSection(const Type1& tempsection)
			noexcept(false);

		//删除key
		template <typename Type1, typename Type2>
		bool DeleteKey(const Type1& tempsection, const Type2& tempkey);

		//写入ini文件
		bool Writeini();

		//检查是否能查到section或key,如果查询异常向上抛出异常
		std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>::iterator
			SeekSection(std::string temp_section)
			noexcept(false);
		std::unordered_map<std::string, std::vector<std::string>>::iterator
			SeekKey(std::string temp_section, std::string temp_key)
			noexcept(false);
	};
}


#ifndef READWRITEINI_TEMPLATE_
#define READWRITEINI_TEMPLATE_
/*
 * @name:TypeToString
 * @describe:将参数转化为字符串类型
 * @param typename Type
 * @return:string
 */
template<typename Type>
std::string rwini::ReadWriteini::TypeToString(Type str) noexcept
{
	std::stringstream tempStrStream;
	std::string goalStr;
	tempStrStream << str;
	goalStr = tempStrStream.str();
	return goalStr;
}

/*
 * @name:FindValue
 * @describe:根据section和key查找value
 * @param const Type1& tempsection, const Type2& tempkey, char* value
 * @return:bool
 */
template <typename Type1, typename Type2>
bool rwini::ReadWriteini::FindValue(const Type1& tempsection, const Type2& tempkey, char* value)
noexcept(false)
{
	//将接收的参数转化为字符串
	std::string section = TypeToString(tempsection);
	std::string key = TypeToString(tempkey);
	value[0] = '\0';
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
	const char* tempStr = tempiter->second.front().c_str();
	memcpy(value, tempStr, sizeof(char) * strlen(tempStr));
	value[strlen(tempStr) + 1] = '\0';
	return true;
}

/// <summary>
/// 查找指定value并返回std::string，如果未查找到section或key则抛出NotFoundKey或NotFoundSection异常
/// </summary>
/// <typeparam name="Type1">section的类型</typeparam>
/// <typeparam name="Type2">key的类型</typeparam>
/// <param name="tempsection">section名称</param>
/// <param name="tempkey">key名称</param>
/// <returns>返回目标字符串</returns>
template <typename Type1, typename Type2>
std::string rwini::ReadWriteini::FindValue(const Type1& tempsection, const Type2& tempkey)
noexcept(false)
{
	//将接收的参数转化为字符串
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

/*
 * @name:SetValue
 * @describe:根据section key设置value
 * @param const Type1& tempsection, const Type2& tempkey, const Type3& tempvalue
 * @return:bool
 */
template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::SetValue(const Type1& tempsection, const Type2& tempkey, const Type3& tempvalue)
noexcept(false)
{
	//将接收的参数转化为字符串
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

/*
 * @name:SetKey
 * @describe:根据section设置key
 * @param {const char*, const char*,const char*}
 * @return:bool
 */
 /*
 *2020/6/11修改，使用模板接收参数
 */
template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::SetKey(const Type1& tempsection, const  Type2& tempoldkey, const  Type3& tempnewkey)
noexcept(false)
{
	//将接收的参数转化为字符串
	std::string section = TypeToString(tempsection);
	std::string oldkey = TypeToString(tempoldkey);
	std::string newkey = TypeToString(tempnewkey);
	if (newkey.empty()) {
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



/*
 * @name:InsertSection
 * @describe:插入section
 * @param {const char*}
 * @return:bool
 */
template <typename Type1>
bool rwini::ReadWriteini::InsertSection(const Type1& tempsection) noexcept(false)
{
	//将接收的参数转化为字符串
	std::string section = TypeToString(tempsection);
	if (section.empty()) {
		throw* (new NullSectionOrKey("section"));
	}
	iniContent->insert(std::make_pair(section, std::unordered_map<std::string, std::vector<std::string>>()));
	return true;
}





/*
 * @name:InsertKey
 * @describe:插入Key
 * @param const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue
 * @return:bool
 */
template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::InsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue)
noexcept(false)
{
	//将接收的参数转化为字符串
	std::string section = TypeToString(tempsection);
	std::string key = TypeToString(tempkey);

	std::string value = TypeToString(tempvalue);
	if (section.empty()) {
		throw* (new NullSectionOrKey("section"));
	}
	if (key.empty()) {
		throw* (new NullSectionOrKey("key"));
	}
	auto iter = iniContent->find(section);
	if (iter == iniContent->end()) {
		throw* (new NotFoundSection("未发现section：" + section));
	}
	std::shared_ptr<std::vector<std::string>> temp_value = std::make_shared< std::vector<std::string>>();
	temp_value->push_back(value);
	iter->second.insert(std::make_pair(key, *temp_value));
	return true;
}

/// <summary>
/// 如若无上级标签，则自动添加上级标签
/// </summary>
/// <typeparam name="Type1">section类型</typeparam>
/// <typeparam name="Type2">key类型</typeparam>
/// <typeparam name="Type3">value类型</typeparam>
/// <param name="tempsection">section</param>
/// <param name="tempkey">key</param>
/// <param name="tempvalue">value</param>
/// <returns>插入结果 </returns>
template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::AutoInsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue)
noexcept(false)
{
	//将接收的参数转化为字符串
	std::string section = TypeToString(tempsection);
	std::string key = TypeToString(tempkey);
	std::string value = TypeToString(tempvalue);
	if (section.empty()) {
		throw* (new NullSectionOrKey("section"));
	}
	if (key.empty()) {
		throw* (new NullSectionOrKey("key"));
	}
	auto iter = iniContent->find(section);
	if (iter == iniContent->end()) {
		this->InsertSection(section);
	}
	iter = iniContent->find(section);
	std::shared_ptr<std::vector<std::string>> temp_value = std::make_shared< std::vector<std::string>>();
	temp_value->push_back(value);
	iter->second.insert(std::make_pair(key, *temp_value));
	return true;
}

/*
 * @name:DeleteSection
 * @describe:删除section
 * @param const Type1& tempsection
 * @return:bool
 */
 /*
 *2020/6/11修改，使用模板接收参数
 */
template <typename Type1>
bool rwini::ReadWriteini::DeleteSection(const Type1& tempsection)
noexcept(false)
{
	//将接收的参数转化为字符串
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

/*
 * @name:DeleteKey
 * @describe:删除Key
 * @param {const char*}
 * @return:bool
 */
template <typename Type1, typename Type2>
bool rwini::ReadWriteini::DeleteKey(const Type1& tempsection, const Type2& tempkey)
{
	//将接收的参数转化为字符串
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


/*
 * @name:构造函数
 * @describe:初始化基础数据
 * @param {char*}
 * @return:无
 */

rwini::ReadWriteini::ReadWriteini(const char* inipath) noexcept(false) {
	memset(iniPath, '\0', sizeof(char) * MAX_PATH);
	memcpy(iniPath, inipath, sizeof(char) * strlen(inipath));

	std::ifstream iniFile;
	iniFile.open(iniPath, std::ios::in);
	std::string tempSection;
	std::string tempKey;
	std::string tempValue;
	std::string tempAnn;
	//左右中括号，左+1，右-1
	int tempSectionBracket = 0;
	enum class LineType
	{
		//当前行是section,键值对,注释,不录入
		section = 1, key = 2, value = 3, ann = 4, refuse = 5
	};
	std::shared_ptr<LineType> lineType = std::make_shared<LineType>();
	*lineType = LineType::refuse;
	//临时储存一行的数据
	std::string tempData;
	std::string temp_end_section;
	std::string temp_end_key;
	while (getline(iniFile, tempData)) {
		if (tempData.length() == 0) {
			continue;
		}
		bool emptystr = true;
		for (char tempCheck : tempData) {
			if (tempCheck == ' ') {
				continue;
			}
			emptystr = false;
			if (tempCheck == '[') {
				*lineType = LineType::section;
			}
			else if (tempCheck == ';') {
				*lineType = LineType::ann;
			}
			else {
				*lineType = LineType::key;
			}
			break;
		}
		if (emptystr) {
			continue;
		}
		for (char tempCh : tempData) {
			if (*lineType != LineType::refuse && tempCh == ' ') {
				continue;
			}
			else if (*lineType == LineType::section && tempCh == '[') {
				*lineType = LineType::section;
				if (++tempSectionBracket >= 2) {
					throw* (new IniFormatError());
				}
			}
			else if (*lineType == LineType::section && tempCh == ']') {
				*lineType = LineType::refuse;
				if (--tempSectionBracket < 0) {
					throw* (new IniFormatError());
				}
			}
			else if (*lineType != LineType::ann && tempCh == ';') {
				*lineType = LineType::ann;
			}
			else if (*lineType != LineType::ann && *lineType != LineType::value && tempCh == '=') {
				*lineType = LineType::value;
			}
			else if (*lineType == LineType::ann) {
				tempAnn.append(TypeToString(tempCh));
			}
			else if (*lineType == LineType::section) {
				tempSection.append(TypeToString(tempCh));
			}
			else if (*lineType == LineType::key) {
				tempKey.append(TypeToString(tempCh));
			}
			else if (*lineType == LineType::value) {
				tempValue.append(TypeToString(tempCh));
			}
		}
		if (!tempSection.empty()) {
			//插入一个section
			iniContent->insert(std::make_pair(tempSection, std::unordered_map<std::string, std::vector<std::string>>()));
			temp_end_section = tempSection;
			temp_end_key.clear();
		}
		else if (!tempKey.empty()) {
			std::vector<std::string> temp_add_ann;
			temp_add_ann.push_back(tempValue);
			if (!tempAnn.empty()) {
				temp_add_ann.push_back(tempAnn);
			}
			iniContent->find(temp_end_section)->second.insert(std::make_pair(tempKey, temp_add_ann));
			temp_end_key = tempKey;
		}
		else if (!tempAnn.empty()) {
			if (!temp_end_key.empty()) {
				iniContent->find(temp_end_section)->second.find(temp_end_key)->second.push_back(tempAnn);
			}
		}
		tempSection.clear();
		tempKey.clear();
		tempValue.clear();
		tempAnn.clear();
		*lineType = LineType::refuse;
	}
	//释放枚举数据
	lineType.~shared_ptr();
	//关闭文件
	iniFile.close();
}

rwini::ReadWriteini::~ReadWriteini()
{
	delete iniContent;
}


/*
 * @name:Writeini
 * @describe:写入文件
 * @param {const char*}
 * @return:bool
 */
bool rwini::ReadWriteini::Writeini()
{
	std::ofstream iniFile;
	iniFile.open(iniPath, std::ios::out | std::ios::trunc);

	for (auto&& i : *iniContent) {
		iniFile << "[" << i.first << "]" << std::endl;
		for (auto&& j : i.second) {
			iniFile << j.first << "=" << j.second.front() << std::endl;
			for (int k = 1; k < j.second.size(); k++) {
				if (j.second[k].front() != ';') {
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
rwini::ReadWriteini::SeekSection(std::string temp_section)
noexcept(false)
{
	if (temp_section.empty()) {
		throw* (new NullSectionOrKey("section"));
	}
	auto iter = iniContent->find(temp_section);
	if (iter == iniContent->end()) {
		throw* (new NotFoundSection("未发现section：" + temp_section));
	}
	return iter;
}
std::unordered_map<std::string, std::vector<std::string>>::iterator
rwini::ReadWriteini::SeekKey(std::string temp_section, std::string temp_key)
noexcept(false) {
	if (temp_section.empty()) {
		throw* (new NullSectionOrKey("section"));
	}
	if (temp_key.empty()) {
		throw* (new NullSectionOrKey("key"));
	}
	auto iter = iniContent->find(temp_section);
	if (iter == iniContent->end()) {
		throw* (new NotFoundSection("未发现section：" + temp_section));
	}
	auto tempiter = iter->second.find(temp_key);
	if (tempiter == iter->second.end()) {
		throw* (new NotFoundKey("未在section:" + temp_section + "下发现key：" + temp_key));
	}
	return tempiter;
}
#endif // READWRITEINI_TEMPLATE_
#endif // !READWRITE_H_