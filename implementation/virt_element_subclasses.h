#pragma once
#include"data_structure_definitions.h"
#include"parsing.h"

#include<memory>



class empty_element : public virt_element
{
public:
	empty_element* getempty()
	{
		return this;
	}

	bool holds(node_branch* a)
	{
		return false;
	}

	std::string get_str()
	{
		return "#";
	}

	virtual std::string get_message()
	{
		return "";
	}

	virt_element* duplicate()
	{
		return new empty_element();
	}

	virt_element* exhaust()
	{
		return this;
	}

	virtual bool is_equal(virt_element* a)
	{
		return a->getempty();
	}

};



empty_element main_empty_elem = empty_element();

//is empty for whatever reason
class message_element : public empty_element
{
public:

	message_element* geterrornode() override
	{
		return this;
	}


	std::string get_str()
	{
		return "\""+error_message+"\"";
	}

	void set_message(const std::string& error)
	{
		error_message = error;
	}

	std::string get_message()
	{
		return error_message;
	}

	virt_element* duplicate()
	{
		return new message_element(std::string(error_message));
	}

	virt_element* exhaust()
	{
		return this;
	}

	virtual bool is_equal(virt_element* a)
	{
		if (a->geterrornode() && a->geterrornode()->error_message == error_message)
		{
			return true;
		}
		return false;
	}

	std::string error_message;

	message_element(const std::string& error)
		:
		error_message(error)
	{ }


private:

	message_element() = delete;

};


//if it uses other nodes (which means it can hold branches)
//for example a function (its args), or an operator(its left hand side and right hand side)
class holder : public virt_element
{
public:
	holder* getholder() override
	{
		return this;
	}

	virtual ~holder() {}

	virtual std::string get_str() = 0;

	virtual virt_element* duplicate() = 0;

	virtual virt_element* exhaust() = 0;

	virtual bool is_equal(virt_element* a) = 0;

	virtual node_branch** getnodebranchof(virt_element* a) = 0;

};

virt_element* make_from_section(std::vector<exp_part>::iterator w, std::vector<exp_part>::iterator stop);


// includes logarithms for now
class operation_element : public holder
{
public:

	node_branch** getnodebranchof(virt_element* a)
	{
		if (data.lhs->read_data() == a)
		{
			return &data.lhs;
		}
		else if (data.rhs->read_data() == a)
		{
			return &data.rhs;
		}
		else
		{
			if (data.lhs->read_data()->getnodebranchof(a) != nullptr)
			{
				return data.lhs->read_data()->getnodebranchof(a);
			}
			if (data.rhs->read_data()->getnodebranchof(a) != nullptr)
			{
				return data.lhs->read_data()->getnodebranchof(a);
			}
			return nullptr;
		}

	}

	operation_element* getoperation() override
	{
		return this;
	}

	operation_struct data;

	virt_element* duplicate()
	{
		operation_element* ret = new operation_element();
		ret->data.rhs = new node_branch{ data.rhs->read_data()->duplicate(),true };
		ret->data.lhs = new node_branch{ data.lhs->read_data()->duplicate(),true };
		ret->data.which = data.which;
		return ret;
	}

	virt_element* exhaust()
	{
		if (data.rhs->user_count != 0)
		{
			data.rhs->break_branch();
			data.rhs = new node_branch{ data.rhs->read_data()->duplicate(),true };
		}
		else
		{
			data.rhs->set_data(data.rhs->read_data()->exhaust());
		}

		if (data.lhs->user_count != 0)
		{
			data.lhs->break_branch();
			data.lhs = new node_branch{ data.lhs->read_data()->duplicate(),true };
		}
		else
		{
			data.lhs->set_data(data.lhs->read_data()->exhaust());
		}
		return this;
	}

	bool is_equal(virt_element* a)
	{
		if (a->getoperation())
		{
			if (data.lhs->read_data()->is_equal(a->getoperation()->data.lhs->read_data()) && data.rhs->read_data()->is_equal(a->getoperation()->data.rhs->read_data()))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	operation_element()
	{}

	operation_element(std::vector<exp_part>::iterator loc, std::vector<exp_part>::iterator end, std::vector<exp_part>::iterator begin)
	{
		data.assign_opname(std::get<oper_name>(loc->data));

		auto left = skip_elem_left
		(std::vector<exp_part>::reverse_iterator{ loc },
			std::vector<exp_part>::reverse_iterator{ begin }).base()-1;

		virt_element* lhsval = make_from_section(left,end);

		data.lhs = new node_branch(lhsval, true);

		
		data.rhs = new node_branch(make_from_section(loc + 1, end), true);

	}

	/*
	operation_element(std::string tar, int locofop)
	{

		data.lhs = (new node_branch{ tar,endofval(tar, locofop - 1, false),true });
		data.rhs = (new node_branch{ tar,locofop + 1 ,true });
		data.assign_char(tar[locofop]);

	}
	*/

	bool holds(node_branch* a)
	{
		if (data.lhs == a || data.rhs == a)
		{
			return true;
		}
		else
		{
			if (data.lhs->read_data()->holds(a) || data.rhs->read_data()->holds(a))
			{
				return true;
			}
		}

		return false;
	}

	~operation_element() override
	{
		data.lhs->break_branch();
		data.rhs->break_branch();
	}

	std::string get_str()
	{

		if (data.which == oper_name::log)
		{
			std::string rhsprnt = data.rhs->get_str();

			if (data.rhs->read_data()->getoperation())
			{
				if (data.rhs->read_data()->getoperation()->data.which != oper_name::log)
				{
					rhsprnt = rhsprnt.substr(1, rhsprnt.size() - 2);
				}
			}
			if (data.lhs->read_data()->getfunction())
			{
				return "log(" + data.lhs->get_str() + ")(" + rhsprnt + ")"; //this|
			}//																		|
			else if (data.lhs->read_data()->getoperation())//							|
			{//																		|
				if (data.lhs->read_data()->getoperation()->data.which == oper_name::log)//	|
				{//																	|
					return "log(" + data.lhs->get_str() + ")(" + rhsprnt + ")"; //|and this adds parenthases to avoid awkward looking logarithm bases e.g. logf(x)(2) will be log(f(x))(2)
				}
				else
				{
					return "log" + data.lhs->get_str() + "(" + rhsprnt + ")";
				}
			}
			else
			{
				return "log" + data.lhs->get_str() + "(" + rhsprnt + ")";
			}
		}
		else
		{
			return "(" + data.lhs->get_str() + char(data.which) + data.rhs->get_str() + ")";
		}
	}

};

class function_element : public holder
{
public:

	node_branch** getnodebranchof(virt_element* a)
	{
		for (int i = 0; i != data.args.size(); ++i)
		{
			if (data.args[i]->read_data() == a)
			{
				return &data.args[i];
			}
		}
		for (int i = 0; i != data.args.size(); ++i)
		{
			if (data.args[i]->read_data()->getnodebranchof(a) != nullptr)
			{
				return data.args[i]->read_data()->getnodebranchof(a);
			}
		}
		return nullptr;
	}

	function_element* getfunction()
	{
		return this;
	}
	
	bool holds(node_branch* a)
	{
		
		for (int i = 0; i != data.args.size(); ++i)
		{
			if (data.args[i]==a)
			{
				return true;
			}
		}
		for (int i = 0; i != data.args.size(); ++i)
		{
			if (data.args[i]->read_data()->holds(a))
			{
				return true;
			}
		}
		return false;
	}

	function_element()
	{}

	function_element(std::vector<exp_part>::iterator w, std::vector<exp_part>::iterator stop)
	{
		data.function_name = std::get<std::string>(w->data);
		++w;
		while (w->typeof != exp_part::function_end) //safe because the parsing would have caught any unfinished functions.
		{
			++w;
			data.args.push_back(new node_branch(make_from_section(w, stop), true));
			w = 1 + skip_elem_right(w, stop);
		}
	}

	/*
	function_element(const std::string& tar, int firstchar)
	{
		while (tar[firstchar] != '(')
		{
			data.function_name.push_back(tar[firstchar]);
			++firstchar;
		}
		++firstchar;
		while (tar[firstchar] != ')')
		{
			data.args.push_back(new node_branch{ tar,firstchar,true });
			firstchar = endofval(tar, firstchar);
			++firstchar;
		}

	}
	*/

	virt_element* duplicate()
	{
		function_element* ret = new function_element();

		ret->data.function_name = data.function_name;

		for (int i = 0; i != data.args.size(); ++i)
		{
			ret->data.args.push_back(new node_branch(data.args[i]->read_data()->duplicate(),true));
		}

		return ret;
	}

	virt_element* exhaust()
	{
		for (int i = 0; i != data.args.size(); ++i)
		{
			if (data.args[i]->user_count != 0)
			{
				data.args[i]->break_branch();
				data.args[i] = new node_branch(data.args[i]->read_data()->duplicate(), true);
			}
			else
			{
				data.args[i]->set_data(data.args[i]->read_data()->exhaust());
			}
		}
		return this;
	}

	bool is_equal(virt_element* a)
	{
		if (a->getfunction() && a->getfunction()->data.function_name == data.function_name && a->getfunction()->data.args.size())
		{
			for (int i = 0; i != a->getfunction()->data.args.size();++i)
			{
				if (!data.args[i]->read_data()->is_equal(a->getfunction()->data.args[i]->read_data()))
				{
					return false;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	std::string get_str()
	{
		std::string ret = "" + data.function_name;

		if (data.args.size() == 1 && data.args[0]->read_data()->getoperation())
		{
			if (data.args[0]->read_data()->getoperation()->data.which != oper_name::log)
			{
				
			}
			else
			{
				ret += "(";
			}
		}
		else
		{
			ret += "(";
		}

		for (int i = 0; i != data.args.size(); ++i)
		{
			if (i != 0)
			{
				ret += ",";
			}
			ret += data.args[i]->get_str();
		}
		if (data.args.size() == 1 && data.args[0]->read_data()->getoperation())
		{
			if (data.args[0]->read_data()->getoperation()->data.which != oper_name::log)
			{
				
			}
			else
			{
				ret += ")";
			}
		}
		else
		{
			ret += ")";
		}
		return ret;
	}


	~function_element()
	{
		for (int i = 0; i != data.args.size(); ++i)
		{
			data.args[i]->break_branch();
		}
	}

	
	function_struct data;
};



class lone : public virt_element
{
public:

	virtual lone* getlone() override
	{
		return this;
	}

	bool holds(node_branch* a)
	{
		return false;
	}

	virtual virt_element* duplicate() = 0;

	virt_element* exhaust()
	{
		return this;
	}

	virtual bool is_equal(virt_element* a) = 0;

};

class constant_element : public lone
{
public:

	constant_element(std::string a)
	{
		data.assign_str(a);
	}

	constant_element(num_type a)
	{
		data.assign_value(a);
	}

	constant_element()
	{}

	virt_element* duplicate()
	{
		constant_element* ret = new constant_element();
		ret->data.assign_value(data.get_value());
		return ret;
	}

	bool is_equal(virt_element* a)
	{
		if (a->getnumber()&&a->getnumber()->data.get_value()==data.get_value())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual constant_element* getnumber() override
	{
		return this;
	}

	std::string get_str()
	{
		return data.get_str();
	}
	
	constant_struct data;

};

class variable_element : public lone
{
public:
	variable_element* getvariable() override
	{
		return this;
	}

	variable_element(char a)
	{
		data.assign_char(a);
	}

	variable_element(var_name a)
	{
		data.assign_char(char(a));
	}

	variable_element()
	{}

	virt_element* duplicate()
	{
		variable_element* ret = new variable_element();
		ret->data.assign_char(data.get_char());
		return ret;
	}

	bool is_equal(virt_element* a)
	{
		if (a->getvariable() && a->getvariable()->data.get_char() == data.get_char())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	std::string get_str()
	{
		return data.get_str();
	}


	variable_struct data;

};


virt_element* make_from_section(std::vector<exp_part>::iterator w, std::vector<exp_part>::iterator stop)
{
	switch (w->typeof)
	{
	case(exp_part::number):
		return new constant_element(std::get<num_type>(w->data));
		break;

	case(exp_part::variable):
		return new variable_element(std::get<var_name>(w->data));
		break;

	case(exp_part::empty):
		return &main_empty_elem;
		break;

	case(exp_part::message):
		return new message_element(std::get<std::string>(w->data));
		break;

	case(exp_part::function_name):
		return new function_element(w, stop);
		break;

	case(exp_part::paren_open):
		if (w + 1 == stop)
		{
			return new message_element(error_name::failed_string_parsing);
		}
		else
		{
			++w;
			auto e = skip_elem_right(w, stop);
			if (e == stop || e + 1 == stop)
			{
				return new message_element(error_name::failed_string_parsing);
			}
			
			if ((e + 1)->typeof == exp_part::paren_close)
			{
				return make_from_section(w, e+1);
			}
			else if ((e + 1)->typeof == exp_part::operation)
			{
				return new operation_element(e + 1, stop,w-1);
			}
			else
			{
				return new message_element(error_name::failed_string_parsing);
			}
		}
		break;

	default:
		return new message_element(error_name::failed_string_parsing);

	}
}

virt_element* make_tree(std::string::const_iterator beg, std::string::const_iterator end,parsing_rule_set const& r)
{
	auto parsed = full_parse(beg, end, r);
	if (parsed.size() == 0)
	{
		return &main_empty_elem;
	}
	else if (parsed[0].typeof == exp_part::parsing_error)
	{
		return new message_element(error_name::failed_string_parsing);
	}
	return make_from_section(parsed.begin(), parsed.end());
}




/*
virt_element* make_tree(const std::string& a, int index)
{

	if (a.size() == 0)
	{
		return new message_element(error_name::failed_string_parsing);
	}
	else if (a[index] == '(')
	{
		if (a[index + 1] == '$')// $ after an open paren indicates that the open paren and its corrosponding close paren contain a single function
		{
			return new function_element(a, index + 2);
		}
		else
		{
			return new operation_element(a, endofval(a, index + 1));
		}
	}
	else if (isvarchar(a[index]))
	{
		return new variable_element(a[index]);
	}
	else if (isnumchar(a[index]))
	{
		return new constant_element(a.substr(index, endofval(a, index)));
	}
	else
	{
		return new message_element(error_name::failed_string_parsing);
	}
}

inline node_branch::node_branch(const std::string& a, int index, bool created_by_holder)
{

	if (a.size() == 0)
	{
		owned = new message_element(error_name::failed_string_parsing);
	}
	else if (a[index] == '(')
	{
		if (a[index + 1] == '$')// $ after an open paren indicates that the open paren and its corrosponding close paren contain a single function
		{
			owned = new function_element(a, index + 2);
		}
		else
		{
			owned = new operation_element(a, endofval(a, index + 1));
		}
	}
	else if (isvarchar(a[index]))
	{
		owned = new variable_element(a[index]);
	}
	else if (isnumchar(a[index]))
	{
		owned = new constant_element(a.substr(index, endofval(a, index)));
	}
	else
	{
		owned = new message_element(error_name::failed_string_parsing);
	}
	is_root = !created_by_holder;
	if (!created_by_holder)
	{
		user_count = 1;
	}
	else
	{
		user_count = 0;
	}
}
*/

inline node_branch::~node_branch()
{
	if (owned != &main_empty_elem)
	{
		delete owned;
	}
}
