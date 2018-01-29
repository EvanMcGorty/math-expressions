#pragma once
#include<string>
#include<vector>

//num type should eventually be an infinite precision whole number
using num_type = unsigned long long;

enum class oper_name : char
{
	add = '+', sub = '-', mul = '*', div = '/', exp = '^', log = '_'
};


enum class var_name : char
{
	a = 'a', b = 'b', c = 'c', d = 'd', e = 'e', f = 'f', g = 'g', h = 'h', i = 'i', j = 'j', k = 'k', l = 'l', m = 'm', n = 'n', o = 'o', p = 'p', q = 'q', r = 'r', s = 's', t = 't', u = 'u', v = 'v', w = 'w', x = 'x', y = 'y', z = 'z'
};


namespace error_name
{
	char const* const failed_string_parsing("bad string error");
	char const* const bad_number_assignment("bad number error");
	char const* const bad_character_assignment("bad char error");
	char const* const invalid_oper_name_assignment("bad operator assignment error");
	char const* const subtree_assignment("bad node movement error");
	char const* const bad_function_name_characters("bad function name error");
	char const* const ambiguous_error("some error");
	char const* const recursing_expression("recursive expression error");
	char const* const nullnode_assigned("bad nullnode use error");
};


//rules for expression parsing
struct parsing_rule_set
{
	std::vector<std::string> function_names; //log counts as a function name for things like log2(8) or log(x)(8), but natural logarithm must be explicitly another function such as loge

	bool use_precedence;

	//use an "@" to indicate the target of the negative sign. 
	//This will literally be plopped around any argument of a negative sign, so ensure that it is surrounded with parenthases.
	//e.g. "(0-@)"
	std::string negative_sign_template; 

	static parsing_rule_set more_functions()
	{
		parsing_rule_set ret;

		ret.function_names = { "log","loge","sin","cos","tan","f","g","h","abs","or","sum","mean","median","mode","avg","sqrt","root","iqr","range","mod","stdev","max","min","fact","product","comp","inv"};
		ret.use_precedence = true;
		ret.negative_sign_template = "((0-1)*@)";

		return ret;
	}

	static parsing_rule_set typical_rules()
	{
		parsing_rule_set ret;

		ret.function_names = { "log","loge","sin","cos","tan","f","g","h","abs","or" };
		ret.use_precedence = true;
		ret.negative_sign_template = "((0-1)*@)";

		return ret;
	}

	static parsing_rule_set most_basic_rules()
	{
		parsing_rule_set ret;

		ret.use_precedence = false;
		ret.function_names = { "log" };

		return ret;
	}

};

//not yet implemented
struct printing_rule_set
{

	static printing_rule_set typical_rules()
	{
		printing_rule_set ret;
		return ret;
	}
};