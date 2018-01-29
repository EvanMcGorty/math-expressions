#pragma once
#include"expressions.h"

std::vector<node> make_additive_list(node a)
{
	operation o = a;
	if (o)
	{
		if (o.get_opname() == oper_name::add)
		{
			std::vector<node> p1 = make_additive_list(o.get_lhs());

			{
				std::vector<node> p2 = make_additive_list(o.get_rhs());

				p1.reserve(p1.size() + p2.size());

				for (int i = 0; i != p2.size(); ++i)
				{
					p1.push_back(p2[i]);
				}
			}

			return p1;
		}
		else if (o.get_opname() == oper_name::sub)
		{
			std::vector<node> p1 = make_additive_list(o.get_lhs());

			{
				std::vector<node> p2 = make_additive_list(o.get_rhs());

				p1.reserve(p1.size() + p2.size());

				for (int i = 0; i != p2.size(); ++i)
				{
					if (operation(p2[i]) && *operation(p2[i]).get_lhs() == operation::make(constant::make(0), oper_name::sub, constant::make(1)))
					{
						(*p2[i])<<(*operation(p2[i]).get_rhs());
					}
					else
					{
						*p2[i] = operation::make(constant::make(0), oper_name::sub, constant::make(1));
					}
					p1.push_back(p2[i]);
				}
			}

			return p1;
		}
		else
		{
			return { ~*a };
		}

	}
	else
	{
		return { ~*a };
	}
}



std::vector<node> make_multiplicative_list(node a)
{
	operation o = a;
	if (o)
	{
		if (o.get_opname() == oper_name::mul)
		{
			std::vector<node> p1 = make_multiplicative_list(o.get_lhs());

			{
				std::vector<node> p2 = make_multiplicative_list(o.get_rhs());

				p1.reserve(p1.size() + p2.size());

				for (int i = 0; i != p2.size(); ++i)
				{
					p1.push_back(p2[i]);
				}
			}

			return p1;
		}
		else if (o.get_opname() == oper_name::div)
		{
			std::vector<node> p1 = make_multiplicative_list(o.get_lhs());

			{
				std::vector<node> p2 = make_multiplicative_list(o.get_rhs());

				p1.reserve(p1.size() + p2.size());

				for (int i = 0; i != p2.size(); ++i)
				{
					if (operation(p2[i]) && *operation(p2[i]).get_lhs() == constant::make(1))
					{
						(*p2[i])<<(*operation(p2[i]).get_rhs());
					}
					else
					{
						*p2[i] = operation::make(constant::make(1), oper_name::div, ~*p2[i]);
					}
					p1.push_back(p2[i]);
				}
			}

			return p1;
		}
		else
		{
			return { ~*a };
		}

	}
	else
	{
		return { ~*a };
	}
}
