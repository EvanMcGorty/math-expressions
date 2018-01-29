#include "..\expressions.h"
#include "virt_element_subclasses.h"

//if constexpr (err_t != error_tech::no_error_check)
//if constexpr (err_t == error_tech::silent_empty_conversion)
//else if constexpr (err_t == error_tech::exception_throw)
//else {static_assert(0);}


raw_tree::raw_tree(elem a)
{
	res = a.stuff->duplicate();
}

raw_tree::raw_tree(virt_element*& a)
{
	res = a;
	a = &main_empty_elem;
}

void raw_tree::operator=(raw_tree&& a)
{
	res = a.res;
	a.res = &main_empty_elem;
}

raw_tree::raw_tree(raw_tree&& a)
{
	res = a.res;
	a.res = &main_empty_elem;
}

raw_tree::~raw_tree()
{
	if (res != &main_empty_elem)
	{
		delete res;
	}
}


//initialization with an empty;
elem::elem()
{
	stuff = &main_empty_elem;
}

elem::~elem()
{
	if (stuff != &main_empty_elem)
	{
		delete stuff;
	}
	else
	{
		//do nothing
	}
}

//initialization by parsing a string.
elem::elem(std::string const& expression, parsing_rule_set const& rules)
{

	stuff = make_tree(expression.begin(), expression.end(),rules);

}

//initialization by parsing a string.
elem::elem(std::string::const_iterator expression_begin, std::string::const_iterator expression_end, parsing_rule_set const& rules)
{
	
	stuff = make_tree(expression_begin,expression_end,rules);

}

elem::elem(raw_tree&& a)
{
	stuff = a.res;
	a.res = &main_empty_elem;
}

template<error_tech err_t = default_error_tech>
raw_tree constant::make(num_type const& numerical_value)
{
	elem ret{ new constant_element() };
	ret.stuff->getnumber()->data.assign_value(numerical_value);
	return raw_tree(ret.stuff);
}

template raw_tree constant::make<error_tech::no_error_check>(num_type const& numerical_value);

template raw_tree constant::make<error_tech::exception_throw>(num_type const& numerical_value);

template raw_tree constant::make<error_tech::silent_empty_conversion>(num_type const& numerical_value);


template<error_tech err_t = default_error_tech>
raw_tree constant::make(int const& integer_val)
{
	
	if constexpr(err_t != error_tech::no_error_check)
	{
		if (integer_val < 0)
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				elem ret{ new message_element(error_name::bad_number_assignment) };
				return raw_tree(ret.stuff);
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::bad_number_assignment);
			}
			else { static_assert(0); }
		}
	}
	
	elem ret{ new constant_element() };
	ret.stuff->getnumber()->data.assign_value(integer_val);
	return raw_tree(ret.stuff);
}

template raw_tree constant::make<error_tech::no_error_check>(int const& integer_val);

template raw_tree constant::make<error_tech::exception_throw>(int const& integer_val);

template raw_tree constant::make<error_tech::silent_empty_conversion>(int const& integer_val);


template<error_tech err_t = default_error_tech>
raw_tree variable::make(var_name const& variable_name)
{
	if constexpr (err_t != error_tech::no_error_check)
	{
		std::locale l;
		if (!std::islower(variable_name, l))
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				elem ret{ new message_element(error_name::bad_character_assignment) };
				return raw_tree(ret.stuff);
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::bad_character_assignment);
			}
			else {static_assert(0);}
		}
	}

	elem ret{ new variable_element() };
	ret.stuff->getvariable()->data.assign_char(char(variable_name));

	return raw_tree(ret.stuff);
}

template raw_tree variable::make<error_tech::no_error_check>(var_name const& variable_name);

template raw_tree variable::make<error_tech::exception_throw>(var_name const& variable_name);

template raw_tree variable::make<error_tech::silent_empty_conversion>(var_name const& variable_name);


template<error_tech err_t = default_error_tech>
raw_tree operation::make(node const& lhs, oper_name const& operator_name, node const& rhs)
{
	
	if constexpr (err_t != error_tech::no_error_check)
	{
		if (!(operator_name == oper_name::log || operator_name == oper_name::exp || operator_name == oper_name::mul || operator_name == oper_name::div || operator_name == oper_name::add || operator_name == oper_name::sub))
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				elem ret{ new message_element(error_name::bad_character_assignment) };
				return raw_tree(ret.stuff);
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::bad_character_assignment);
			}
			else {static_assert(0);}
		}
		if (lhs.loc == nullptr || rhs.loc == nullptr)
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				elem ret{ new message_element(error_name::nullnode_assigned) };
				return raw_tree(ret.stuff);
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::nullnode_assigned);
			}
			else {static_assert(0);}
		}
	}

	elem ret{ new operation_element() };
	ret.stuff->getoperation()->data.assign_opname(operator_name);

	if constexpr (err_t != error_tech::no_error_check)
	{
		if (lhs.loc->is_subbranch()) //if lhs already has a holder using it
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				ret.stuff->getoperation()->data.lhs = new node_branch(new message_element(error_name::subtree_assignment), true);//in this case the holder would be the node branch that wrap_ptr points to
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				delete ret.stuff;
				throw std::exception(error_name::subtree_assignment);
			}
			else {static_assert(0);}
		}
		else
		{
			ret.stuff->getoperation()->data.lhs = lhs.loc;
			lhs.loc->attach_branch();
		}
		if (rhs.loc->is_subbranch()) //if rhs already has a holder using it
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				ret.stuff->getoperation()->data.rhs = new node_branch(new message_element(error_name::subtree_assignment), true);//in this case the holder would be the node branch that wrap_ptr points to
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				delete ret.stuff;
				throw std::exception(error_name::subtree_assignment);
			}
			else {static_assert(0);}
		}
		else
		{
			ret.stuff->getoperation()->data.rhs = rhs.loc;
			rhs.loc->attach_branch();
		}
	}
	else
	{
		ret.stuff->getoperation()->data.lhs = lhs.loc;
		lhs.loc->attach_branch();
		ret.stuff->getoperation()->data.rhs = rhs.loc;
		rhs.loc->attach_branch();
	}
	return raw_tree(ret.stuff);
}

template raw_tree operation::make<error_tech::no_error_check>(node const& lhs, oper_name const& operator_name, node const& rhs);

template raw_tree operation::make<error_tech::exception_throw>(node const& lhs, oper_name const& operator_name, node const& rhs);

template raw_tree operation::make<error_tech::silent_empty_conversion>(node const& lhs, oper_name const& operator_name, node const& rhs);


template<error_tech err_t = default_error_tech>
raw_tree function::make(std::string const& name, std::vector<node> const& args)
{
	if constexpr (err_t != error_tech::no_error_check)
	{
		for (int i = 0; i != name.length(); ++i)
		{
			if (!std::isalpha(name[i], std::locale()))
			{
				if constexpr (err_t == error_tech::silent_empty_conversion)
				{
					elem ret{ new message_element(error_name::bad_function_name_characters) };
					return raw_tree(ret.stuff);
				}
				else if constexpr (err_t == error_tech::exception_throw)
				{
					throw std::exception(error_name::bad_function_name_characters);
				}
				else {static_assert(0);}
			}
		}
	}

	elem ret{ new function_element() };
	ret.stuff->getfunction()->data.function_name = name;

	for (int i = 0; i != args.size(); ++i)
	{
		if constexpr (err_t != error_tech::no_error_check)
		{
			if (args[i].loc->is_subbranch()) //if current argument already has a holder using it
			{
				if constexpr (err_t == error_tech::silent_empty_conversion)
				{
					ret.stuff->getfunction()->data.args.push_back(new node_branch(new message_element(error_name::subtree_assignment), true));//in this case the holder would be the node branch that wrap_ptr points to
				}
				else if constexpr (err_t == error_tech::exception_throw)
				{
					delete ret.stuff;
					throw std::exception(error_name::subtree_assignment);
				}
				else {static_assert(0);}
			}
			else
			{
				ret.stuff->getfunction()->data.args.push_back(args[i].loc);
				args[i].loc->attach_branch();
			}
		}
		else
		{
			ret.stuff->getfunction()->data.args.push_back(args[i].loc);
			args[i].loc->attach_branch();
		}
	}
	return raw_tree(ret.stuff);
}

template raw_tree function::make<error_tech::no_error_check>(std::string const& name, std::vector<node> const& args);

template raw_tree function::make<error_tech::exception_throw>(std::string const& name, std::vector<node> const& args);

template raw_tree function::make<error_tech::silent_empty_conversion>(std::string const& name, std::vector<node> const& args);


template<error_tech err_t = default_error_tech>
raw_tree empty::make(std::string message)
{
	elem ret(new message_element(message));
	return raw_tree( ret.stuff );
}

template raw_tree empty::make<error_tech::no_error_check>(std::string message);

template raw_tree empty::make<error_tech::exception_throw>(std::string message);

template raw_tree empty::make<error_tech::silent_empty_conversion>(std::string message);


template<error_tech err_t = default_error_tech>
raw_tree empty::make()
{
	elem ret(&main_empty_elem);
	return raw_tree(ret.stuff);
}

template raw_tree empty::make<error_tech::no_error_check>();

template raw_tree empty::make<error_tech::exception_throw>();

template raw_tree empty::make<error_tech::silent_empty_conversion>();


//copies contents of "target"
elem::elem(elem const& target)
{
	stuff = target.stuff->duplicate();
}

//copies contents of "rhs"
void elem::operator=(elem const& rhs)
{

	if (stuff != &main_empty_elem)
	{
		delete stuff;
	}
	else
	{
		//do nothing
	}
	stuff = rhs.stuff->duplicate();
}

//steals as much of the tree that isn't used by a node
elem::elem(elem&& target)
{
	stuff = target.stuff->exhaust();
	if (stuff == target.stuff)
	{
		target.stuff = &main_empty_elem;
	}
}

//steals as much of the tree that isn't used by a node
void elem::operator=(elem&& rhs)
{

	if (stuff != &main_empty_elem)
	{
		delete stuff;
	}
	stuff = rhs.stuff->exhaust();
	if (stuff == rhs.stuff)
	{
		rhs.stuff = &main_empty_elem;
	}
}



template<error_tech err_t = default_error_tech>
void elem::operator<<(elem& rhs)
{

	if (stuff != &main_empty_elem)
	{
		delete stuff;
	}
	
	if constexpr (err_t != error_tech::no_error_check)
	{
		if (rhs.stuff->getnodebranchof(stuff))
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				delete rhs.stuff;
				rhs.stuff = &main_empty_elem;
				stuff = new message_element(error_name::recursing_expression);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				stuff = &main_empty_elem;
				throw std::exception(error_name::recursing_expression);
			}
			else {static_assert(0);}
		}

	}

	stuff = rhs.stuff;
	rhs.stuff = &main_empty_elem;
}

template void elem::operator<<<error_tech::no_error_check>(elem& rhs);

template void elem::operator<<<error_tech::exception_throw>(elem& rhs);

template void elem::operator<<<error_tech::silent_empty_conversion>(elem& rhs);


template<error_tech err_t = default_error_tech>
void elem::operator>>(elem& rhs)
{

	if (rhs.stuff != &main_empty_elem)
	{
		delete rhs.stuff;
	}

	if constexpr (err_t != error_tech::no_error_check)
	{
		if (stuff->getnodebranchof(rhs.stuff))
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				delete stuff;
				stuff = &main_empty_elem;
				rhs.stuff = new message_element(error_name::recursing_expression);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				rhs.stuff = &main_empty_elem;
				throw std::exception(error_name::recursing_expression);
			}
			else {static_assert(0);}
		}

	}

	rhs.stuff = stuff;
	stuff = &main_empty_elem;
}

template void elem::operator>><error_tech::no_error_check>(elem& rhs);

template void elem::operator>><error_tech::exception_throw>(elem& rhs);

template void elem::operator>><error_tech::silent_empty_conversion>(elem& rhs);


//compares contents to "rhs"
bool elem::operator==(elem const& rhs) const
{
	return stuff->is_equal(rhs.stuff);
}

//compares contents to "rhs"
bool elem::operator!=(elem const& rhs) const
{
	return !(stuff->is_equal(rhs.stuff));//this needs to be optimized
}


node elem::operator~()
{
	node ret;
	ret.loc = new node_branch(stuff, false);
	stuff = &main_empty_elem;
	return ret;
}

//produce a string from contents, using rules as the format of the print.
template<error_tech err_t = default_error_tech>
std::string elem::str(printing_rule_set rules) const
{
	return stuff->get_str();
}

template std::string elem::str<error_tech::no_error_check>(printing_rule_set rules) const;

template std::string elem::str<error_tech::exception_throw>(printing_rule_set rules) const;

template std::string elem::str<error_tech::silent_empty_conversion>(printing_rule_set rules) const;


//returns true if this element contains "a", whether direcly or indirectly.
bool elem::holds(node a) const
{
	return stuff->holds(a.loc);
}


elem::elem(virt_element* a)
{
	stuff = a;
}


elem::iterator::iterator()
{
	curind = 0;
}

elem::iterator::iterator(const node& a)
{
	current_row.push_back(a);
	curind = 0;
}

elem::iterator::iterator(const iterator& a)
{
	current_row.reserve(a.current_row.size() - a.curind);
	for (int i = a.curind; i != a.current_row.size(); ++i)
	{
		current_row.push_back(a.current_row[i]);
	}
	next_row = a.next_row;
	curind = 0;
}


void elem::iterator::operator=(const node& a)
{
	current_row.clear();
	current_row.push_back(a);
	next_row.clear();
	curind = 0;
}

void elem::iterator::operator=(const iterator& a)
{
	current_row.clear();
	current_row.reserve(a.current_row.size() - a.curind);
	for (int i = a.curind; i != a.current_row.size(); ++i)
	{
		current_row.push_back(a.current_row[i]);
	}
	next_row = a.next_row;
	curind = 0;
}


elem::iterator::iterator(node&& a)
{
	current_row.push_back(std::move(a));
	curind = 0;
}

elem::iterator::iterator(iterator&& a)
{
	current_row = std::move(a.current_row);
	next_row = std::move(a.next_row);
	curind = a.curind;
}


void elem::iterator::operator=(node&& a)
{
	current_row.clear();
	next_row.clear();
	current_row.push_back(std::move(a));
	curind = 0;
}

void elem::iterator::operator=(iterator&& a)
{
	current_row = std::move(a.current_row);
	next_row = std::move(a.next_row);
	curind = a.curind;
}


elem::iterator::operator node() const
{
	return current_row[curind];
}


elem& elem::iterator::operator*() const
{
	return *current_row[curind];
}

elem* elem::iterator::operator->() const
{
	return &*current_row[curind];
}


void elem::iterator::next()
{
	if (operation(current_row[curind]))
	{
		next_row.reserve(next_row.size() + 2);
		next_row.push_back(operation(current_row[curind]).get_lhs());
		next_row.push_back(operation(current_row[curind]).get_rhs());
	}
	else if (function(current_row[curind]))
	{
		next_row.reserve(next_row.size() + function(current_row[curind]).get_length());
		for (int i = 0; i != function(current_row[curind]).get_length(); ++i)
		{
			next_row.push_back(function(current_row[curind]).get_arg(i));
		}
	}
	else
	{

	}

	if (curind + 1 == current_row.size())
	{
		curind = 0;
		current_row.swap(next_row);
		next_row.clear();
	}
	else
	{
		++curind;
	}
}

elem::iterator elem::iterator::blank()
{
	iterator ret;
	ret.curind = 0;
	return ret;
}

bool elem::iterator::isblank() const
{
	return current_row.size() == 0 && next_row.size() == 0;
}


bool elem::iterator::operator==(iterator a) const
{
	size_t ti = curind;
	size_t ri = a.curind;
	if (current_row.size() - ti != a.current_row.size() - ri || next_row!=a.next_row)
	{
		return false;
	}
	while (ti != current_row.size() && ri!=a.current_row.size())
	{
		if (current_row[ti] != a.current_row[ri])
		{
			return false;
		}
	}
	return true;
}

elem::iterator elem::front() const
{
	if (stuff->getoperation())
	{
		iterator ret;
		ret.current_row.reserve(2 + ret.current_row.size());
		ret.current_row = { node(stuff->getoperation()->data.lhs),node(stuff->getoperation()->data.rhs) };
		return ret;
	}
	else if (stuff->getfunction())
	{
		iterator ret;
		ret.current_row.reserve(stuff->getfunction()->data.args.size()+ret.current_row.size());
		for (int i = 0; i != stuff->getfunction()->data.args.size(); ++i)
		{
			ret.current_row.push_back(node(stuff->getfunction()->data.args[i]));
		}
		return ret;
	}
	else
	{
		return elem::iterator::blank();
	}
}



node::node()
{
	loc = nullptr;
}

//refer to the same element as "target"
node::node(node const& target)
{
	loc = target.loc;
	if (loc != nullptr)
	{
		loc->add_user();
	}
}

//refer to the same element as "target"
node::node(node&& target)
{
	loc = target.loc;
	target.loc = nullptr;
}


node::node(raw_tree&& a)
{
	loc = new node_branch(a.res, false);
	a.res = &main_empty_elem;
}


//refer to the same element as "target"
void node::operator=(node const& target)
{
	if (loc != nullptr)
	{
		loc->drop_user();
	}
	loc = target.loc;
	if (loc != nullptr)
	{
		loc->add_user();
	}
}

//refer to the same element as "target"
void node::operator=(node&& target)
{
	if (loc != nullptr)
	{
		loc->drop_user();
	}
	loc = target.loc;
	target.loc = nullptr;
}


//does this refer to the same elem as "rhs"
bool node::operator==(node const& rhs) const
{
	return loc == rhs.loc;
}

//does this not refer to the same elem as "rhs"
bool node::operator!=(node const& rhs) const
{
	return loc != rhs.loc;
}


//for std::map usage
bool node::operator<(node const& rhs) const
{
	return loc < rhs.loc;
}


//returns the used elem, which may be modified.
elem& node::operator*() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	return *reinterpret_cast<elem *>(&loc->owned);
}

elem* node::operator->() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	return reinterpret_cast<elem *>(&loc->owned);
}


node node::make_nullnode()
{
	return { (node_branch*)nullptr };
}


node::~node()
{
	if (loc != nullptr)
	{
		loc->drop_user();
	}
}


node::node(node_branch* a)
{
	loc = a;
	if (loc != nullptr)
	{
		loc->add_user();
	}
}



const node nullnode{ node::make_nullnode() };


constant::constant(node const& target) :
	node{ target }
{ }

constant::constant(node&& target) :
	node{ std::move(target) }
{ }



variable::variable(node const& target) :
	node{ target }
{ }

variable::variable(node&& target) :
	node{ std::move(target) }
{ }



operation::operation(node const& target) :
	node{ target }
{ }

operation::operation(node&& target) :
	node{ std::move(target) }
{ }



function::function(node const& target) :
	node{ target }
{ }

function::function(node&& target) :
	node{ std::move(target) }
{ }



empty::empty(node const& target) :
	node{ target }
{ }

empty::empty(node&& target) :
	node{ std::move(target) }
{ }


constant::constant()
{
	loc = nullptr;
}

variable::variable()
{
	loc = nullptr;
}

operation::operation()
{
	loc = nullptr;
}

function::function()
{
	loc = nullptr;
}

empty::empty()
{
	loc = nullptr;
}

constant::operator bool() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	return static_cast<bool>( loc->read_data()->getnumber() );
}

variable::operator bool() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	return static_cast<bool>(loc->read_data()->getvariable());
}

operation::operator bool() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	return static_cast<bool>(loc->read_data()->getoperation());
}

function::operator bool() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	return static_cast<bool>(loc->read_data()->getfunction());
}

empty::operator bool() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	return static_cast<bool>(loc->read_data()->geterrornode());
}


template<bool to_throw = bool(default_error_tech)>
num_type constant::get_value() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	return loc->read_data()->getnumber()->data.get_value();
}

template num_type constant::get_value<true>() const;

template num_type constant::get_value<false>() const;

template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void constant::set_value(num_type a)
{

	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	loc->read_data()->getnumber()->data.assign_value(a);
}

template void constant::set_value<error_tech::no_error_check, false>(num_type a);

template void constant::set_value<error_tech::exception_throw, false>(num_type a);

template void constant::set_value<error_tech::silent_empty_conversion, false>(num_type a);

template void constant::set_value<error_tech::no_error_check, true>(num_type a);

template void constant::set_value<error_tech::exception_throw, true>(num_type a);

template void constant::set_value<error_tech::silent_empty_conversion, true>(num_type a);


template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void constant::set_value(int a)
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	if constexpr (err_t != error_tech::no_error_check)
	{
		if (a < 0)
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				delete loc->read_data();
				loc->set_data(new message_element(error_name::bad_number_assignment));
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::bad_number_assignment);
			}
			else {static_assert(0);}
		}
	}

	loc->read_data()->getnumber()->data.assign_value(a);
	
}

template void constant::set_value<error_tech::no_error_check, false>(int a);

template void constant::set_value<error_tech::exception_throw, false>(int a);

template void constant::set_value<error_tech::silent_empty_conversion, false>(int a);

template void constant::set_value<error_tech::no_error_check, true>(int a);

template void constant::set_value<error_tech::exception_throw, true>(int a);

template void constant::set_value<error_tech::silent_empty_conversion, true>(int a);


template<bool to_throw = bool(default_error_tech)>
var_name variable::get_name() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	return var_name(loc->read_data()->getvariable()->data.get_char());
}

template var_name variable::get_name<true>() const;

template var_name variable::get_name<false>() const;

template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void variable::set_name(var_name a)
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	if constexpr (err_t != error_tech::no_error_check)
	{
		std::locale l;
		if (!std::islower(a, l))
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				delete loc->read_data();
				loc->set_data(new message_element(error_name::bad_character_assignment));
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)	
			{
				throw std::exception(error_name::bad_character_assignment);
			}
			else {static_assert(0);}
		}
	}

	loc->read_data()->getvariable()->data.assign_char(char(a));
	
}

template void variable::set_name<error_tech::no_error_check, false>(var_name a);

template void variable::set_name<error_tech::exception_throw, false>(var_name a);

template void variable::set_name<error_tech::silent_empty_conversion, false>(var_name a);

template void variable::set_name<error_tech::no_error_check, true>(var_name a);

template void variable::set_name<error_tech::exception_throw, true>(var_name a);

template void variable::set_name<error_tech::silent_empty_conversion, true>(var_name a);


template<bool to_throw = bool(default_error_tech)>
oper_name operation::get_opname() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	return loc->read_data()->getoperation()->data.get_opname();
}

template oper_name operation::get_opname<true>() const;

template oper_name operation::get_opname<false>() const;

template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void operation::set_opname(oper_name a)
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}

	if constexpr (err_t != error_tech::no_error_check)
	{

		if (!(a == oper_name::add || a == oper_name::sub || a == oper_name::mul || a == oper_name::div || a == oper_name::exp || a == oper_name::log))
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				delete loc->read_data();
				loc->set_data(new message_element(error_name::invalid_oper_name_assignment));
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::invalid_oper_name_assignment);
			}
			else {static_assert(0);}
		}
	}

	loc->read_data()->getoperation()->data.assign_opname(a);
}

template void operation::set_opname<error_tech::no_error_check, false>(oper_name a);

template void operation::set_opname<error_tech::exception_throw, false>(oper_name a);

template void operation::set_opname<error_tech::silent_empty_conversion, false>(oper_name a);

template void operation::set_opname<error_tech::no_error_check, true>(oper_name a);

template void operation::set_opname<error_tech::exception_throw, true>(oper_name a);

template void operation::set_opname<error_tech::silent_empty_conversion, true>(oper_name a);


template<bool to_throw = bool(default_error_tech)>
node operation::get_lhs() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	return loc->read_data()->getoperation()->data.lhs;
}

template node operation::get_lhs<true>() const;

template node operation::get_lhs<false>() const;

template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void operation::set_lhs(node a)
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	
	if constexpr (err_t != error_tech::no_error_check)
	{
		if (a.loc == nullptr)
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				loc->read_data()->getoperation()->data.lhs->break_branch();
				loc->read_data()->getoperation()->data.lhs = new node_branch(new message_element(error_name::nullnode_assigned), true);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::nullnode_assigned);
			}
			else {static_assert(0);}
		}
		if (a.loc->is_subbranch())
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				loc->read_data()->getoperation()->data.lhs->break_branch();
				loc->read_data()->getoperation()->data.lhs = new node_branch(new message_element(error_name::subtree_assignment), true);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::subtree_assignment);
			}
			else {static_assert(0);}
		}
		if (a.loc->read_data()->holds(loc) || a.loc->read_data() == loc->read_data())
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				loc->read_data()->getoperation()->data.lhs->break_branch();
				loc->read_data()->getoperation()->data.lhs = new node_branch(new message_element(error_name::recursing_expression), true);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::recursing_expression);
			}
			else { static_assert(0); }
		}
	}
	loc->read_data()->getoperation()->data.lhs->break_branch();

	loc->read_data()->getoperation()->data.lhs = a.loc;
	a.loc->attach_branch();
	
	
}

template void operation::set_lhs<error_tech::no_error_check, false>(node a);

template void operation::set_lhs<error_tech::exception_throw, false>(node a);

template void operation::set_lhs<error_tech::silent_empty_conversion, false>(node a);

template void operation::set_lhs<error_tech::no_error_check, true>(node a);

template void operation::set_lhs<error_tech::exception_throw, true>(node a);

template void operation::set_lhs<error_tech::silent_empty_conversion, true>(node a);


template<bool to_throw = bool(default_error_tech)>
node operation::get_rhs() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	return loc->read_data()->getoperation()->data.rhs;
}

template node operation::get_rhs<true>() const;

template node operation::get_rhs<false>() const;

template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void operation::set_rhs(node a)
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}

	if constexpr (err_t != error_tech::no_error_check)
	{
		if (a.loc == nullptr)
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				loc->read_data()->getoperation()->data.rhs->break_branch();
				loc->read_data()->getoperation()->data.rhs = new node_branch(new message_element(error_name::nullnode_assigned), true);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::nullnode_assigned);
			}
			else {static_assert(0);}
		}
		if (a.loc->is_subbranch())
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				loc->read_data()->getoperation()->data.rhs->break_branch();
				loc->read_data()->getoperation()->data.rhs = new node_branch(new message_element(error_name::subtree_assignment), true);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::subtree_assignment);
			}
			else {static_assert(0);}
		}
		if (a.loc->read_data()->holds(loc) || a.loc->read_data() == loc->read_data())
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				loc->read_data()->getoperation()->data.rhs->break_branch();
				loc->read_data()->getoperation()->data.rhs = new node_branch(new message_element(error_name::recursing_expression), true);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::recursing_expression);
			}
			else { static_assert(0); }
		}
	}
	loc->read_data()->getoperation()->data.rhs->break_branch();

	loc->read_data()->getoperation()->data.rhs = a.loc;
	a.loc->attach_branch();
}

template void operation::set_rhs<error_tech::no_error_check, false>(node a);

template void operation::set_rhs<error_tech::exception_throw, false>(node a);

template void operation::set_rhs<error_tech::silent_empty_conversion, false>(node a);

template void operation::set_rhs<error_tech::no_error_check, true>(node a);

template void operation::set_rhs<error_tech::exception_throw, true>(node a);

template void operation::set_rhs<error_tech::silent_empty_conversion, true>(node a);


template<bool to_throw = bool(default_error_tech)>
std::string function::get_name() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	return loc->read_data()->getfunction()->data.function_name;
}

template std::string function::get_name<true>() const;

template std::string function::get_name<false>() const;

template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void function::set_name(std::string a)
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	
	if constexpr (err_t != error_tech::no_error_check)
	{
		if (a.length() == 0)
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				delete loc->read_data();
				loc->set_data(new message_element(error_name::bad_function_name_characters));
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::bad_function_name_characters);
			}
			else {static_assert(0);}
		}
		for (int i = 0; i != a.length(); ++i)
		{
			if (!std::islower(a[i], std::locale()))
			{
				if constexpr (err_t == error_tech::silent_empty_conversion)
				{
					delete loc->read_data();
					loc->set_data(new message_element(error_name::bad_function_name_characters));
					return;
				}
				else if constexpr (err_t == error_tech::exception_throw)
				{
					throw std::exception(error_name::bad_function_name_characters);
				}
				else {static_assert(0);}
			}
		}
	}

	loc->read_data()->getfunction()->data.function_name = a;
}

template void function::set_name<error_tech::no_error_check, false>(std::string a);

template void function::set_name<error_tech::exception_throw, false>(std::string a);

template void function::set_name<error_tech::silent_empty_conversion, false>(std::string a);

template void function::set_name<error_tech::no_error_check, true>(std::string a);

template void function::set_name<error_tech::exception_throw, true>(std::string a);

template void function::set_name<error_tech::silent_empty_conversion, true>(std::string a);


template<bool to_throw = bool(default_error_tech)>
std::vector<node> function::get_args() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	std::vector<node> ret;
	ret.reserve(loc->read_data()->getfunction()->data.args.size());
	for (int i = 0; i != loc->read_data()->getfunction()->data.args.size(); ++i)
	{
		ret.push_back(loc->read_data()->getfunction()->data.args[i]);
	}
	return ret;
}

template std::vector<node> function::get_args<true>() const;

template std::vector<node> function::get_args<true>() const;

template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void function::set_args(std::vector<node> a)
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}

	if constexpr (err_t != error_tech::no_error_check)
	{
		for (int i = 0; i != a.size(); ++i)
		{
			if (a[i].loc == nullptr)
			{
				if constexpr (err_t == error_tech::silent_empty_conversion)
				{
					a[i].loc = new node_branch(new message_element(error_name::nullnode_assigned),false); //when "a" goes out of scope it will decrease its user count by 1. this is fine because in the assignment this node_branch will become held.
				}
				else if constexpr (err_t == error_tech::exception_throw)
				{
					throw std::exception(error_name::nullnode_assigned);
				}
				else {static_assert(0);}
			}
			else if (a[i].loc->is_subbranch())
			{
				if constexpr (err_t == error_tech::silent_empty_conversion)
				{
					a[i].loc->drop_user();
					a[i].loc = new node_branch(new message_element(error_name::subtree_assignment), false);
				}
				else if constexpr (err_t == error_tech::exception_throw)
				{
					throw std::exception(error_name::subtree_assignment);
				}
				else {static_assert(0);}
			}
			if (a[i].loc->read_data()->holds(loc) || a[i].loc->read_data() == loc->read_data())
			{
				if constexpr (err_t == error_tech::silent_empty_conversion)
				{
					a[i].loc->drop_user();
					a[i].loc = new node_branch(new message_element(error_name::recursing_expression), false);
				}
				else if constexpr (err_t == error_tech::exception_throw)
				{
					throw std::exception(error_name::recursing_expression);
				}
				else { static_assert(0); }
			}
		}
	}

	for (int i = 0; i != loc->read_data()->getfunction()->data.args.size(); ++i)
	{
		loc->read_data()->getfunction()->data.args[i]->break_branch();
	}

	loc->read_data()->getfunction()->data.args.clear();
	loc->read_data()->getfunction()->data.args.reserve(a.size());

	for (int i = 0; i != a.size(); ++i)
	{
		loc->read_data()->getfunction()->data.args.push_back(a[i].loc);
		a[i].loc->attach_branch();
	}
}

template void function::set_args<error_tech::no_error_check, false>(std::vector<node> a);

template void function::set_args<error_tech::exception_throw, false>(std::vector<node> a);

template void function::set_args<error_tech::silent_empty_conversion, false>(std::vector<node> a);

template void function::set_args<error_tech::no_error_check, true>(std::vector<node> a);

template void function::set_args<error_tech::exception_throw, true>(std::vector<node> a);

template void function::set_args<error_tech::silent_empty_conversion, true>(std::vector<node> a);


template<bool to_throw = bool(default_error_tech)>
node function::get_arg(size_t index) const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	return loc->read_data()->getfunction()->data.args[index];
}

template node function::get_arg<true>(size_t index) const;

template node function::get_arg<false>(size_t index) const;

template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void function::set_arg(size_t index, node a)
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}

	if constexpr (err_t != error_tech::no_error_check)
	{
		if (a.loc == nullptr)
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				loc->read_data()->getfunction()->data.args[index]->break_branch();
				loc->read_data()->getfunction()->data.args[index] = new node_branch(new message_element(error_name::nullnode_assigned), true);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::nullnode_assigned);
			}
			else {static_assert(0);}
		}
		if (a.loc->is_subbranch())
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				loc->read_data()->getfunction()->data.args[index]->break_branch();
				loc->read_data()->getfunction()->data.args[index] = new node_branch(new message_element(error_name::subtree_assignment), true);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::subtree_assignment);
			}
			else {static_assert(0);}
		}
		if (a.loc->read_data()->holds(loc) || a.loc->read_data() == loc->read_data())
		{
			if constexpr (err_t == error_tech::silent_empty_conversion)
			{
				loc->read_data()->getfunction()->data.args[index]->break_branch();
				loc->read_data()->getfunction()->data.args[index] = new node_branch(new message_element(error_name::recursing_expression), true);
				return;
			}
			else if constexpr (err_t == error_tech::exception_throw)
			{
				throw std::exception(error_name::recursing_expression);
			}
			else { static_assert(0); }
		}
	}
	loc->read_data()->getfunction()->data.args[index]->break_branch();

	loc->read_data()->getfunction()->data.args[index] = a.loc;
	a.loc->attach_branch();
}

template void function::set_arg<error_tech::no_error_check, false>(size_t index, node a);

template void function::set_arg<error_tech::exception_throw, false>(size_t index, node a);

template void function::set_arg<error_tech::silent_empty_conversion, false>(size_t index, node a);

template void function::set_arg<error_tech::no_error_check, true>(size_t index, node a);

template void function::set_arg<error_tech::exception_throw, true>(size_t index, node a);

template void function::set_arg<error_tech::silent_empty_conversion, true>(size_t index, node a);


template<bool to_throw = bool(default_error_tech)>
size_t function::get_length() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	return loc->read_data()->getfunction()->data.args.size();
}

template size_t function::get_length<true>() const;

template size_t function::get_length<false>() const;

template<bool to_throw = bool(default_error_tech)>
bool empty::has_message() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!operator bool())
		{
			throw bad_elem_member_access();
		}
	}
	if (loc->read_data()->geterrornode())
	{
		return true;
	}
	else
	{
		return false;
	}
}

template bool empty::has_message<true>() const;

template bool empty::has_message<false>() const;

template<bool to_throw = bool(default_error_tech)>
std::string empty::get_message() const
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!loc->read_data()->geterrornode())
		{
			throw bad_elem_member_access();
		}
	}
	return loc->read_data()->geterrornode()->get_message();
}

template std::string empty::get_message<true>() const;

template std::string empty::get_message<false>() const;

template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
void empty::set_message(std::string a)
{
	if (loc == nullptr)
	{
		throw bad_nullnode_usage();
	}
	if constexpr(to_throw)
	{
		if (!loc->read_data()->geterrornode())
		{
			throw bad_elem_member_access();
		}
	}
	loc->read_data()->geterrornode()->set_message(a);
}

template void empty::set_message<error_tech::no_error_check, false>(std::string a);

template void empty::set_message<error_tech::exception_throw, false>(std::string a);

template void empty::set_message<error_tech::silent_empty_conversion, false>(std::string a);

template void empty::set_message<error_tech::no_error_check, true>(std::string a);

template void empty::set_message<error_tech::exception_throw, true>(std::string a);

template void empty::set_message<error_tech::silent_empty_conversion, true>(std::string a);