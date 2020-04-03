#include "answer.h"

using namespace std;
using namespace boost::property_tree;

Answer::Answer(std::string uuid, std::string json, std::string answer)
{
	this->uuid = uuid;
	ptree root, data;
	stringstream ss(json);
	read_json(ss, root);
	data = root.get_child("data");
	string rightAnswer = data.get<string>("rightAnswer");
	is_right = (rightAnswer == "true");
	if (is_right)
		right_answer = answer;
	else
		right_answer = data.get<string>("rightOption")[0];
}
