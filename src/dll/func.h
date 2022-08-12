#pragma once
#include <iostream>
using namespace std;

inline string& replace_all_distinct(string& str, const  string& old_value, const   string& new_value)
{
    for (string::size_type pos(0); pos != string::npos; pos += new_value.length())
    {
        if ((pos = str.find(old_value, pos)) != string::npos)
        {
            str.replace(pos, old_value.length(), new_value);
        }
        else
        {
            break;
        }
    }
    return   str;
}
