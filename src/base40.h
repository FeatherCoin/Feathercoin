/* Encode strings as base40.
   Lizhi
*/

#ifndef FEATHERCOIN_BASE40_H
#define FEATHERCOIN_BASE40_H

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <cctype>
#include <iostream>

#include <sstream>
#include <algorithm>

static const char* pszBase40 = "0123456789abcdefghijklmnopqrstuvwxyz-_.+";
static const char* pszBase16 = "0123456789abcdef";


//Turn a string into a non-negative integer.
uint64_t charset_to_int(const unsigned char* pbegin, const unsigned char* pend)
{
	uint64_t output = 0;
	while (pbegin != pend)
	{
		const char *ch = strchr(pszBase40, *pbegin);
		if (ch == NULL)
            return 404;
		int carry = ch - pszBase40; //indexof
		output = output * strlen(pszBase40) + carry;
		pbegin++;
	}
	return output;
}

void i64tohex(uint64_t n,char *s)
{
    char base[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
    uint64_t a = n;
    int i = 0;
    while(a != 0)
    {
        s[i++] = base[a%16];
        a/=16;
    }
}

//Turn a non-negative integer into a string.
std::string int_to_charset(uint64_t val)
{
	if (val < 0)	 return "ERROR";
	if (val == 0)	 return "0";
		 
	std::string output;
	char a[80] = {0};
	i64tohex(val,a);
	std::string tmp;
	tmp.assign(a);
	
	const char *s1 = tmp.c_str();
	char b[80] = {0};
	int p=0;
	for (int i=(strlen(s1)-1);i>=0;i--)
	{
			b[p]=a[i];
			p++;
	}
	
	output.assign(b);
	return output;
}

std::string hexpad(std::string val)
{
	std::string output="0";
	if ((val.length()%2)==1)
	{
		output=output+val;
		return output;
	}
	else
	{
		return val;
	}
}

std::string charset_to_hex(const std::vector<unsigned char>& vch)
{
	uint64_t intermediate_integer=charset_to_int(&vch[0],&vch[0] + vch.size());
	
	std::string output;
	output=int_to_charset(intermediate_integer);
	output=hexpad(output);
	return output;
}





void unhexlify(const std::string &hex, std::vector<unsigned char> &bytes)
{
  for (std::size_t i = 0; i < hex.size(); i += 2)
  {
    int byte;
    std::stringstream ss;
    ss << std::hex << hex[i] << hex[i + 1];
    ss >> byte;
    bytes.push_back(byte);
  }
}

std::string unhexlify(std::string inData)
{
	std::vector<unsigned char> vch;
	unhexlify(inData,vch);
	
	std::string output(vch.begin(),vch.end());	
	return output;
}



#endif // FEATHERCOIN_BASE40_H