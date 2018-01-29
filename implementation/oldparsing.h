#pragma once
#include<string>
#include<locale>
#include<vector>

const char negative_one_char = '@';
const char empty_value_char = '#';

void prepare_string(std::string& tar, std::vector<std::string> funct_names = {}, int start = 0, int end = -1);

std::string remove_whitespace(const std::string& tar,int start = 0,int end = -1)
{
	if (end == -1)
	{
		end = tar.length();
	}

	std::string ret;

	for (int i = start; i != end; ++i)
	{
		if (tar[i] != ' ')
		{
			ret.push_back(tar[i]);
		}
	}

	return ret;
}

bool isvarchar(char a)
{
	try 
	{
		std::locale l;
		return std::isalpha(a, l) || (a == empty_value_char) || (a == negative_one_char);
	}
	catch (...)
	{
		return false;
	}
}

bool isnumchar(char a)
{
	try
	{
		std::locale l;
		return std::isdigit(a, l);
	}
	catch (...)
	{
		return false;
	}
}



bool isoperator(char a)
{
	return (a == '+' || a == '-' || a == '*' || a == '/' || a == '^' || a == '_' || a==','); // commas are not parsed into operators, but it helps to pretend that they are such to make moving through a function easier
}


//make "std::string a" be const reference
//when finding the first char in a val from the last, it returns that the location of the first char. when finding the last character of a value, it returns the first character not in that value.
int endofval(const std::string& a, unsigned int start, bool forwards = true)
{

	int incam = -1;
	if (forwards)
	{
		incam = 1;
	}

	if (a.size()>(start+1)&&a[start + 1] == '$') //skiping functions
	{
		++start;
		while (a[start] != '(')
		{
			++start;
		}
		return endofval(a, start) + 1;
	}

	int origional = start;
	if (a[start] == ')' || a[start] == '(')
	{
		int depth = 1; //depth into parenthases nesting
		while (depth != 0)
		{
			start += incam;
			if (a[start] == '(')
			{
				depth += incam;
			}
			if (a[start] == ')')
			{
				depth -= incam;
			}
		}
		if (forwards)
		{
			++start;
		}
	}
	else if (isvarchar(a[start]))
	{
		if (incam == 1)
		{
			return start + 1;
		}
		else
		{
			return start;
		}
	}
	else if (isnumchar(a[start]))
	{
		while (isnumchar(a[start]))
		{
			if (incam == -1 && start == 0)
			{
				return start;
			}
			if (start >= a.size())
			{
				start -= incam;
				break;
			}
			start += incam;
		}
		if (!forwards)
		{
			++start;
		}
	}

	return start;

}

void decimals_to_rationals(std::string& tar)
{

}



void change_functions(std::string& tar, std::vector<std::string>& functs,int whichname)
{
	int i = 0;
	while (true)
	{
		int locoffunct = tar.find(functs[whichname], i);

		if (locoffunct == std::string::npos)
		{
			return;
		}

		tar.insert(locoffunct, "($");
		tar.insert(endofval(tar, locoffunct + 2 + functs[whichname].size()), ")");

		locoffunct += 2;

		int opep = locoffunct + functs[whichname].size();
		int over = endofval(tar, locoffunct + functs[whichname].size());

		std::string funct_arg = tar.substr(opep+1, over-opep-2);
		prepare_string(funct_arg,functs);
		

		tar.replace(opep+1, over - opep - 2, funct_arg);
		i = endofval(tar, locoffunct + functs[whichname].size());
	}
}


//for example will change "log2(8)" to 2_8
//but just log(8) or log2 will crash
void change_logarithms(std::string& tar)
{
	size_t locoflog;

	while (true)
	{
		locoflog = tar.find("log", 0);
		if (locoflog == std::string::npos)
		{
			break;
		}


		int endofbase = endofval(tar, locoflog + 3);

		/*
		int endofarg = endofval(tar, endofbase+1);
		
		// leaving this here for if the end of the arguments ever becomes important
		while (tar[endofarg] != ')')
		{
			if (isoperator(tar[endofarg]))
			{
				++endofarg;
			}
			endofarg = endofval(tar, endofarg);
		}
		*/

		if (tar[endofval(tar, endofbase + 1)] == ')') //if the argument of log is a single value
		{
			tar[endofbase] = '_';
			tar[locoflog + 2] = '(';
		}
		else
		{
			tar.insert(endofbase, "_");
			tar[locoflog + 2] = ' ';
		}


		tar[locoflog] = ' ';
		tar[locoflog + 1] = ' ';


		tar = remove_whitespace(tar);

	}


}



void parenthesize_exp(std::string& tar)
{
	std::vector<int> op_locs;
	for (int i = 0; i != tar.size(); ++i)
	{
		if (tar[i] == '^' || tar[i] == '_')
		{
			op_locs.push_back(i);
		}
	}


	for (int i = op_locs.size() - 1; i >= 0; --i)
	{
		int front;
		int end;
		if (tar[op_locs[i]+1] == negative_one_char)
		{
			end = endofval(tar, op_locs[i] + 2, true);
		}
		else
		{
			end = endofval(tar, op_locs[i] + 1, true);
		}
		front = endofval(tar, op_locs[i] - 1, false);

		if (tar[end] == ')'&&tar[front - 1] == '(')
		{
			continue;
		}

		tar.insert(tar.begin() + front, '(');
		tar.insert(tar.begin() + end + 1, ')');
		for (int i2 = i; i2 >= 0; --i2)
		{
			if (op_locs[i2] >= front) { op_locs[i2] += 1; }
		}
	}



}

void multiply_negative_numbers(std::string& tar)
{
	if (tar[0] == '-')
	{
		tar[0] = negative_one_char;
	}
	for (int i = 0; i != tar.size() - 1; ++i)
	{
		if (tar[i] == '(' || isoperator(tar[i]))
		{
			if (tar[i + 1] == '-')
			{
				tar[i + 1] = negative_one_char;
				//from here
				int curp = endofval(tar,i + 2);
				while (tar[curp] == '^' || tar[curp] == '_')
				{
					curp = endofval(tar, curp + 1);
				}
				tar.insert(tar.begin() + curp, ')');
				tar.insert(tar.begin() + i + 1, '(');
				//to here is new code
			}
			
		}
	}
}

//adds signs for implied multiplications (for example -54x(x+2)(4^x) would become (-1)*54*x*(x+2)*(4^x)
void add_multiply_signs(std::string& tar)
{
	for (int i = 0; i < tar.size(); ++i) //"i" might NEED to be signed (dont change it unless absolutly nessicaryp
	{
		bool was_nonval = false;
		while (isoperator(tar[i]) || tar[i] == '(')
		{
			was_nonval = true;

			if (tar[i] == '('&&i!=0)
			{
				if (tar[i - 1] == ')')
				{

					tar.insert(tar.begin() + i, '*');
					
				}
			}
			++i;
		}

		if (was_nonval && i != 0 && tar[i - 1] == ')')
		{
			tar.insert(i, "*");
			++i;
		}

		if (tar[i] == '$')
		{
			i -= 1;
			if (i == tar.size())
			{
				return;
			}
		}

		bool loopcalled = false;

		//at this point we must be at the first char of a value (comment may be outdated)
		while (tar[i] == ')')
		{
			loopcalled = true;
			++i;
			if (i == tar.size())
			{
				return;
			}
			
		}
		
		if (loopcalled&&(!isoperator(tar[i])))
		{
			tar.insert(i, "*");
			++i;
		}
		i = endofval(tar, i);

		if (i == tar.size())
		{
			return;
		}

		//at this point we are exactly one character after that value

		while (!(isoperator(tar[i]) || tar[i] == ')'))//while that one character, emediatly after some value that we're at, is a value
		{
			if (tar[i] != '$')//to avoid putting * before a function (might no longer be nessicary?)
			{
				tar.insert(tar.begin() + i, '*');
				++i;
				i = endofval(tar, i);
			}
			else
			{
				i = endofval(tar, i - 1);
				++i;
			}
			
			//at this point we are once again exactly one character after the last value

			if (i >= tar.size())
			{
				return;
			}
		}
	}
}


void parenthesize_mult(std::string& tar)
{
	std::vector<int> op_locs;
	for (int i = 0; i != tar.size(); ++i)
	{
		if (tar[i] == '*' || tar[i] == '/')
		{
			op_locs.push_back(i);
		}
	}


	for (int i = 0; i != op_locs.size(); ++i)
	{
		int front = endofval(tar, op_locs[i] - 1, false);
		int end = endofval(tar, op_locs[i] + 1, true);

		if (tar[end] == ')'&&tar[front - 1] == '(')
		{
			continue;
		}

		tar.insert(tar.begin() + front, '(');
		tar.insert(tar.begin() + end + 1, ')');
		for (int i2 = i; i2 != op_locs.size(); ++i2)
		{
			int incamount = 0;
			if (op_locs[i2] >= front) { incamount += 1; }
			if (op_locs[i2] >= end) { incamount += 1; }

			op_locs[i2] += incamount;
		}
	}


}

void parenthesize_add(std::string& tar)
{
	std::vector<int> op_locs;
	for (int i = 0; i != tar.size(); ++i)
	{
		if (tar[i] == '+' || tar[i] == '-')
		{
			op_locs.push_back(i);
		}
	}


	for (int i = 0; i != op_locs.size(); ++i)
	{
		int front = endofval(tar, op_locs[i] - 1, false);
		int end = endofval(tar, op_locs[i] + 1, true);

		if (tar[end] == ')'&&tar[front - 1] == '(')
		{
			continue;
		}

		tar.insert(tar.begin() + front, '(');
		tar.insert(tar.begin() + end + 1, ')');
		for (int i2 = i; i2 != op_locs.size(); ++i2)
		{
			int incamount = 0;
			if (op_locs[i2] >= front) { incamount += 1; }
			if (op_locs[i2] >= end) { incamount += 1; }

			op_locs[i2] += incamount;
		}
	}


}

void change_negative_one_chars(std::string& tar)
{
	int f;

	while (true)
	{
		f = tar.find(negative_one_char);
		if (f == std::string::npos)
		{
			return;
		}

		tar[f] = '(';
		tar.insert(f+1, "0-1)");
	}
}


void prepare_string(std::string& tar, std::vector<std::string> funct_names, int start, int end)
{

	tar = remove_whitespace(" " + tar + " ",start,end);

	decimals_to_rationals(tar);

	multiply_negative_numbers(tar);

	change_logarithms(tar);//should come before change_functions?

	for (int i = 0; i != funct_names.size(); ++i)
	{
		change_functions(tar, funct_names,i);
	}


	parenthesize_exp(tar);
	add_multiply_signs(tar);
	parenthesize_mult(tar);
	parenthesize_add(tar);

	change_negative_one_chars(tar);

	
}
