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


public:

	// Internal storage
	ClientAttributeType                      type;
	std::string                              s;
	int                                      i;
	unsigned int                             u;
	double                                   d;
	bool                                     b;

};

// ClientAttribute operator ==
inline bool operator== (const ClientAttribute& lhs, const ClientAttribute& rhs)
{
	if (lhs.type == rhs.type)
	{
		switch (lhs.type)
		{
			case ClientAttributeType::T_STR: return (lhs.s == rhs.s);
			case ClientAttributeType::T_INT: return (lhs.i == rhs.i);
			case ClientAttributeType::T_UNS: return (lhs.u == rhs.u);
			case ClientAttributeType::T_DBL: return (lhs.d == rhs.d);
			case ClientAttributeType::T_BOL: return (lhs.b == rhs.b);
		}
	}
	return false;
}

// ClientAttribute operator <
inline bool operator< (const ClientAttribute& lhs, const ClientAttribute& rhs)
{
	if (lhs.type == rhs.type)
	{
		switch (lhs.type)
		{
			case ClientAttributeType::T_STR: return (lhs.s < rhs.s);
			case ClientAttributeType::T_INT: return (lhs.i < rhs.i);
			case ClientAttributeType::T_UNS: return (lhs.u < rhs.u);
			case ClientAttributeType::T_DBL: return (lhs.d < rhs.d);
			case ClientAttributeType::T_BOL: return (lhs.b < rhs.b);
		}
	}
	return false;
}

// Trivial ClientAttribute operators
inline bool operator!= (const ClientAttribute& lhs, const ClientAttribute& rhs){ return !(rhs == lhs);}
inline bool operator>  (const ClientAttribute& lhs, const ClientAttribute& rhs){ return   rhs <  lhs;}
inline bool operator<= (const ClientAttribute& lhs, const ClientAttribute& rhs){ return !(rhs <  lhs);}
inline bool operator>= (const ClientAttribute& lhs, const ClientAttribute& rhs){ return !(lhs <  rhs);}

