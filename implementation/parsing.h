#pragma once
#include<string>
#include<locale>
#include<vector>
#include<variant>
#include<sstream>
#include<iterator>

#include"..\definitions.h"



struct exp_part
{

	enum part_type_name
	{
		paren_open, //contains nothing
		paren_close, //contains nothing
		number, //contains num_type
		decimal_point, //contains a num type saying how many 0s it has following it
		negative, //contains nothing
		character, //contains a char
		variable, //contains a var_name
		operation, //contains an oper_name
		function_name, //contains std::string
		function_start, //contains nothing
		function_comma, //contains nothing
		function_end, //contains nothing
		place_holder, //contains nothing
		empty, //contains nothing
		message, //contains string
		parsing_error, //contains message 
	};


	part_type_name typeof;

	std::variant<num_type,std::string,char,var_name,oper_name> data;
};

std::vector<exp_part>::iterator skip_elem_right(std::vector<exp_part>::iterator start, std::vector<exp_part>::iterator bounds);

std::vector<exp_part>::reverse_iterator skip_elem_left(std::vector<exp_part>::reverse_iterator start, std::vector<exp_part>::reverse_iterator bounds);

std::vector<exp_part> full_parse(std::string::const_iterator a, std::string::const_iterator stop, parsing_rule_set const& rules);



/*
#include"parsing2.h"

#include<iostream>

int main()
{
while (true)
{
std::string inp;
std::getline(std::cin, inp);
auto a = full_parse(inp.begin(), inp.end(), parsing_rule_set::typical_rules());
for (int i = 0; i != a.size(); ++i)
{

switch (a[i].typeof)
{
case(exp_part::character):
std::cout << "character: " << std::get<char>(a[i].data) << std::endl;
break;
case(exp_part::number):
std::cout << "number: " << std::get<num_type>(a[i].data) << std::endl;
break;
case(exp_part::function_name):
std::cout << "function name: " << std::get<std::string>(a[i].data) << std::endl;
break;
case(exp_part::variable):
std::cout << "variable: " << char(std::get<var_name>(a[i].data)) << std::endl;
break;
case(exp_part::operation):
std::cout << "operation: " << char(std::get<oper_name>(a[i].data)) << std::endl;
break;
case(exp_part::paren_open):
std::cout << "open_paren" << std::endl;
break;
case(exp_part::paren_close):
std::cout << "close_paren" << std::endl;
break;
case(exp_part::function_start):
std::cout << "function_start" << std::endl;
break;
case(exp_part::function_end):
std::cout << "function_end" << std::endl;
break;
case(exp_part::function_comma):
std::cout << "comma" << std::endl;
break;
case(exp_part::empty):
std::cout << "empty" << std::endl;
break;
case(exp_part::negative):
std::cout << "negative sign" << std::endl;
break;
case(exp_part::decimal_point):
std::cout << "decimal point: " << std::get<num_type>(a[i].data) << std::endl;
break;
default:
std::cout << std::get<std::string>(a[i].data) << std::endl;
}

}
}
}

*/