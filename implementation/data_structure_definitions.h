#pragma once
#include<string>
#include<sstream>
#include<vector>

#include"node_branch.h"

#include"..\definitions.h"



struct constant_struct
{
	void assign_str(std::string a)
	{
		std::stringstream s;
		s << a;
		s >> value;
	}

	std::string get_str()
	{
		std::stringstream s;
		s << value;
		return s.str();
	}

	void assign_value(num_type a)
	{
		value = a;
	}

	num_type get_value()
	{
		return value;
	}


	num_type value;
};




struct operation_struct
{
	void assign_char(char a)
	{
		which = oper_name(a);
	}

	char get_char()
	{
		return char(which);
	}


	void assign_opname(oper_name a)
	{
		which = a;
	}

	oper_name get_opname()
	{
		return which;
	}


	oper_name which;

	node_branch* lhs;
	node_branch* rhs;


};



struct variable_struct
{
	char get_char()
	{
		return name;
	}

	void assign_char(char a)
	{
		name = a;
	}

	std::string get_str()
	{
		return { name };
	}


	char name;

};



struct function_struct
{
	std::string function_name;

	std::vector<node_branch*> args;




};


