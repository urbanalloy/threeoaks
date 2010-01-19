//////////////////////////////////////////////////////////////////////////
//
//	ini_file template class.
//	(c) Andreas Verhoeven, 2005
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <map>
#include <iterator>
#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <algorithm>

namespace ini
{

template<typename Elem> class ini_stream_functions
{
public:
	static void skip_char(std::basic_istream<Elem>& st, Elem skipChar)
	{
		bool isSpace;
		do
		{
			int read = st.peek();
			Elem ch = static_cast<Elem>(read);

			isSpace = read != EOF && ch == skipChar;
			if(isSpace)
				st.get();

		}while(isSpace);
	}
};

template<typename Elem> class ini_chars_traits
{
public:
	static const Elem SPACE                = ' ';
	static const Elem SECTION_HEADER_START = '[';
	static const Elem SECTION_HEADER_END   = ']';
	static const Elem KEY_VALUE_SEPERATOR  = '=';
};

template<typename Elem> class ini_converter_traits{};
template<> class ini_converter_traits<char>
{
public:
	typedef std::basic_string<char> string_type;
	typedef std::basic_stringstream<char> stringstream_type;

	std::string static convert(const string_type& str)
	{
		return str;
	}

	template<typename T> static string_type to_string(const T& t)
	{
		stringstream_type ss;
		ss << t;
		return ss.str();
	}

	template<typename T> static T from_string(const string_type& str, const T& defValue)
	{
		T t(defValue);
		stringstream_type ss(str);
		ss >> t;

		return t;
	}
};

template<> class ini_converter_traits<wchar_t>
{
public:
	typedef std::basic_string<wchar_t> string_type;
	typedef std::basic_stringstream<wchar_t> stringstream_type;

	std::string static convert(const string_type& str)
	{
		// --start of windows specific code
		// better than using C++ codept conversion functions, since
		// WCTMB maps to better to the system local codepage.
		size_t strLength = str.length() + 1;
		char* cStr = new char[strLength];
		WideCharToMultiByte(CP_OEMCP, 0, str.c_str(), -1, cStr, static_cast<int>(strLength), NULL, NULL);
		// --end of windows specific code
		std::string retString(cStr);
		delete[] cStr;
		return retString;
	}

	template<typename T> static string_type to_string(const T& t)
	{
		stringstream_type ss;
		ss << t;
		return ss.str();
	}

	template<typename T> static T from_string(const string_type& str, const T& defValue)
	{
		T t(defValue);
		stringstream_type ss(str);
		ss >> t;

		return t;
	}
};

template<typename Elem, typename ini_traits, typename converter_traits> class basic_ini_section
{
protected:
	typedef std::basic_string<Elem> string_type;
	typedef std::map<string_type, string_type> values_map;
	//typedef values_map::const_iterator values_iterator;

	values_map values;

public:
	template<typename T>  T value(const string_type& key, const T& defValue)const
	{
		values_map::const_iterator iter = values.find(key);
		if(iter == values.end())
			return defValue;
		else
			return converter_traits::from_string<T>(iter->second, defValue);
	}

	template<string_type>  string_type value(const string_type& key, const string_type& defValue)const
	{
		values_map::const_iterator iter = values.find(key);
		if(iter == values.end())
			return defValue;
		else
			return iter->second;
	}

	template<typename T> void set(const string_type& key, const T& value)
	{
		values[key]  = converter_traits::to_string(value);
	}

	template<string_type> void set(const string_type& key, const string_type& value)
	{
		values[key]  = value;
	}

	bool exists(const string_type& key)const
	{
		return values.find(key) != values.end();
	}


	void clear()
	{
		values.clear();
	}

	void save(std::basic_ostream<Elem>& st)const
	{
		for(values_map::const_iterator iter = values.begin(); iter != values.end(); ++iter)
			st << iter->first << ini_traits::KEY_VALUE_SEPERATOR << iter->second << std::endl;
	}

	void read(std::basic_istream<Elem>& st)
	{
		bool atEnd = false;
		while(!atEnd)
		{
			ini_stream_functions<Elem>::skip_char(st, ini_traits::SPACE);

			int next = st.peek();
			atEnd = !st || next == EOF || static_cast<Elem>(next) == ini_traits::SECTION_HEADER_START;

			if(!atEnd)
			{
				string_type line;
				std::getline(st, line);

				string_type::size_type equalPos = line.find_first_of(ini_traits::KEY_VALUE_SEPERATOR);
				if(equalPos != std::wstring::npos)
				{
					string_type key, value;

					std::copy(line.begin(), line.begin() + equalPos, std::insert_iterator<string_type>(key, key.begin()));
					std::copy(line.begin() + equalPos + 1, line.end(), std::insert_iterator<string_type>(value, value.begin()));
	
					values[key] = value;
				}
			}
		}
	}
};

template<typename Elem, typename ini_traits> class ini_output_traits
{
public:

	typedef std::basic_ostream<Elem> stream_type;

	stream_type& stream;

	ini_output_traits(stream_type& stream) : stream(stream){}

	template<typename sections_iterator> void operator()(const sections_iterator& iter)
	{
		stream << ini_traits::SECTION_HEADER_START << iter.first << ini_traits::SECTION_HEADER_END << std::endl;
		iter.second.save(stream);
	}
};


template<typename Elem, typename ini_traits = ini_chars_traits<Elem>, 
	typename converter_traits = ini_converter_traits<Elem>,
	typename output_traits = ini_output_traits<Elem, ini_traits>  > class basic_ini_file
{
public:
	typedef std::basic_string<Elem> string_type;
	typedef basic_ini_section<Elem, ini_traits, converter_traits> section_type;

	typedef std::map<string_type, section_type> sections_map;
	//typedef sections_map::const_iterator sections_iterator;
	sections_map sections;

	string_type  filename;
	bool         hasChanged;

	basic_ini_file() : hasChanged(false){}

	basic_ini_file(const string_type& filename) : hasChanged(false)
	{
		read(filename);
	}

	~basic_ini_file()
	{
		if(filename.length() > 0 && has_changed())
			save();
	}

	bool has_changed()const
	{
		return hasChanged;
	}

	void changed()
	{
		hasChanged = true;
	}

	void unchanged()
	{
		hasChanged = false;
	}


	template<typename T> T value(const string_type& section, const string_type& key, const T& def_value)const
	{
		if(!exists(section))
			return def_value;
		else
			return sections.find(section)->second.value(key, def_value);
	}	

	template<typename T> void set(const string_type& section, const string_type& key, const T& value)
	{
		changed();
		section_type& sect = sections[section];
		sect.set(key, value);
	}

	bool exists(const string_type& section)const
	{
		return sections.find(section) != sections.end();
	}

	void clear()
	{
		changed();
		sections.clear();
	}

	void save()
	{
		save(filename);
		unchanged();
	}

	void save(std::basic_ostream<Elem>& st)const
	{
		output_traits outputter(st);
		std::for_each(sections.begin(), sections.end(), outputter);
		
	}
	
	void save(const string_type& filename)const
	{

		std::basic_ofstream<Elem> file(converter_traits::convert(filename).c_str());
		if(file)
		{
			save(file);
		}
	}

	void read(const string_type& filename)
	{
		clear();
		unchanged();
		this->filename = filename;
		std::basic_ifstream<Elem> file(converter_traits::convert(filename).c_str());
		if(file)
			read(file);


	}

	void read(std::basic_ifstream<Elem>& stream)
	{
		string_type line;
		while(std::getline(stream, line))
		{
			string_type::size_type begin = line.find_first_not_of(ini_traits::SPACE);
			if(begin != string_type::npos && line.at(begin) == ini_traits::SECTION_HEADER_START)
			{
				string_type name;
				string_type::size_type end = line.find_last_of(ini_traits::SECTION_HEADER_END);
				std::copy(line.begin() + begin + 1, line.begin() + end, std::insert_iterator<string_type>(name, name.begin()));

				sections[name].read(stream);
			}
		}
	}
};

typedef basic_ini_file<char> ini_file;
typedef basic_ini_file<wchar_t> wini_file;

#ifdef UNICODE
	typedef wini_file tini_file;
#else
	typedef ini_file tini_file;
#endif

}