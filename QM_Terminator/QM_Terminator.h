#pragma once

#include "stdafx.h"
#include "quest.h"
#include "ui_QM_Terminator.h"

class QM_Terminator : public QWidget
{
	Q_OBJECT

public:
	QM_Terminator(QWidget *parent = Q_NULLPTR);
	boost::asio::io_service io_se;
	boost::asio::ip::tcp::resolver resolver{ io_se };
	boost::asio::ip::tcp::socket socket{ io_se };
	Quest current_quest;

	const std::string host = "qm.linyisong.top";		//要访问的主机名
	const std::string port = "80";					//http服务端口
	const std::string ua = "NintendoSwitch-core(Golden Edition) yiban_iOS"; //浏览器标识
	std::string url = "";
	std::string cookie = "";
	std::string uuid = "";
	int unit = 2;
	const int subject[13] = { 2,7,8,9,10,12,15,16,19,47,48,49,-1 };

	//2、7、8、9、10、12、15、16、19、47、48、-1

private:
	Ui::MainForm ui;
	void build_connection();
	bool get_cookie();
	bool get_unit();
	void get_quest();
	int answer_the_question();
	void closeEvent(QCloseEvent* event);

public slots:
	void btn_submit_click();
	void btn_next_question_click();
	void btn_answer_click();
	void btn_cheat_click();
	void cb_unit_change(int);
	void cb_change_font_size(QString);
	void le_answer_input(QString);
};
