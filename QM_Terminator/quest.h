#pragma once

#include "stdafx.h"

class Quest
{
public:
	Quest(std::string);//���캯��
	std::string uuid;//���
	std::string type;//��Ŀ����
	int opt_count;//ѡ�����
	std::string content;//��Ŀ����
	std::string opt[15];//ѡ������
};

