#pragma once
#include"parsing.h"

std::locale used_locale;

namespace parse_piece
{
	//what is allowed when an element is expected; after an operator, open parenthase, or beginning of an entire expression.
	//brings iter to the end of the piece
	//iter must not be std::end() or whitespace
	exp_part at_elem(std::string::const_iterator& iter, const std::string::const_iterator stop)
	{
		exp_part ret;

		//open paren
		if (*iter == '(')
		{
			ret.typeof = exp_part::paren_open;
			++iter;
		}
		//variable or start of function name
		else if (std::islower(*iter, used_locale))
		{
			ret.typeof = exp_part::character;
			ret.data = char(*iter);
			++iter;
		}
		//number
		else if (std::isdigit(*iter, used_locale))
		{
			ret.typeof = exp_part::number;
			std::stringstream numvalss;
			numvalss << *iter;
			++iter;

			while (iter != stop && std::isdigit(*iter, used_locale))
			{
				numvalss << *iter;
				++iter;
			}
			num_type numvalnt;
			numvalss >> numvalnt;
			ret.data = std::move(numvalnt); //for if num_type is arbitrary prescision.
		}
		//negative sign
		else if (*iter == '-')
		{
			ret.typeof = exp_part::negative;
			++iter;
		}
		//decimal point prefix
		else if (*iter == '.')
		{
			ret.typeof = exp_part::decimal_point;
			num_type zerocount = 1;
			while (iter + 1 != stop && *(iter+1) == '0')
			{
				zerocount *= 10;
				++iter;
			}
			ret.data = zerocount;
			++iter;
		}
		//message
		else if (*iter == '"')
		{
			++iter;
			if (iter == stop)
			{
				ret.typeof = exp_part::parsing_error;
				ret.data = std::string("unmatched open quote");
				return ret;
			}
			std::string strofret;
			while (*iter != '"')
			{
				strofret.push_back(*iter);
				++iter;
				if (iter == stop)
				{
					ret.typeof = exp_part::parsing_error;
					ret.data = std::string("unmatched open quote");
					return ret;
				}
			}

			ret.typeof = exp_part::message;
			ret.data = std::move(strofret);
			++iter;
		}
		//empty
		else if (*iter == '#')
		{
			ret.typeof = exp_part::empty;
			++iter;
		}
		//placeholder
		else if (*iter == '@')
		{
			ret.typeof = exp_part::place_holder;
			++iter;
		}
		//nothing matches
		else
		{
			ret.typeof = exp_part::parsing_error;
			ret.data = std::string("expected element or prefix");
		}

		return ret;
	}


	//what is allowed emediatly after an element; after a number, character, closed parenthase,
	//brings iter to the end of the piece
	//iter must not be std::end() or whitespace
	exp_part after_elem(std::string::const_iterator& iter, const std::string::const_iterator stop)
	{
		exp_part ret;

		//open paren
		if (*iter == '(')
		{
			ret.typeof = exp_part::paren_open;
			++iter;
		}
		else if (*iter == ')')
		{
			ret.typeof = exp_part::paren_close;
			++iter;
		}
		else if (*iter == ',')
		{
			ret.typeof = exp_part::function_comma;
			++iter;
		}
		else if (*iter == '.')
		{
			ret.typeof = exp_part::decimal_point;
			num_type zerocount = 1;
			while (iter + 1 != stop && *(iter + 1) == '0')
			{
				zerocount*=10;
				++iter;
			}
			ret.data = zerocount;
			if (zerocount!=1 && (iter + 1 == stop || !std::isdigit(*(iter + 1),used_locale)))
			{

			}
			++iter;
		}
		//variable or start of function name
		else if (std::islower(*iter, used_locale))
		{
			ret.typeof = exp_part::character;
			ret.data = char(*iter);
			++iter;
		}
		//number
		else if (std::isdigit(*iter, used_locale))
		{
			ret.typeof = exp_part::number;
			std::stringstream numvalss;
			numvalss << *iter;
			++iter;

			while (iter != stop && std::isdigit(*iter, used_locale))
			{
				numvalss << *iter;
				++iter;
			}
			num_type numvalnt;
			numvalss >> numvalnt;
			ret.data = std::move(numvalnt); //for if num_type is arbitrary prescision.
		}
		//operation
		else if (*iter == '+' || *iter == '-' || *iter == '*' || *iter == '/' || *iter == '^')
		{
			ret.typeof = exp_part::operation;
			ret.data = oper_name(*iter);
			++iter;
		}
		//nothing matches
		else
		{
			ret.typeof = exp_part::parsing_error;
			ret.data = std::string("expected element, operation, or closing");
		}

		return ret;
	}

	exp_part at_number(std::string::const_iterator& iter, const std::string::const_iterator stop)
	{
		exp_part ret;

		//number
		if (std::isdigit(*iter, used_locale))
		{
			ret.typeof = exp_part::number;
			std::stringstream numvalss;
			numvalss << *iter;
			++iter;

			while (iter != stop && std::isdigit(*iter, used_locale))
			{
				numvalss << *iter;
				++iter;
			}
			num_type numvalnt;
			numvalss >> numvalnt;
			ret.data = std::move(numvalnt); //for if num_type is arbitrary prescision.
		}
		//nothing matches
		else
		{
			ret.typeof = exp_part::parsing_error;
			ret.data = std::string("expected digit");
		}


		return ret;
	}

}

std::vector<exp_part> initial_parse(std::string::const_iterator a, std::string::const_iterator stop, parsing_rule_set const& rules)
{
	std::vector<exp_part> ret;

	int what_is_expected = 1; //0 is after an elem, 1 is at an elem, 2 is at a number

	while (true)
	{
		while (a != stop && *a == ' ')
		{
			++a;
		}

		if (a == stop)
		{
			return ret;
		}
		else
		{
			exp_part curpart;
			if (what_is_expected == 1)
			{
				curpart = parse_piece::at_elem(a, stop);
			}
			else if (what_is_expected==0)
			{
				curpart = parse_piece::after_elem(a, stop);
			}
			else if (what_is_expected == 2)
			{
				curpart = parse_piece::at_number(a, stop);
			}
			switch (curpart.typeof)
			{
			case(exp_part::place_holder):
			case(exp_part::character):
			case(exp_part::number):
			case(exp_part::paren_close):
			case(exp_part::message):
			case(exp_part::empty):
				what_is_expected = 0;
				break;
			case(exp_part::function_comma):
			case(exp_part::paren_open):
			case(exp_part::negative):
			case(exp_part::operation):
				what_is_expected = 1;
				break;
			case(exp_part::decimal_point):
				what_is_expected = 2;
				break;
			case(exp_part::parsing_error):
			default:
				ret.clear();
				ret.push_back(std::move(curpart));
				return ret;
				break;
			}
			ret.push_back(curpart);

		}
	}
}

void change_functs_and_variables(std::vector<exp_part>& a, parsing_rule_set const& rules)
{
	std::vector<exp_part> changed;
	for (auto it = a.begin(); it != a.end(); ++it)
	{
		std::vector<std::string const*> names;

		for (int i = 0; i != rules.function_names.size(); ++i)
		{
			names.push_back(&rules.function_names[i]);
		}

		if (it->typeof != exp_part::character)
		{
			changed.push_back(*it);
		}
		else
		{
			auto first = it; //so we can remember the first character in this possible sequence of characters
			auto cur_names = names; //making a copy so we dont have to remake "names" everytime this block is hit

			bool out_of_names = false;

			while ((!out_of_names) && ((it!=a.end()) && it->typeof == exp_part::character))//until this sequence of characters no longer matches an allowed function name, or it ends
			{
				char cc = std::get<char>(it->data); //current character

				out_of_names = true; //temporarily. will be set false once we find a non nullptr

				for (int i = 0; i != cur_names.size(); ++i)
				{
					if (cur_names[i] != nullptr)
					{
						if ((*cur_names[i])[std::distance(first,it)] != cc)
						{
							cur_names[i] = nullptr;
						}
						else //if cur_names[i] doesn't now have a nullptr, and didn't before
						{
							out_of_names = false;
						}
					}
				}

				++it;
			}


			bool has_found_function = false;

			if (!out_of_names)
			{
				for (int i = 0; i != cur_names.size(); ++i)
				{
					if (cur_names[i] != nullptr && cur_names[i]->length() == std::distance(first, it)) //if it were longer than the distance of our sequence, it wouldn't match the whole function name, only a piece of it.
					{
						exp_part pb;
						pb.typeof = exp_part::function_name;
						pb.data = *cur_names[i];
						changed.push_back(pb);
						has_found_function = true;
						--it;
					}
				}
			}

			if (!has_found_function)
			{
				it = first;
				while (it!=a.end() && it->typeof == exp_part::character) //all consecutive characters starting at the start of this sequence are variables
				{
					exp_part pb;
					pb.typeof = exp_part::variable;
					pb.data = var_name(std::get<char>(it->data));
					changed.push_back(pb);
					++it;
				}
				--it;
			}
		}

	}
	a = std::move(changed);
}

void change_decimal_points(std::vector<exp_part>& a, parsing_rule_set const& rules)
{
	for (int i = 0; i != a.size(); ++i)
	{
		if (a[i].typeof == exp_part::decimal_point)
		{
			if (i + 1 == a.size() || a[i+1].typeof !=exp_part::number)
			{
				exp_part er;
				er.typeof = exp_part::parsing_error;
				er.data = std::string("bad decimal point");
				a = { er };
				return;
			}
			num_type leftnum;
			num_type rightnum; //vals

			int rightlen; //length of rightnum

			if (i == 0 || a[i - 1].typeof != exp_part::number)
			{
				leftnum = 0;

				exp_part ins;
				ins.typeof = exp_part::paren_open;
				a.insert(a.begin() + i, ins);
				++i;
			}
			else
			{
				leftnum = std::get<num_type>(a[i - 1].data);

				exp_part set;
				set.typeof = exp_part::paren_open;
				a[i - 1] = set;
			}

			rightnum = std::get<num_type>(a[i + 1].data);
			std::stringstream s;
			s << rightnum;
			rightlen = s.str().length();

			num_type numerator = leftnum;
			num_type denomenator = std::get<num_type>(a[i].data);

			for (int i = 0; i != rightlen; ++i)
			{
				numerator *= 10;
				denomenator *= 10;
			}

			numerator = std::get<num_type>(a[i].data) * numerator;

			numerator = numerator + rightnum;

			

			exp_part leftarg, divide, rightarg, closepar;

			leftarg.typeof = exp_part::number;
			leftarg.data = numerator;

			divide.typeof = exp_part::operation;
			divide.data = oper_name::div;

			rightarg.typeof = exp_part::number;
			rightarg.data = denomenator;

			closepar.typeof = exp_part::paren_close;

			a[i] = leftarg;
			a[i + 1] = divide;
			a.insert(a.begin() + i + 2, { rightarg,closepar });
			

		}
	}
}


//only knows about open parens and close parens. start it at an open paren, it returns its corrosponding close paren
std::vector<exp_part>::iterator skip_nesting(std::vector<exp_part>::iterator start, std::vector<exp_part>::iterator bounds)
{
	++start;
	while (start != bounds)
	{

		if (start->typeof == exp_part::paren_open)
		{
			start = 1+skip_nesting(start, bounds);
			continue;
		}
		else if (start->typeof == exp_part::paren_close)
		{
			return start;
		}

		++start;

	}
	return bounds;
}


void change_logarithms(std::vector<exp_part>& a, parsing_rule_set const& rules)
{

	for (int i = 0; i != a.size(); ++i)
	{
		if (a[i].typeof == exp_part::function_name&&std::get<std::string>(a[i].data) == "log")
		{
			if (i + 1 != a.size())
			{
				{
					exp_part ins;
					ins.typeof = exp_part::paren_open;
					a.insert(a.begin() + i, ins);
					++i;
				}
				if (a[(i + 1)].typeof == exp_part::paren_open)
				{
					
					a.erase(a.begin() + i);

					if (skip_nesting(a.begin() + i, a.end()) == a.end())
					{
						exp_part retv;
						retv.typeof = exp_part::parsing_error;
						retv.data = std::string("logarithm missing base");
						a = { retv };
						return;
					}
					
					std::insert_iterator<std::vector<exp_part>> ins_iter{ a, skip_nesting(a.begin() + i, a.end())+1 };

					exp_part ins;
					ins.typeof = exp_part::operation;
					ins.data = oper_name::log;
					ins_iter = ins;

					std::vector<exp_part>::iterator iter = skip_nesting(a.begin() + i, a.end())+1;

					std::vector<exp_part> subpart;

					for (auto b = a.begin() + i; b != iter; ++b)
					{
						subpart.push_back(*b);
					}

					change_logarithms(subpart, rules);

					for (int wi = 0; wi!=subpart.size(); ++wi)
					{
						*(a.begin() + i + wi) = subpart[wi];
					}

					i += subpart.size(); //places i at the log operation
					
				}
				else if ((a[(i + 1)].typeof == exp_part::decimal_point || a[(i + 1)].typeof == exp_part::negative))
				{
					exp_part retv;
					retv.typeof = exp_part::parsing_error;
					retv.data = std::string("logarithm missing base");
					a = { retv };
					return;
				}
				else
				{
					a.erase(a.begin() + i);

					exp_part ins;
					ins.typeof = exp_part::operation;
					ins.data = oper_name::log;

					a.insert(a.begin() + i + 1, ins);
					i += 1;
				}
				if (i + 1 != a.size())
				{
					auto e = skip_nesting(a.begin() + i + 1, a.end());
					if (e == a.end())
					{
						exp_part retv;
						retv.typeof = exp_part::parsing_error;
						retv.data = std::string("bad parenthases after logarithm");
						a = { retv };
						return;
					}
					else
					{
						exp_part ins;
						ins.typeof = exp_part::paren_close;
						a.insert(e + 1, ins);
					}
				}
			}
		}
		
	}
}


//changes functions to consist of a name, start and an end. no parenthases.
void change_function_parenthases(std::vector<exp_part>& a, parsing_rule_set const& rules)
{
	std::vector<exp_part> changed;
	for (int i = 0; i != a.size(); ++i)
	{
		if (a[i].typeof == exp_part::function_name)
		{
			if (i + 1 == a.size() || a[i + 1].typeof != exp_part::paren_open)
			{
				exp_part retv;
				retv.typeof = exp_part::parsing_error;
				retv.data = std::string("expected open parenthase after function name");
				a = { retv };
				return;
			}
			else
			{
				changed.push_back(a[i]);
				auto endof = skip_nesting(a.begin() + i + 1, a.end());
				if (endof == a.end())
				{
					exp_part retv;
					retv.typeof = exp_part::parsing_error;
					retv.data = std::string("failed to find matching close parenthase");
					a = { retv };
					return;
				}
				std::vector<exp_part> skipped;
				for (auto it = a.begin() + i + 2; it != endof; ++it)
				{
					skipped.push_back(*it);
				}
				change_function_parenthases(skipped,rules);

				{
					exp_part pb;
					pb.typeof = exp_part::function_start;
					changed.push_back(pb);
				}
				for (int ni = 0; ni != skipped.size(); ++ni)
				{
					changed.push_back(skipped[ni]);
				}
				{
					exp_part pb;
					pb.typeof = exp_part::function_end;
					changed.push_back(pb);
				}
				i += std::distance(a.begin() + i, endof);
			}
		}
		else
		{
			changed.push_back(a[i]);
		}
	}

	a = std::move(changed);

}



//when started at a open parenthase, it will return the corrosponding close parenthase. when started at a function name, it will return the end of said function. when started at any single piece elem, it will return the starting location. 
std::vector<exp_part>::iterator skip_elem_right(std::vector<exp_part>::iterator start, std::vector<exp_part>::iterator bounds)
{
	if (start == bounds)
	{
		return bounds;
	}

	switch (start->typeof)
	{
	case(exp_part::function_name):
		++start;
		while (start != bounds)
		{

			if (start->typeof == exp_part::paren_open || start->typeof == exp_part::function_name)
			{
				start = skip_elem_right(start, bounds);
				if (start == bounds)
				{
					return bounds;
				}
				++start;
				continue;
			}


			if (start->typeof == exp_part::function_end)
			{
				return start;
			}

			++start;

		}
		return bounds;
		break;

	case(exp_part::paren_open):
		++start;
		while (start != bounds)
		{

			if (start->typeof == exp_part::paren_open || start->typeof == exp_part::function_name)
			{
				start = skip_elem_right(start, bounds);
				if (start == bounds)
				{
					return bounds;
				}
				++start;
				continue;
			}


			if (start->typeof == exp_part::paren_close)
			{
				return start;
			}

			++start;

		}
		return bounds;
		break;

	case(exp_part::negative):
		return skip_elem_right(start + 1, bounds);
		break;

	default:
		return start;
		break;
	}
}


//when started at a closed parenthase, it will return the corrosponding open parenthase. when started at a function end, it will return the name of the function. when started at any single piece elem, it will return the starting location. 
std::vector<exp_part>::reverse_iterator skip_elem_left(std::vector<exp_part>::reverse_iterator start, std::vector<exp_part>::reverse_iterator bounds)
{
	if (start == bounds)
	{
		return bounds;
	}

	switch (start->typeof)
	{
	case(exp_part::function_end):
		++start;
		while (start != bounds)
		{

			if (start->typeof == exp_part::paren_close || start->typeof == exp_part::function_end)
			{
				start = skip_elem_left(start, bounds);
				if (start == bounds)
				{
					return bounds;
				}
				++start;
				continue;
			}


			if (start->typeof == exp_part::function_name)
			{
				return start;
			}

			++start;

		}
		return bounds;
		break;

	case(exp_part::paren_close):
		++start;
		while (start != bounds)
		{

			if (start->typeof == exp_part::paren_close || start->typeof == exp_part::function_end)
			{
				start = skip_elem_left(start, bounds);
				if (start == bounds)
				{
					return bounds;
				}
				++start;
				continue;
			}


			if (start->typeof == exp_part::paren_open)
			{
				return start;
			}

			++start;

		}
		return bounds;
		break;

	default:
		return start;
		break;
	}
}



void parenthasize_exponential(std::vector<exp_part>& a)
{
	for (int i = a.size()-1; i >= 0; --i)
	{
		if (a[i].typeof == exp_part::operation && std::get<oper_name>(a[i].data) == oper_name::exp)
		{
			auto re = skip_elem_right(a.begin() + i + 1, a.end());
			auto le = skip_elem_left(a.rbegin() + (a.size() - i), a.rend());

			if (re == a.end() || le == a.rend())
			{
				exp_part ret;
				ret.typeof = exp_part::parsing_error;
				ret.data = std::string("bad operand");
				a = { ret };
				return;
			}

			if ((re + 1 == a.end() || (re + 1)->typeof != exp_part::paren_close) || (le + 1 == a.rend() || (le + 1)->typeof != exp_part::paren_open))
			{
				exp_part rins;
				rins.typeof = exp_part::paren_close;
				a.insert(re + 1, rins);
				exp_part lins;
				lins.typeof = exp_part::paren_open;
				a.insert((skip_elem_left(a.rbegin() + (a.size() - i), a.rend())+1).base(), lins); //need to redo the calculation because the old iterator is now invalid
				++i;
			}

		}

	}
}


bool is_val_beg(exp_part& a)
{
	switch (a.typeof)
	{
	case(exp_part::variable):
	case(exp_part::number):
	case(exp_part::function_name):
	case(exp_part::paren_open):
	case(exp_part::empty):
	case(exp_part::message):
	case(exp_part::place_holder):
		return true;
	default:
		return false;
	}
}

bool is_val_end(exp_part& a)
{
	switch (a.typeof)
	{
	case(exp_part::variable):
	case(exp_part::number):
	case(exp_part::function_end):
	case(exp_part::paren_close):
	case(exp_part::empty):
	case(exp_part::message):
	case(exp_part::place_holder):
		return true;
	default:
		return false;
	}
}

void make_implicit_multiplication_explicit(std::vector<exp_part>& a)
{
	bool areafter = false;

	for (int i = 0; i != a.size(); ++i)
	{
		if (areafter)
		{
			if (is_val_beg(a[i]))
			{
				exp_part ins;
				ins.typeof = exp_part::operation;
				ins.data = oper_name::mul;
				a.insert(a.begin() + i, std::move(ins));
			}
		}

		if (is_val_end(a[i]))
		{
			areafter = true;
		}
		else
		{
			areafter = false;
		}
	}
}

void parenthasize_multiplicative(std::vector<exp_part>& a)
{
	for (int i = 0; i != a.size(); ++i)
	{
		if (a[i].typeof == exp_part::operation && (std::get<oper_name>(a[i].data) == oper_name::mul || std::get<oper_name>(a[i].data) == oper_name::div))
		{
			auto re = skip_elem_right(a.begin() + i + 1, a.end());
			auto le = skip_elem_left(a.rbegin() + (a.size() - i), a.rend());

			if (re == a.end() || le == a.rend())
			{
				exp_part ret;
				ret.typeof = exp_part::parsing_error;
				ret.data = std::string("bad operand");
				a = { ret };
				return;
			}

			if ((re + 1 == a.end() || (re + 1)->typeof != exp_part::paren_close) || (le + 1 == a.rend() || (le + 1)->typeof != exp_part::paren_open))
			{
				exp_part rins;
				rins.typeof = exp_part::paren_close;
				a.insert(re + 1, rins);
				exp_part lins;
				lins.typeof = exp_part::paren_open;
				a.insert((skip_elem_left(a.rbegin() + (a.size() - i), a.rend())+1).base(), lins); //need to redo the calculation because the old iterator is now invalid
				++i;
			}

		}

	}
}

void parenthasize_additive(std::vector<exp_part>& a)
{
	for (int i = 0; i != a.size(); ++i)
	{
		if (a[i].typeof == exp_part::operation && (std::get<oper_name>(a[i].data) == oper_name::add || std::get<oper_name>(a[i].data) == oper_name::sub))
		{
			auto re = skip_elem_right(a.begin() + i + 1, a.end());
			auto le = skip_elem_left(a.rbegin() + (a.size() - i), a.rend());

			if (re == a.end() || le == a.rend())
			{
				exp_part ret;
				ret.typeof = exp_part::parsing_error;
				ret.data = std::string("bad operand");
				a = { ret };
				return;
			}

			if ((re + 1 == a.end() || (re + 1)->typeof != exp_part::paren_close) || (le + 1 == a.rend() || (le + 1)->typeof != exp_part::paren_open))
			{
				exp_part rins;
				rins.typeof = exp_part::paren_close;
				a.insert(re + 1, rins);
				exp_part lins;
				lins.typeof = exp_part::paren_open;
				a.insert((skip_elem_left(a.rbegin() + (a.size() - i), a.rend())+1).base(), lins); //need to redo the calculation because the old iterator is now invalid
				++i;
			}

		}

	}
}

void parenthasize_left_to_right(std::vector<exp_part>& a)
{

	for (int i = 0; i != a.size(); ++i)
	{
		if (a[i].typeof == exp_part::operation)
		{
			auto re = skip_elem_right(a.begin() + i + 1, a.end());
			auto le = skip_elem_left(a.rbegin() + (a.size() - i), a.rend());

			if (re == a.end() || le == a.rend())
			{
				exp_part ret;
				ret.typeof = exp_part::parsing_error;
				ret.data = std::string("bad operand");
				a = { ret };
				return;
			}

			if ((re + 1 == a.end() || (re + 1)->typeof != exp_part::paren_close) || (le + 1 == a.rend() || (le + 1)->typeof != exp_part::paren_open))
			{
				exp_part rins;
				rins.typeof = exp_part::paren_close;
				a.insert(re + 1, rins);
				exp_part lins;
				lins.typeof = exp_part::paren_open;
				a.insert((skip_elem_left(a.rbegin() + (a.size() - i), a.rend()) + 1).base(), lins); //need to redo the calculation because the old iterator is now invalid
				++i;
			}

		}

	}
}


void change_negative_signs(std::vector<exp_part>& a, std::string const& rep)
{
	auto rules = parsing_rule_set::most_basic_rules();
	auto neg = initial_parse(rep.begin(), rep.end(), rules);

	change_functs_and_variables(neg, rules);
	change_decimal_points(neg, rules);

	change_logarithms(neg, rules);

	change_function_parenthases(neg, rules);


	std::vector<exp_part> changed;

	for (int i = 0; i != a.size(); ++i)
	{
		if (a[i].typeof == exp_part::negative)
		{
			i += 1;
			if (i == a.size()) //can't have more than two negative signs in a row
			{
				exp_part ret;
				ret.typeof = exp_part::parsing_error;
				ret.data = std::string("bad negative sign");
				a = { ret };
				return;
			}
			if (neg.size() == 0)
			{
				exp_part ret;
				ret.typeof = exp_part::parsing_error;
				ret.data = std::string("bad negative sign");
				a = { ret };
				return;
			}
			for (auto it = neg.begin(); it != neg.end(); ++it)
			{
				if (it->typeof == exp_part::place_holder)
				{
					auto save = it;
					it = a.begin() + i;
					auto skip = skip_elem_right(a.begin()+i, a.end());
					if (skip == a.end())
					{
						exp_part ret;
						ret.typeof = exp_part::parsing_error;
						ret.data = std::string("bad negative sign");
						a = { ret };
						return;
					}
					++skip;
					std::vector<exp_part> piece;
					while (it != skip)
					{
						piece.push_back(std::move(*it));
						++it;
						++i;
					}
					change_negative_signs(piece, rep);
					for (auto pbs = piece.begin(); pbs != piece.end(); ++pbs)
					{
						changed.push_back(std::move(*pbs));
					}
					it = save;
				}
				else
				{
					changed.push_back(*it);
				}
			}
		}
		if(i!=a.size())
		{
			changed.push_back(std::move(a[i]));
		}
		else
		{
			break;
		}
	}
	a = std::move(changed);
}


std::vector<exp_part> full_parse(std::string::const_iterator a, std::string::const_iterator stop, parsing_rule_set const& rules)
{
	auto ret = initial_parse(a, stop, rules);

	change_functs_and_variables(ret, rules);
	change_decimal_points(ret, rules);
	
	change_logarithms(ret, rules);

	change_function_parenthases(ret, rules);

	if (rules.use_precedence)
	{
		parenthasize_exponential(ret);

		make_implicit_multiplication_explicit(ret);

		change_negative_signs(ret, rules.negative_sign_template);

		parenthasize_multiplicative(ret);
		parenthasize_additive(ret);
	}
	else
	{
		make_implicit_multiplication_explicit(ret);

		change_negative_signs(ret, rules.negative_sign_template);

		parenthasize_left_to_right(ret);
	}
	

	return ret;
}
