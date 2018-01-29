#pragma once

#include<stdexcept>
#include"definitions.h"

class node_branch;
class virt_element;

class node;


//in terms of tree location (node) comparison, null nodes are considered to be part of the one, same tree, and therefore this exception will not ever be thrown by a node comparison.
class bad_nullnode_usage : public std::exception
{

};

//using a .getsomething() or .setsomthing() function from a constant, variable, operation, or function, when said class does not point to the corrosponding type.
class bad_elem_member_access : public std::exception
{

};


//the kind of error technique that will be used when an input is invalid.
//unrelated to handeling invalid type access or bad nullnode usage, which must be done by the programmer.
enum class error_tech
{
	exception_throw = 1, //throws a std::exception with an error_name
	silent_empty_conversion = 2, //object which is being assigned
	no_error_check = 0, //doesn't check 
};

//in release mode, no error checking will be done, but in debug mode, an exception will be thrown
#ifdef _DEBUG
#define default_error_tech error_tech::exception_throw
#else
#define default_error_tech error_tech::no_error_check
#endif

class raw_tree
{

public:
	friend class elem;
	
	friend class node;

	friend class function;

	friend class operation;

	friend class constant;

	friend class variable;

	friend class empty;


	raw_tree(elem a);

	void operator=(raw_tree a) = delete;

	raw_tree(const raw_tree& a) = delete;

	void operator=(raw_tree&& a);

	raw_tree(raw_tree&& a);

	~raw_tree();

private:
	raw_tree(virt_element*& r);

	virt_element* res;
};


//an element of an expression
class elem
{
	friend class node;

	friend class function;

	friend class operation;

	friend class constant;

	friend class variable;

	friend class empty;

	friend class raw_tree;
public:


	//initialization with an empty;
	elem();

	~elem();

	//initialization by parsing a string.
	elem(std::string const& expression, parsing_rule_set const& rules = parsing_rule_set::typical_rules());

	//initialization by parsing a string.
	elem(std::string::const_iterator expression_begin, std::string::const_iterator expression_end, parsing_rule_set const& rules = parsing_rule_set::typical_rules());

	
	elem(raw_tree&& a);


	//copies contents of "target"
	elem(elem const& target);

	//copies contents of "rhs"
	void operator=(elem const& rhs);

	//steals all resources that aren't used by the held parts of target.
	//behaves exactly how copy construction does, is just more efficient.
	elem(elem&& target);

	//steals all resources that aren't used by the held parts of rhs.
	//behaves exactly how copy assignment does, is just more efficient.
	void operator=(elem&& rhs);


	//steals resources of "rhs"  all nodes held by rhs are kept and reused.
	//there is an error when this causes a looping expression.
	template<error_tech err_t = default_error_tech>
	void operator<<(elem& rhs);

	//gives resources to "rhs". all held nodes are kept by rhs.
	//there is an error when this causes a looping expression.
	template<error_tech err_t = default_error_tech>
	void operator>>(elem& rhs);

	
	//compares contents to "rhs"
	bool operator==(elem const& rhs) const;

	//compares contents to "rhs"
	bool operator!=(elem const& rhs) const;


	//produce a string from contents, using rules as the format of the print.
	template<error_tech err_t = default_error_tech>
	std::string str(printing_rule_set rules = printing_rule_set::typical_rules()) const;

	//returns true if this element contains "a" direcly, or indirectly by one of its nodes' elements
	bool holds(node a) const;

	//release resources to be pointed at by nodes
	node operator~();


	class iterator
	{
		friend class elem;
	public:
		
		iterator();

		explicit iterator(const node& a);

		iterator(const iterator& a);


		void operator=(const node& a);

		void operator=(const iterator& a);


		explicit iterator(node&& a);

		iterator(iterator&& a);


		void operator=(node&& a);

		void operator=(iterator&& a);


		explicit operator node() const;


		elem& operator*() const;

		elem* operator->() const;


		void next();

		
		static iterator blank();

		bool isblank() const;


		bool operator==(node a) = delete;

		bool operator==(iterator a) const;


	private:
		size_t curind;
		std::vector<node> current_row;
		std::vector<node> next_row;
	};


	elem::iterator front() const;

private:


	virt_element* stuff;

	elem(virt_element* a);

};


//a referance/pointer to an element of an expression
class node
{
	friend class elem;

	friend class function;

	friend class operation;

	friend class constant;

	friend class variable;

	friend class empty;
public:

	//initialization as a nullnode
	node();

	//refer to the same element as "target"
	node(node const& target);

	//refer to the same element as "target"
	node(node&& target);


	node(raw_tree && a);


	//refer to the same element as "target"
	void operator=(node const& target);

	//refer to the same element as "target"
	void operator=(node&& target);


	//does this refer to the same elem as "rhs"
	bool operator==(node const& rhs) const;

	//does this not refer to the same elem as "rhs"
	bool operator!=(node const& rhs) const;


	//for std::map usage
	bool operator<(node const& rhs) const;

	
	//returns the used elem, which may be modified.
	elem& operator*() const;

	elem* operator->() const;


	static node make_nullnode();


	~node();


protected:

	node_branch* loc;


	node(node_branch* a);


};


//a node that allows access of values from a constant (number) element
class constant : public node
{
public:

	//initialization as a nullnode
	constant();

	//refer to the same element as "target"
	constant(node const& target);

	//refer to the same element as "target"
	constant(node&& target);


	//initialization of an elem.
	template<error_tech err_t = default_error_tech>
	static raw_tree make(num_type const& numerical_value);


	//initialization of an elem from an int type. when int isn't positive an error will occur
	template<error_tech err_t = default_error_tech>
	static raw_tree make(int const& integer_val);


	operator bool() const;

	template<bool to_throw = bool(default_error_tech)>
	num_type get_value() const;

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_value(num_type a);

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_value(int a);
};

//a node that allows access of values from a variable element
class variable : public node
{
public:

	//initialization as a nullnode
	variable();

	//refer to the same element as "target"
	variable(node const& target);

	//refer to the same element as "target"
	variable(node&& target);


	//initialization of a variable from a char type. char must be a valid alphabetical character or initialization will be with a bad_character_assignment error
	template<error_tech err_t = default_error_tech>
	static raw_tree make(var_name const& variable_name);


	operator bool() const;

	template<bool to_throw = bool(default_error_tech)>
	var_name get_name() const;

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_name(var_name a);
};

//a node that allows access of values from an operation element
class operation : public node
{
public:

	//initialization as a nullnode
	operation();

	//refer to the same element as "target"
	operation(node const& target);

	//refer to the same element as "target"
	operation(node&& target);


	//initialization of an operation from two nodes, and an operation name.
	template<error_tech err_t = default_error_tech>
	static raw_tree make(node const& left_hand_side, oper_name const& operator_name, node const& right_hand_side);


	operator bool() const;

	template<bool to_throw = bool(default_error_tech)>
	oper_name get_opname() const;

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_opname(oper_name a);

	template<bool to_throw = bool(default_error_tech)>
	node get_lhs() const;

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_lhs(node a);

	template<bool to_throw = bool(default_error_tech)>
	node get_rhs() const;

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_rhs(node a);
};

//a node that allows access of values from a function element
class function : public node
{
public:

	//initialization as a nullnode
	function();

	//refer to the same element as "target"
	function(node const& target);

	//refer to the same element as "target"
	function(node&& target);


	//initialization of a function from a string and a vector of nodes.
	template<error_tech err_t = default_error_tech>
	static raw_tree make(std::string const& functname, std::vector<node> const& function_arguments);


	operator bool() const;

	template<bool to_throw = bool(default_error_tech)>
	std::string get_name() const;

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_name(std::string a);

	template<bool to_throw = bool(default_error_tech)>
	std::vector<node> get_args() const;

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_args(std::vector<node> a);

	template<bool to_throw = bool(default_error_tech)>
	node get_arg(size_t index) const;

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_arg(size_t index, node a);

	template<bool to_throw = bool(default_error_tech)>
	size_t get_length() const;
};

//a node that allows access to possible messages from an empty element
class empty : public node
{
public:

	//initialization as a nullnode
	empty();

	//refer to the same element as "target"
	empty(node const& target);

	//refer to the same element as "target"
	empty(node&& target);


	//creates an empty elem
	template<error_tech err_t = default_error_tech>
	static raw_tree make();

	//creates an empty elem with a message
	template<error_tech err_t = default_error_tech>
	static raw_tree make(std::string message);


	operator bool() const;

	template<bool to_throw = bool(default_error_tech)>
	bool has_message() const;

	template<bool to_throw = bool(default_error_tech)>
	std::string get_message() const;

	template<error_tech err_t = default_error_tech, bool to_throw = bool(default_error_tech)>
	void set_message(std::string a);
};




extern const node nullnode;



