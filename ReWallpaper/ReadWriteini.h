
/*
 * @Description: ���ڶ�ȡini�����ļ�
 * @version:0.0.2 preview
 * @Author: windSnowLi
 * @Date:2020/5/7
 * @LastEditors: windSnowLi
 * @LastEditTime:2020/11/9
 */

#ifndef READWRITE_H_
#define READWRITE_H_
#include <unordered_map>
 //Ϊ�˼���MSVC��MingW,�ڴ�������cstring��string
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
		NotFoundKey() { this->message = "Error : û�з���ָ��Key"; }
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
		NotFoundSection() { this->message = "Error : û�з���ָ��Section"; }
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
		NullSectionOrKey(std::string str) { this->message = "Error : " + str + "����Ϊ��"; }
	};
	class IniFormatError : public RwiniException
	{
	public:
		virtual const rwini::RwExceptionType GetExceptionType() const {
			return RwExceptionType::IniFormatError;
		}
		IniFormatError() { this->message = "Error : �ļ���ʽ����"; }
	};
	class ReadWriteini {
	private:
		//�����ļ�·��ȫ��
		char iniPath[MAX_PATH];
		//[section [key,value] ]
		std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>* iniContent = new std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>>();
		//�����յ��Ĳ���ת��Ϊ�ַ���
		template <typename Type>
		std::string TypeToString(Type str)
			noexcept;
	public:
		//���캯��,����Ϊini�ļ�����·��
		ReadWriteini(const char* inipath)
			noexcept(false);

		//��������
		~ReadWriteini();

		//����ָ��[section [key,value] ]
		template <typename Type1, typename Type2>
		bool FindValue(const Type1& tempsection, const Type2& tempkey, char* value)
			noexcept(false);;

		template<typename Type1, typename Type2>
		std::string FindValue(const Type1& tempsection, const Type2& tempkey)
			noexcept(false);

		//����value
		template <typename Type1, typename Type2, typename Type3>
		bool SetValue(const Type1& tempsection, const Type2& tempkey, const Type3& value)
			noexcept(false);

		//����key
		template <typename Type1, typename Type2, typename Type3>
		bool SetKey(const Type1& tempsection, const  Type2& tempoldkey, const  Type3& tempnewkey)
			noexcept(false);

		//����section
		template <typename Type1>
		bool InsertSection(const Type1& tempsection);

		//����key
		template <typename Type1, typename Type2, typename Type3>
		bool InsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue)
			noexcept(false);

		template<typename Type1, typename Type2, typename Type3>
		bool AutoInsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue)
			noexcept(false);

		//ɾ��section
		template <typename Type1>
		bool DeleteSection(const Type1& tempsection)
			noexcept(false);

		//ɾ��key
		template <typename Type1, typename Type2>
		bool DeleteKey(const Type1& tempsection, const Type2& tempkey);

		//д��ini�ļ�
		bool Writeini();

		//����Ƿ��ܲ鵽section��key,�����ѯ�쳣�����׳��쳣
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
 * @describe:������ת��Ϊ�ַ�������
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
 * @describe:����section��key����value
 * @param const Type1& tempsection, const Type2& tempkey, char* value
 * @return:bool
 */
template <typename Type1, typename Type2>
bool rwini::ReadWriteini::FindValue(const Type1& tempsection, const Type2& tempkey, char* value)
noexcept(false)
{
	//�����յĲ���ת��Ϊ�ַ���
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
/// ����ָ��value������std::string�����δ���ҵ�section��key���׳�NotFoundKey��NotFoundSection�쳣
/// </summary>
/// <typeparam name="Type1">section������</typeparam>
/// <typeparam name="Type2">key������</typeparam>
/// <param name="tempsection">section����</param>
/// <param name="tempkey">key����</param>
/// <returns>����Ŀ���ַ���</returns>
template <typename Type1, typename Type2>
std::string rwini::ReadWriteini::FindValue(const Type1& tempsection, const Type2& tempkey)
noexcept(false)
{
	//�����յĲ���ת��Ϊ�ַ���
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
 * @describe:����section key����value
 * @param const Type1& tempsection, const Type2& tempkey, const Type3& tempvalue
 * @return:bool
 */
template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::SetValue(const Type1& tempsection, const Type2& tempkey, const Type3& tempvalue)
noexcept(false)
{
	//�����յĲ���ת��Ϊ�ַ���
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
 * @describe:����section����key
 * @param {const char*, const char*,const char*}
 * @return:bool
 */
 /*
 *2020/6/11�޸ģ�ʹ��ģ����ղ���
 */
template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::SetKey(const Type1& tempsection, const  Type2& tempoldkey, const  Type3& tempnewkey)
noexcept(false)
{
	//�����յĲ���ת��Ϊ�ַ���
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
 * @describe:����section
 * @param {const char*}
 * @return:bool
 */
template <typename Type1>
bool rwini::ReadWriteini::InsertSection(const Type1& tempsection) noexcept(false)
{
	//�����յĲ���ת��Ϊ�ַ���
	std::string section = TypeToString(tempsection);
	if (section.empty()) {
		throw* (new NullSectionOrKey("section"));
	}
	iniContent->insert(std::make_pair(section, std::unordered_map<std::string, std::vector<std::string>>()));
	return true;
}





/*
 * @name:InsertKey
 * @describe:����Key
 * @param const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue
 * @return:bool
 */
template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::InsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue)
noexcept(false)
{
	//�����յĲ���ת��Ϊ�ַ���
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
		throw* (new NotFoundSection("δ����section��" + section));
	}
	std::shared_ptr<std::vector<std::string>> temp_value = std::make_shared< std::vector<std::string>>();
	temp_value->push_back(value);
	iter->second.insert(std::make_pair(key, *temp_value));
	return true;
}

/// <summary>
/// �������ϼ���ǩ�����Զ�����ϼ���ǩ
/// </summary>
/// <typeparam name="Type1">section����</typeparam>
/// <typeparam name="Type2">key����</typeparam>
/// <typeparam name="Type3">value����</typeparam>
/// <param name="tempsection">section</param>
/// <param name="tempkey">key</param>
/// <param name="tempvalue">value</param>
/// <returns>������ </returns>
template <typename Type1, typename Type2, typename Type3>
bool rwini::ReadWriteini::AutoInsertKey(const Type2& tempsection, const Type1& tempkey, const Type3& tempvalue)
noexcept(false)
{
	//�����յĲ���ת��Ϊ�ַ���
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
 * @describe:ɾ��section
 * @param const Type1& tempsection
 * @return:bool
 */
 /*
 *2020/6/11�޸ģ�ʹ��ģ����ղ���
 */
template <typename Type1>
bool rwini::ReadWriteini::DeleteSection(const Type1& tempsection)
noexcept(false)
{
	//�����յĲ���ת��Ϊ�ַ���
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
 * @describe:ɾ��Key
 * @param {const char*}
 * @return:bool
 */
template <typename Type1, typename Type2>
bool rwini::ReadWriteini::DeleteKey(const Type1& tempsection, const Type2& tempkey)
{
	//�����յĲ���ת��Ϊ�ַ���
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
 * @name:���캯��
 * @describe:��ʼ����������
 * @param {char*}
 * @return:��
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
	//���������ţ���+1����-1
	int tempSectionBracket = 0;
	enum class LineType
	{
		//��ǰ����section,��ֵ��,ע��,��¼��
		section = 1, key = 2, value = 3, ann = 4, refuse = 5
	};
	std::shared_ptr<LineType> lineType = std::make_shared<LineType>();
	*lineType = LineType::refuse;
	//��ʱ����һ�е�����
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
			//����һ��section
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
	//�ͷ�ö������
	lineType.~shared_ptr();
	//�ر��ļ�
	iniFile.close();
}

rwini::ReadWriteini::~ReadWriteini()
{
	delete iniContent;
}


/*
 * @name:Writeini
 * @describe:д���ļ�
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
		throw* (new NotFoundSection("δ����section��" + temp_section));
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
		throw* (new NotFoundSection("δ����section��" + temp_section));
	}
	auto tempiter = iter->second.find(temp_key);
	if (tempiter == iter->second.end()) {
		throw* (new NotFoundKey("δ��section:" + temp_section + "�·���key��" + temp_key));
	}
	return tempiter;
}
#endif // READWRITEINI_TEMPLATE_
#endif // !READWRITE_H_