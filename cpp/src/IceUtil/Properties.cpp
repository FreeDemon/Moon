// **********************************************************************
//
// Copyright (c) 2003-2015 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/Properties.h>
#include <IceUtil/StringUtil.h>
#include <IceUtil/FileUtil.h>



using namespace std;


	string
IceUtil::Properties::getProperty(const string& key)
{
	IceUtil::Mutex::Lock sync(*this);

	map<string, PropertyValue>::iterator p = _properties.find(key);
	if(p != _properties.end())
	{
		p->second.used = true;
		return p->second.value;
	}
	else
	{
		return string();
	}
}

	string
IceUtil::Properties::getPropertyWithDefault(const string& key, const string& value)
{
	IceUtil::Mutex::Lock sync(*this);

	map<string, PropertyValue>::iterator p = _properties.find(key);
	if(p != _properties.end())
	{
		p->second.used = true;
		return p->second.value;
	}
	else
	{
		return value;
	}
}

	int
IceUtil::Properties::getPropertyAsInt(const string& key)
{
	return getPropertyAsIntWithDefault(key, 0);
}

	int
IceUtil::Properties::getPropertyAsIntWithDefault(const string& key, int value)
{
	IceUtil::Mutex::Lock sync(*this);

	map<string, PropertyValue>::iterator p = _properties.find(key);
	if(p != _properties.end())
	{
		int val = value;
		p->second.used = true;
		istringstream v(p->second.value);
		if(!(v >> value) || !v.eof())
		{
			//     Warning out(getProcessLogger());
			cout << "numeric property " << key << " set to non-numeric value, defaulting to " << val;
			return val;
		}
	}

	return value;
}


	std::vector<std::string>
IceUtil::Properties::getPropertyAsList(const string& key)
{
	std::vector<std::string> s;
	return getPropertyAsListWithDefault(key, s);
}

	std::vector<std::string>
IceUtil::Properties::getPropertyAsListWithDefault(const string& key, const std::vector<std::string>& value)
{
	IceUtil::Mutex::Lock sync(*this);

	map<string, PropertyValue>::iterator p = _properties.find(key);
	if(p != _properties.end())
	{
		p->second.used = true;

		std::vector<std::string> result;
		if(!IceUtilInternal::splitString(p->second.value, ", \t\r\n", result))
		{
			//  Warning out(getProcessLogger());
			cout << "mismatched quotes in property " << key << "'s value, returning default value";
		}
		if(result.size() == 0)
		{
			result = value;
		}
		return result;
	}
	else
	{
		return value;
	}
}

	void
IceUtil::Properties::setProperty(const string& key, const string& value)
{
	//
	// Trim whitespace
	//
	string currentKey = IceUtilInternal::trim(key);
	if(currentKey.empty())
	{
		cout << "Attempt to set property with empty key" << endl;;
		return;
	}

	//
	// Check if the property is legal.


	IceUtil::Mutex::Lock sync(*this);

	//
	// Set or clear the property.
	//
	if(!value.empty())
	{
		PropertyValue pv(value, false);
		map<string, PropertyValue>::const_iterator p = _properties.find(currentKey);
		if(p != _properties.end())
		{
			pv.used = p->second.used;
		}
		_properties[currentKey] = pv;
	}
	else
	{
		_properties.erase(currentKey);
	}
}


	void
IceUtil::Properties::load(const std::string& file)
{
	//
	// Metro style applications cannot access Windows registry.
	//
#if defined (_WIN32) && !defined(ICE_OS_WINRT)
	if(file.find("HKLM\\") == 0)
	{
		HKEY iceKey;
		const wstring keyName = IceUtil::stringToWstring(file, _converter).substr(5).c_str();
		LONG err;
		if((err = RegOpenKeyExW(HKEY_LOCAL_MACHINE, keyName.c_str(), 0, KEY_QUERY_VALUE, &iceKey)) != ERROR_SUCCESS)
		{
			InitializationException ex(__FILE__, __LINE__);
			ex.reason = "could not open Windows registry key `" + file + "':\n" + IceUtilInternal::errorToString(err);
			throw ex;
		}

		DWORD maxNameSize; // Size in characters not including terminating null character.
		DWORD maxDataSize; // Size in bytes
		DWORD numValues;
		try
		{
			err = RegQueryInfoKey(iceKey, NULL, NULL, NULL, NULL, NULL, NULL, &numValues, &maxNameSize, &maxDataSize,
					NULL, NULL);
			if(err != ERROR_SUCCESS)
			{
				InitializationException ex(__FILE__, __LINE__);
				ex.reason = "could not open Windows registry key `" + file + "':\n";
				ex.reason += IceUtilInternal::errorToString(err);
				throw ex;
			}

			for(DWORD i = 0; i < numValues; ++i)
			{
				vector<wchar_t> nameBuf(maxNameSize + 1);
				vector<BYTE> dataBuf(maxDataSize);
				DWORD keyType;
				DWORD nameBufSize = static_cast<DWORD>(nameBuf.size());
				DWORD dataBufSize = static_cast<DWORD>(dataBuf.size());
				err = RegEnumValueW(iceKey, i, &nameBuf[0], &nameBufSize, NULL, &keyType, &dataBuf[0], &dataBufSize);
				if(err != ERROR_SUCCESS || nameBufSize == 0)
				{
					ostringstream os;
					os << "could not read Windows registry property name, key: `" + file + "', index: " << i << ":\n";
					if(nameBufSize == 0)
					{
						os << "property name can't be the empty string";
					}
					else
					{
						os << IceUtilInternal::errorToString(err);
					}
					getProcessLogger()->warning(os.str());
					continue;
				}
				string name = IceUtil::wstringToString(
						wstring(reinterpret_cast<wchar_t*>(&nameBuf[0]), nameBufSize), _converter);
				if(keyType != REG_SZ && keyType != REG_EXPAND_SZ)
				{
					ostringstream os;
					os << "unsupported type for Windows registry property `" + name + "' key: `" + file + "'";
					getProcessLogger()->warning(os.str());
					continue;
				}

				string value;
				wstring valueW = wstring(reinterpret_cast<wchar_t*>(&dataBuf[0]), (dataBufSize / sizeof(wchar_t)) - 1);
				if(keyType == REG_SZ)
				{
					value = IceUtil::wstringToString(valueW, _converter);
				}
				else // keyType == REG_EXPAND_SZ
				{
					vector<wchar_t> expandedValue(1024);
					DWORD sz = ExpandEnvironmentStringsW(valueW.c_str(), &expandedValue[0],
							static_cast<DWORD>(expandedValue.size()));
					if(sz >= expandedValue.size())
					{
						expandedValue.resize(sz + 1);
						if(ExpandEnvironmentStringsW(valueW.c_str(), &expandedValue[0],
									static_cast<DWORD>(expandedValue.size())) == 0)
						{
							ostringstream os;
							os << "could not expand variable in property `" << name << "', key: `" + file + "':\n";
							os << IceUtilInternal::lastErrorToString();
							getProcessLogger()->warning(os.str());
							continue;
						}
					}
					value = IceUtil::wstringToString(wstring(&expandedValue[0], sz -1), _converter);
				}
				setProperty(name, value);
			}
		}
		catch(...)
		{
			RegCloseKey(iceKey);
			throw;
		}
		RegCloseKey(iceKey);
	}
	else
#endif
	{
		IceUtilInternal::ifstream in(file);
		if(!in)
		{
			cout << "Load config file error " << file << endl;
			return;
		}

		string line;
		bool firstLine = true;
		while(getline(in, line))
		{
			//
			// Skip UTF8 BOM if present.
			//
			if(firstLine)
			{
				const unsigned char UTF8_BOM[3] = {0xEF, 0xBB, 0xBF};
				if(line.size() >= 3 &&
						static_cast<const unsigned char>(line[0]) == UTF8_BOM[0] &&
						static_cast<const unsigned char>(line[1]) == UTF8_BOM[1] &&
						static_cast<const unsigned char>(line[2]) == UTF8_BOM[2])
				{
					line = line.substr(3);
				}
				firstLine = false;
			}
			parseLine(line, _converter);
		}
	}
}


IceUtil::Properties::Properties() :
	_converter(IceUtil::getProcessStringConverter())
{
}

	void
IceUtil::Properties::parseLine(const string& line, const IceUtil::StringConverterPtr& converter)
{
	string key;
	string value;

	enum ParseState { Key , Value };
	ParseState state = Key;

	string whitespace;
	string escapedspace;
	bool finished = false;
	for(string::size_type i = 0; i < line.size(); ++i)
	{
		char c = line[i];
		switch(state)
		{
			case Key:
				{
					switch(c)
					{
						case '\\':
							if(i < line.length() - 1)
							{
								c = line[++i];
								switch(c)
								{
									case '\\':
									case '#':
									case '=':
										key += whitespace;
										whitespace.clear();
										key += c;
										break;

									case ' ':
										if(key.length() != 0)
										{
											whitespace += c;
										}
										break;

									default:
										key += whitespace;
										whitespace.clear();
										key += '\\';
										key += c;
										break;
								}
							}
							else
							{
								key += whitespace;
								key += c;
							}
							break;

						case ' ':
						case '\t':
						case '\r':
						case '\n':
							if(key.length() != 0)
							{
								whitespace += c;
							}
							break;

						case '=':
							whitespace.clear();
							state = Value;
							break;

						case '#':
							finished = true;
							break;

						default:
							key += whitespace;
							whitespace.clear();
							key += c;
							break;
					}
					break;
				}

			case Value:
				{
					switch(c)
					{
						case '\\':
							if(i < line.length() - 1)
							{
								c = line[++i];
								switch(c)
								{
									case '\\':
									case '#':
									case '=':
										value += value.length() == 0 ? escapedspace : whitespace;
										whitespace.clear();
										escapedspace.clear();
										value += c;
										break;

									case ' ':
										whitespace += c;
										escapedspace += c;
										break;

									default:
										value += value.length() == 0 ? escapedspace : whitespace;
										whitespace.clear();
										escapedspace.clear();
										value += '\\';
										value += c;
										break;
								}
							}
							else
							{
								value += value.length() == 0 ? escapedspace : whitespace;
								value += c;
							}
							break;

						case ' ':
						case '\t':
						case '\r':
						case '\n':
							if(value.length() != 0)
							{
								whitespace += c;
							}
							break;

						case '#':
							finished = true;
							break;

						default:
							value += value.length() == 0 ? escapedspace : whitespace;
							whitespace.clear();
							escapedspace.clear();
							value += c;
							break;
					}
					break;
				}
		}
		if(finished)
		{
			break;
		}
	}
	value += escapedspace;

	if((state == Key && key.length() != 0) || (state == Value && key.length() == 0))
	{
		//getProcessLogger()->warning("invalid config file entry: \"" + line + "\"");
		return;
	}
	else if(key.length() == 0)
	{
		return;
	}

	key = IceUtil::UTF8ToNative(key, converter);
	value = IceUtil::UTF8ToNative(value, converter);

	setProperty(key, value);
}

	void
IceUtil::Properties::loadConfig()
{
	string value = getProperty("Moon.cfg");
#ifndef ICE_OS_WINRT
	//
	// WinRT cannot access environment variables
	//
	if(value.empty() || value == "1")
	{
#   ifdef _WIN32
		vector<wchar_t> v(256);
		DWORD ret = GetEnvironmentVariableW(L"MOON_CONFIG", &v[0], static_cast<DWORD>(v.size()));
		if(ret >= v.size())
		{
			v.resize(ret + 1);
			ret = GetEnvironmentVariableW(L"MOON_CONFIG", &v[0], static_cast<DWORD>(v.size()));
		}
		if(ret > 0)
		{
			value = IceUtil::wstringToString(wstring(&v[0], ret), _converter);
		}
		else
		{
			value = "";
		}
#   else
		const char* s = getenv("MOON_CONFIG");
		if(s && *s != '\0')
		{
			value = s;
		}
#   endif
	}
#endif

	if(!value.empty())
	{
		vector<string> files;
		IceUtilInternal::splitString(value, ",", files);
		for(vector<string>::const_iterator i = files.begin(); i != files.end(); ++i)
		{
			load(IceUtilInternal::trim(*i));
		}

		PropertyValue pv(value, true);
		_properties["Moon.cfg"] = pv;
	}
}
