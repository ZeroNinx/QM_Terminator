#include "QM_Terminator.h"
#include "quest.h"
#include "answer.h"
#include "sqlite3.h"

using namespace std;
using namespace boost::property_tree;
using namespace boost::beast::http;
#define display(x) ui.pte_message->appendPlainText(x)

class Quest;

//构造函数、初始化
QM_Terminator::QM_Terminator(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(width(), height());
	ui.pte_message->setReadOnly(true);
	ui.cb_change_font_size->setCurrentIndex(7);
	
	fstream fs;
	fs.open("cache.cfg", ios::in);
	if (fs)
	{
		string saved_url;
		fs >> saved_url;
		ui.le_url->setText(qs(saved_url));
	}
	fs.close();

	
	build_connection();
}

//建立连接
void QM_Terminator::build_connection()
{
	
	//查找域名
	auto const results = resolver.resolve(host, port);

	// 链接找到的域名
	boost::asio::connect(socket, results.begin(), results.end());
}

//取得验证Cookie
bool QM_Terminator::get_cookie()
{
	try
	{
		string target = url.substr(23, url.length() - 1);
		while (target[target.length() - 1] == ' '|| target[target.length() - 1] == '\n')
			target.pop_back();
		int version = 11;

		//设定GET请求
		boost::asio::streambuf request;
		std::ostream request_stream(&request);

		//http头
		request_stream << "GET " << target << " HTTP/1.1 \r\n";
		request_stream << "Host: " << host << "\r\n";
		request_stream << "Accept: application/json \r\n";
		request_stream << "Accept-Encoding: gzip, deflate, br \r\n";
		request_stream << "Conection: keep-alive \r\n";
		request_stream << "Content-Type: application/x-www-form-urlencoded \r\n";
		request_stream << "User-Agent: " << ua << " \r\n";
		if(!cookie.empty())
			request_stream << "Cookie: " << cookie << " \r\n";
		request_stream << "\r\n";//务必加上标记结束
		if(cookie.empty())
			ui.pte_message->appendPlainText(qs("侦查中......"));
		// 发送request
		write(socket, request);

		//声明一个容器来保存响应
		boost::beast::flat_buffer buf;
		response<string_body> resp;
		stringstream ss;

		//接收容器
		read(socket, buf, resp);

		//提取cookie
		if (cookie.empty())
		{
			ss << resp[field::set_cookie];
			ss >> cookie;
			cookie = cookie.substr(0, 43);
			ui.pte_message->appendPlainText(qs("取得伪装：") + qs(cookie));
		}
	}
	catch (std::exception const& e)
	{
		//输出错误
		ui.pte_message->appendPlainText(qs((std::string)"错误：" + e.what()));
		ui.pte_message->appendPlainText(qs("好像潜入失败了......也许是神秘连接出错了？"));
	}
	return true;
}

//取得单元
bool QM_Terminator::get_unit()
{
	try
	{

		const string target = "/yiban-web/stu/toSubject.jhtml?courseId="+to_string(unit);	//目标
		int version = 11;

		//设定GET请求
		boost::asio::streambuf request;
		std::ostream request_stream(&request);

		//http头
		request_stream << "GET " << target << " HTTP/1.1 \r\n";
		request_stream << "Host: " << host << "\r\n";
		request_stream << "Accept: application/json \r\n";
		request_stream << "Accept-Encoding: gzip, deflate, br \r\n";
		request_stream << "Conection: keep-alive \r\n";
		request_stream << "User-Agent: " << ua << " \r\n";
		request_stream << "Cookie: " << cookie << " \r\n";
		request_stream << "\r\n";//务必加上标记结束

		// 发送request
		write(socket, request);

		//声明一个容器来保存响应
		boost::beast::flat_buffer buf;
		response<string_body> resp;
		stringstream ss;

		read(socket, buf, resp);
		
		/*ss << resp.body();
		ui.pte_message->appendPlainText(qs8(ss.str()));
		ui.pte_message->appendPlainText(qs8(to_string(resp.body().length())));*/
		ui.pte_message->appendPlainText(qs("课程设置成功，当前课程：")+ui.cb_unit->currentText());
		return true;
	}
	catch (std::exception const& e)
	{
		//输出错误
		ui.pte_message->appendPlainText(qs((std::string)"错误：" + e.what()));
		return false;
	}
}

//取得题目
void QM_Terminator::get_quest()
{
	try
	{
		const string target = "/yiban-web/stu/nextSubject.jhtml";	//目标
		int version = 11;

		//设定POST请求
		boost::asio::streambuf request;
		std::ostream request_stream(&request);

		//http头
		request_stream << "POST " << target << " HTTP/1.1 \r\n";
		request_stream << "Host: " << host << "\r\n";
		request_stream << "Accept: application/json \r\n";
		request_stream << "Accept-Encoding: gzip, deflate, br \r\n";
		request_stream << "Conection: keep-alive \r\n";
		request_stream << "Content-Type: application/x-www-form-urlencoded \r\n";
		request_stream << "Content-Length: "<< 10+to_string(unit).length() << " \r\n";
		request_stream << "User-Agent: " << ua << " \r\n";
		request_stream << "Cookie: " << cookie << " \r\n\r\n";
		request_stream << "courseId=" << unit << "\r\n";

		// 发送request
		write(socket, request);

		//声明一个容器来保存响应
		boost::beast::flat_buffer buf;
		response<string_body> resp;
		stringstream ss;

		read(socket, buf, resp);
		ss << resp.body();

		//创建题目
		Quest q(ss.str());
		current_quest = q;
		uuid = q.uuid;

		ui.pte_message->appendPlainText(qs("------------------------------------------------------------"));
		ui.pte_message->appendPlainText(qs("\n")+qs8(q.content)+qs("\n"));//显示题型
		
		ffor(i, 0, q.opt_count-1)//显示选项
		{
			string index;
			index.push_back('A' + i);
			index.push_back('.');
			ui.pte_message->appendPlainText(qs(index)+qs8(q.opt[i]));
		}
		ui.pte_message->appendPlainText(qs("\n题型：") + qs8(q.type));
	}
	catch (std::exception const& e)
	{
		//输出错误
		ui.pte_message->appendPlainText(qs((string)"错误：" + e.what()));
		ui.pte_message->appendPlainText(qs("尝试连接..."));
		build_connection();
	}
}

//回答问题
int QM_Terminator::answer_the_question()
{
	bool answer_empty = ui.le_answer->text().isEmpty();
	if (answer_empty)
	{
		see(qs("请输入答案!"));
		return -1;
	}
	try
	{
		const string target = "/yiban-web/stu/changeSituation.jhtml";	//目标
		int version = 11;

		//编辑POST信息
		string info = "";
		info += "answer=" + ui.le_answer->text().toStdString();
		info += "&courseId=" + to_string(unit);
		info += "&uuid=" + uuid;
		info += "&deviceUuid=";

		//request头
		request<string_body> r{ verb::post,target,11 };
		r.set(field::host, host);
		r.set(field::user_agent, ua);
		r.set(field::cookie, cookie);
		r.set(field::connection, "keep-alive");
		r.set(field::content_type, "application/x-www-form-urlencoded");
		r.set(field::content_length, info.length());
		r.body() = info;

		// 发送request
		write(socket, r);

		//声明一个容器来保存响应
		boost::beast::flat_buffer buf;
		response<string_body> resp;
		stringstream ss;

		read(socket, buf, resp);
		ss << resp.body();
		//display(qs8(ss.str()));
		Answer ans(uuid,ss.str(),ui.le_answer->text().toStdString());
		if (ans.is_right)
		{
			ui.pte_message->appendPlainText(qs("答对啦！积分+1"));
			ui.sb_right_counter->setValue(ui.sb_right_counter->value() + 1);
		}
		else 
			ui.pte_message->appendPlainText(qs("答错了，正确答案是：") + qs(ans.right_answer));
		ui.sb_total_counter->setValue(ui.sb_total_counter->value() + 1);
		char rate[10];
		sprintf(rate, "%.2f", ui.sb_right_counter->value() * 1.0 / (ui.sb_total_counter->value() * 1.0)*100);
		if (ui.sb_total_counter->value() != 0)
			ui.lbl_rate->setText(qs(rate)+qs("%"));
		return 1;
	}
	catch (std::exception const& e)
	{
		//输出错误
		ui.pte_message->appendPlainText(qs((string)"错误：" + e.what()));
		ui.pte_message->appendPlainText(qs("尝试连接..."));
		build_connection();
		return 0;
	}
}

//作弊
void QM_Terminator::btn_cheat_click()
{
	sqlite3* sqlite = NULL;
	sqlite3_stmt* stmt = NULL;
	string sql = "select answer from mao where subDescript='" +current_quest.content + "'";
	int res = sqlite3_open("tk.db", &sqlite);
	if (res != SQLITE_OK)
	{
		display(qs("数据库读取失败！"));
		return;
	}
	res = sqlite3_prepare(sqlite, sql.c_str(), -1, &stmt, NULL);
	if (res != SQLITE_OK)
	{
		display(qs("数据库出错了！"));
		return;
	}
	display(qs("作弊中..."));
	//ui.le_answer->clear();
	bool found = false;
	string ans;
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		found = true;
		ans = (char*)sqlite3_column_text(stmt, 0);
	}
	if (found)
	{
		ffor(i, 0, current_quest.opt_count - 1)//显示选项
		{
			if (current_quest.opt[i] == ans)
			{
				string res = "A";
				res[0] += i;
				display(qs("正确答案：")+qs(res+".") + qs8(ans));
				ui.le_answer->setText(qs(res));
				break;
			}
		}
	}
	else display(qs("没找到呐....."));
}

//登录按钮
void QM_Terminator::btn_submit_click()
{
	url = ui.le_url->text().toStdString();
	if (url.empty())
	{
		see(qs("请输入神秘连接！"));
		return;
	}
	unit = subject[ui.cb_unit->currentIndex()];

	fstream fs;
	fs.open("cache.cfg",ios::out);
	if (fs)
		fs << ui.le_url->text().toStdString();
	fs.close();

	ui.pte_message->appendPlainText(qs("正在潜入敌方总部......"));
	ui.pte_message->appendPlainText(qs("您的浏览器是：")+qs(ua));
	if (get_cookie())
	{
		//再次验证以正式登陆
		get_cookie();

		//确定当前单元
		get_unit();

		//绝地求生
		get_quest();
	}
	else ui.pte_message->appendPlainText(qs("认证未通过"));
}

//改变课程按钮
void QM_Terminator::cb_unit_change(int index)
{
	unit = subject[index];
	get_unit();
	get_quest();
}

//下一题按钮
void QM_Terminator::btn_next_question_click()
{
	if (cookie.empty())
		see(qs("还没登录哦"));
	else get_quest();
}

//回答按钮
void QM_Terminator::btn_answer_click()
{
	if (answer_the_question() != -1)
		get_quest();
}

//选择字号
void QM_Terminator::cb_change_font_size(QString font_size)
{
	ui.pte_message->setStyleSheet(qs("font-size:")+font_size);
}

//限定格式
void QM_Terminator::le_answer_input(QString answer)
{
	if (!answer.isEmpty())
	{
		string ans = answer.toStdString();
		if (ans[0] >= 'a' && ans[0] <= 'z')
		{
			ans[0] = ans[0]- 'a' + 'A';
			ui.le_answer->setText(qs(ans));
		}
	}
}

//关闭事件
void QM_Terminator::closeEvent(QCloseEvent* event)
{
	//优雅地关闭套接字s
	boost::system::error_code ec;
	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

	//检查错误
	if (ec && ec != boost::system::errc::not_connected)
		throw boost::system::system_error{ ec };

	//到这里连接就关闭了

}
