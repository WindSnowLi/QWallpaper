/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ** * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ** * * * * * * * * * * * * * */
//
//				与窗口无关的一些功能性函数
//
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ** * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * ** * * * * * * * * * * * * * */
#ifndef TOOL_H_
#define TOOL_H_
#include <QtCore\qstring.h>
#include <vector>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <thread>
namespace tool {
	//QString转CString
	std::wstring QSTOCS(QString qs);
	//获取当前系统用户名
	QString GetSystemUserName();
	//获取当前系统壁纸路径
	QString GetSystemWallpaperPath();
	//清理无效文件路径
	void CleanVoidFilelist(std::vector<std::string>& filelist);
	//判断文件是否存在
	bool JudgeFile(char* path);
	//Qt中读取的路径播放器无法直接使用，将/替换成‘\\\\’
	void PathConvert(QString& ch);
	//自启动设置		参数int 0为取消自启动，1为设置自启动，2为查询自启动，自启动返回true,不是自启动返回false
	bool OperationAutoStart(int operation);
	//获取程序路径
	QString GetProgramPath();
	//获取一个有效的文件
	std::string GetValidFilePath(std::vector <std::string>& filelist, int& linenumber);
	//序列化写入对象
	template<typename T>
	void BoostWrite(QString  write_boost_file_path, T& write_target_object);
	//读取序列化对象
	template<typename T>
	void BoostRead(QString  read_boost_file_path, T& read_target_object);
}

/************************************************************************/
//模板函数放置头文件，以防编译链接出错
/************************************************************************/
/*
*┌────────────────────────────────────────────────┐
*│　描    述：序列化
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：(QString, T  &)
*└────────────────────────────────────────────────┘
*/
template<typename T>
void tool::BoostWrite(QString write_boost_file_path, T& write_target_object)
{
	//序列化输入到配置文件
	std::ofstream write_targetConfig_file(write_boost_file_path.toStdString());
	boost::archive::xml_oarchive oa(write_targetConfig_file);
	oa& BOOST_SERIALIZATION_NVP(write_target_object);
	//延迟100ms，以防文件写入缺失
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/*
*┌────────────────────────────────────────────────┐
*│　描    述：反序列化
*│　作    者：windSnowLi
*│　版    本：0.0.1
*│　创建时间：2020/05/31
*└────────────────────────────────────────────────┘
*┌────────────────────────────────────────────────┐
*│　命名空间：tool
*│　类    名：tool
*│　参    数：(QString, T  &)
*└────────────────────────────────────────────────┘
*/
template<typename T>
void tool::BoostRead(QString read_boost_file_path, T& read_target_object)
{
	std::ifstream read_targetConfig_file(read_boost_file_path.toStdString());
	boost::archive::xml_iarchive ia(read_targetConfig_file);
	ia& BOOST_SERIALIZATION_NVP(read_target_object);
}
#endif // !TOOL_H_