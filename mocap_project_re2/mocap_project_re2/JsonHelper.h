#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <json\json_struct.h>


namespace JsonHelper {
	template<typename T>
	T LoadObjectFromFile(std::string&& path)
	{
		using namespace std;
		ifstream json_file(path);
		string json_string((istreambuf_iterator<char>(json_file)), istreambuf_iterator<char>());
		JS::ParseContext context(json_string);
		T obj;
		context.parseTo(obj);
		return obj;
	}
}