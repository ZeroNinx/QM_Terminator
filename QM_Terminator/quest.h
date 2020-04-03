#pragma once

#include "stdafx.h"

class Quest
{
public:
	Quest(std::string);//构造函数
	std::string uuid;//题号
	std::string type;//题目类型
	int opt_count;//选项个数
	std::string content;//题目内容
	std::string opt[15];//选项内容
};

