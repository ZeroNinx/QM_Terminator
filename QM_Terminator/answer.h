#pragma once

#include "stdafx.h"

class Answer
{
public:
	Answer(std::string uuid,std::string json,std::string answer);
	std::string right_answer;//��ȷѡ��
	std::string uuid;//���
	bool is_right;//�Ƿ���ȷ
};

