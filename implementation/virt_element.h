#pragma once
#include<string>

class node_branch;

class message_element;
class empty_element;

class holder;
class operation_element;
class function_element;

class lone;
class constant_element;
class variable_element;

class virt_element
{
public:


	virtual holder* getholder()
	{
		return nullptr;
	}


	virtual operation_element* getoperation()
	{
		return nullptr;
	}

	virtual lone* getlone()
	{
		return nullptr;
	}

	virtual constant_element* getnumber()
	{
		return nullptr;
	}

	virtual variable_element* getvariable()
	{
		return nullptr;
	}

	virtual message_element* geterrornode()
	{
		return nullptr;
	}

	virtual function_element* getfunction()
	{
		return nullptr;
	}

	virtual empty_element* getempty()
	{
		return nullptr;
	}

	virtual bool holds(node_branch* a) = 0;

	virtual node_branch** getnodebranchof(virt_element* a)
	{
		return nullptr;
	}



	virtual std::string get_str() = 0;

	virtual virt_element* duplicate() = 0;

	virtual virt_element* exhaust() = 0;

	virtual bool is_equal(virt_element* a) = 0;

	virtual ~virt_element()
	{

	}



};