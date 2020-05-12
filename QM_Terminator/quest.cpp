#include "quest.h"

using namespace std;
using namespace boost::property_tree;

Quest::Quest(std::string s)
{
	ptree json;
	stringstream ss(s);
	read_json(ss, json);
	ptree data = json.get_child("data");
	ptree next_subject=data.get_child("nextSubject");

	uuid = data.get<string>("uuid");
	type = next_subject.get<string>("subType");
	opt_count = next_subject.get<int>("optionCount");
	content = next_subject.get<string>("subDescript");
	ffor(i, 0, opt_count - 1)
		opt[i] = next_subject.get<string>((string)"option" + to_string(i));
}
