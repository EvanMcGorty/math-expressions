#pragma once
#include"virt_element.h"



class node_branch
{
public:
	friend class node;


	//node_branch(const std::string& a, int index, bool created_by_holder);

	node_branch(virt_element* a, bool created_by_holder)
	{
		owned = a;
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


	~node_branch();


	std::string get_str()
	{
		return owned -> get_str();
	} 

	virt_element* read_data()
	{
		return owned;
	}

	void set_data(virt_element* a)
	{
		owned = a;
	}


	int user_count; //if this node has a "value" or "term" pointing to itself (note: value and term refer to specific classes)

					// (for wrapping classes to use) indicate that a wrapping class is now using this node
	void add_user()
	{
		user_count += 1;
	}

	// (for wrapping classes to use) indicate that a wrapping class is no longer using this node
	void drop_user()
	{
		user_count -= 1;
		if ((!is_subbranch()) && (user_count == 0))
		{
			delete this;
		}

	}


	// (for holder nodes to use) indicate that ownership of this node is being dropped by its holder
	void break_branch()
	{
		is_root = true;
		if ((!is_subbranch()) && (user_count == 0))
		{
			delete this;
		}
	}

	// (for holder nodes to use) indicate that ownership of this node is being taken by a holder
	void attach_branch()
	{
		is_root = false;
	}



	//is the node's lifetime being prolonged by another node
	bool is_subbranch()
	{
		return !is_root;
	}

private:

	virt_element* owned;

	bool is_root; //if this node is not part of an earlier element


};