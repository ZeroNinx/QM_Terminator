#pragma once

#include "stdafx.h"

class Answer
{
public:
	Answer(std::string uuid,std::string json,std::string answer);
	std::string right_answer;//正确选项
	std::string uuid;//题号
	bool is_right;//是否正确
};

