#include <assert.h>
#include <sstream>
#include "stdstring.h"
using namespace std;

static string StringFactory(asUINT length, const char *s)
{
	return string(s);
}

static void ConstructString(string *thisPointer)
{
	new(thisPointer) string();
}

static void DestructString(string *thisPointer)
{
	thisPointer->~string();
}

static string &AssignBitsToString(unsigned int i, string &dest)
{
	ostringstream stream;
	stream << hex << i;
	dest = stream.str(); 
	return dest;
}

static string &AddAssignBitsToString(unsigned int i, string &dest)
{
	ostringstream stream;
	stream << hex << i;
	dest += stream.str(); 
	return dest;
}

static string AddStringBits(string &str, unsigned int i)
{
	ostringstream stream;
	stream << hex << i;
	str += stream.str(); 
	return str;
}

static string AddBitsString(unsigned int i, string &str)
{
	ostringstream stream;
	stream << hex << i;
	return stream.str() + str;
}

static string &AssignUIntToString(unsigned int i, string &dest)
{
	ostringstream stream;
	stream << i;
	dest = stream.str(); 
	return dest;
}

static string &AddAssignUIntToString(unsigned int i, string &dest)
{
	ostringstream stream;
	stream << i;
	dest += stream.str(); 
	return dest;
}

static string AddStringUInt(string &str, unsigned int i)
{
	ostringstream stream;
	stream << i;
	str += stream.str(); 
	return str;
}

static string AddIntString(int i, string &str)
{
	ostringstream stream;
	stream << i;
	return stream.str() + str;
}

static string &AssignIntToString(int i, string &dest)
{
	ostringstream stream;
	stream << i;
	dest = stream.str(); 
	return dest;
}

static string &AddAssignIntToString(int i, string &dest)
{
	ostringstream stream;
	stream << i;
	dest += stream.str(); 
	return dest;
}

static string AddStringInt(string &str, int i)
{
	ostringstream stream;
	stream << i;
	str += stream.str(); 
	return str;
}

static string AddUIntString(unsigned int i, string &str)
{
	ostringstream stream;
	stream << i;
	return stream.str() + str;
}

static string &AssignDoubleToString(double f, string &dest)
{
	ostringstream stream;
	stream << f;
	dest = stream.str(); 
	return dest;
}

static string &AddAssignDoubleToString(double f, string &dest)
{
	ostringstream stream;
	stream << f;
	dest += stream.str(); 
	return dest;
}

static string AddStringDouble(string &str, double f)
{
	ostringstream stream;
	stream << f;
	str += stream.str(); 
	return str;
}

static string AddDoubleString(double f, string &str)
{
	ostringstream stream;
	stream << f;
	return stream.str() + str;
}

//@{ Jaewon 20050218
static string &AssignFloatToString(float f, string &dest)
{
	ostringstream stream;
	stream << f;
	dest = stream.str(); 
	return dest;
}

static string &AddAssignFloatToString(float f, string &dest)
{
	ostringstream stream;
	stream << f;
	dest += stream.str(); 
	return dest;
}

static string AddStringFloat(string &str, float f)
{
	ostringstream stream;
	stream << f;
	str += stream.str(); 
	return str;
}

static string AddFloatString(float f, string &str)
{
	ostringstream stream;
	stream << f;
	return stream.str() + str;
}
//@} Jaewon

void RegisterStdString(asIScriptEngine *engine)
{
	int r;

	// Register the bstr type
	r = engine->RegisterObjectType("string", sizeof(string), asOBJ_CLASS_CDA); assert( r >= 0 );

	// Register the bstr factory
	r = engine->RegisterStringFactory("string", asFUNCTION(StringFactory), asCALL_CDECL); assert( r >= 0 );

	// Register the object operator overloads
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f()",              asFUNCTION(ConstructString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT,   "void f()",              asFUNCTION(DestructString),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(string &in)", asMETHODPR(string, operator =, (const string&), string&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(string &in)", asMETHODPR(string, operator+=, (const string&), string&), asCALL_THISCALL); assert( r >= 0 );

	// Register the global operator overloads
	r = engine->RegisterGlobalBehaviour(asBEHAVE_EQUAL,       "bool f(string &in, string &in)", asFUNCTIONPR(operator==, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_NOTEQUAL,    "bool f(string &in, string &in)", asFUNCTIONPR(operator!=, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_LEQUAL,      "bool f(string &in, string &in)", asFUNCTIONPR(operator<=, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_GEQUAL,      "bool f(string &in, string &in)", asFUNCTIONPR(operator>=, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_LESSTHAN,    "bool f(string &in, string &in)", asFUNCTIONPR(operator <, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_GREATERTHAN, "bool f(string &in, string &in)", asFUNCTIONPR(operator >, (const string &, const string &), bool), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(string &in, string &in)", asFUNCTIONPR(operator +, (const string &, const string &), string), asCALL_CDECL); assert( r >= 0 );

	// Register the object methods
	r = engine->RegisterObjectMethod("string", "uint length()", asMETHOD(string,size), asCALL_THISCALL); assert( r >= 0 );

	// Automatic conversion from values
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(double)", asFUNCTION(AssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(double)", asFUNCTION(AddAssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(string &in, double)", asFUNCTION(AddStringDouble), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(double, string &in)", asFUNCTION(AddDoubleString), asCALL_CDECL); assert( r >= 0 );

	//@{ Jaewon 20050218
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(float)", asFUNCTION(AssignFloatToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(float)", asFUNCTION(AddAssignFloatToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(string &in, float)", asFUNCTION(AddStringFloat), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(float, string &in)", asFUNCTION(AddFloatString), asCALL_CDECL); assert( r >= 0 );
	//@} Jaewon

	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(int)", asFUNCTION(AssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(int)", asFUNCTION(AddAssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(string &in, int)", asFUNCTION(AddStringInt), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(int, string &in)", asFUNCTION(AddIntString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(uint)", asFUNCTION(AssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(uint)", asFUNCTION(AddAssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(string &in, uint)", asFUNCTION(AddStringUInt), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(uint, string &in)", asFUNCTION(AddUIntString), asCALL_CDECL); assert( r >= 0 );

	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ASSIGNMENT, "string &f(bits)", asFUNCTION(AssignBitsToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_ADD_ASSIGN, "string &f(bits)", asFUNCTION(AddAssignBitsToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(string &in, bits)", asFUNCTION(AddStringBits), asCALL_CDECL); assert( r >= 0 );
	r = engine->RegisterGlobalBehaviour(asBEHAVE_ADD,         "string f(bits, string &in)", asFUNCTION(AddBitsString), asCALL_CDECL); assert( r >= 0 );
}


