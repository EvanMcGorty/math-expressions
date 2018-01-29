#pragma once
#include"expressions.h"



std::vector<node> get_held_nodes(node a)
{
	operation o(a);
	if (o)
	{
		return { o.get_lhs(), o.get_rhs() };
	}
	else
	{
		function f(a);
		if (f)
		{
			return f.get_args();
		}
	}

	return {};
}

//enumerates the elements/nodes of an expression
class tree_elem_count
{
public:

	tree_elem_count(node a)
	{
		head = a;
		std::vector<node> held_nodes = get_held_nodes(head);

		if (held_nodes.size())
		{
			body.push_back(std::move(held_nodes));
		}

		for (int i = 0; i != body.size(); ++i)
		{
			held_nodes.clear();

			for (int i2 = 0; i2 != body[i].size(); ++i2)
			{
				std::vector<node> found = get_held_nodes(body[i][i2]);
				
				held_nodes.insert(held_nodes.end(), found.begin(), found.end());
			}

			if (held_nodes.size())
			{
				body.push_back(std::move(held_nodes));
			}

		}
	}


	node get(size_t a) const
	{
		if (a == 0)
		{
			return head;
		}

		--a;

		for (int i = 0; i != body.size(); ++i)
		{
			if (a >= body[i].size())
			{
				a -= body[i].size();
			}
			else
			{
				return body[i][a];
			}
		}
	}

	std::vector<node> get_layer(size_t a) const
	{
		if (a == 0)
		{
			return { head };
		}
		else
		{
			return body[a - 1];
		}
	}


	//the total amount of nodes in this tree
	size_t count() const
	{
		size_t ret = 1;
		for (int i = 0; i != body.size(); ++i)
		{
			ret += body[i].size();
		}
		return ret;
	}

	size_t layer_count() const
	{
		return body.size() + 1;
	}

private:
	node head;
	std::vector<std::vector<node>> body;
};


