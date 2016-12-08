#pragma once

#include <string>


/*-----------------------------------------------------------------------------
	Data types
-----------------------------------------------------------------------------*/

// Type of attribute
enum class ClientAttributeType {T_NONE = 0, T_STR, T_INT, T_UNS, T_DBL, T_BOL};


// Client attribute value
class ClientAttribute
{
public:

	// Constructors
	ClientAttribute()               : type(ClientAttributeType::T_NONE)      {}
	ClientAttribute(std::string  v) : type(ClientAttributeType::T_STR), s(v) {}
	ClientAttribute(int          v) : type(ClientAttributeType::T_INT), i(v) {}
	ClientAttribute(unsigned int v) : type(ClientAttributeType::T_UNS), u(v) {}
	ClientAttribute(double       v) : type(ClientAttributeType::T_DBL), d(v) {}
	ClientAttribute(bool         v) : type(ClientAttributeType::T_BOL), b(v) {}

	// Operator ==
	inline bool operator==(const ClientAttribute& o) const
	{
		if (type == o.type)
		{
			switch (type)
			{
				case ClientAttributeType::T_STR: return (s == o.s);
				case ClientAttributeType::T_INT: return (i == o.i);
				case ClientAttributeType::T_UNS: return (u == o.u);
				case ClientAttributeType::T_DBL: return (d == o.d);
				case ClientAttributeType::T_BOL: return (b == o.b);
			}
		}
		return false;
	}
	
	// Operator <
	inline bool operator< (const ClientAttribute& o)
	{
		if (type == o.type)
		{
			switch (type)
			{
				case ClientAttributeType::T_STR: return (s < o.s);
				case ClientAttributeType::T_INT: return (i < o.i);
				case ClientAttributeType::T_UNS: return (u < o.u);
				case ClientAttributeType::T_DBL: return (d < o.d);
				case ClientAttributeType::T_BOL: return (b < o.b);
			}
		}
		return false;
	}

	// Trivial operators
	inline bool operator> (const ClientAttribute& o) { return   *this < o; }
	inline bool operator<=(const ClientAttribute& o) { return !(*this > o); }
	inline bool operator>=(const ClientAttribute& o) { return !(*this < o); }


public:

	// Internal storage
	ClientAttributeType                      type;
	std::string                              s;
	int                                      i;
	unsigned int                             u;
	double                                   d;
	bool                                     b;

};


// Map of client attributes
class ClientData
{
public:

	ClientData()
	{}

	ClientData(std::string id)
		: privateId(id)
	{}

public:

	std::string                              privateId;
	std::map<std::string, ClientAttribute>   attributes;

};


// Search result for a value
using ClientSearchResult = std::map<std::string, ClientAttribute>;
