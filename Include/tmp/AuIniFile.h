// IniFile.cpp:  Implementation of the CIniFile class.
// Written by:   Adam Clauss
// Email: cabadam@houston.rr.com
// You may use this class/code as you wish in your programs.  Feel free to distribute it, and
// email suggested changes to me.
//
// Rewritten by: Shane Hill
// Date:         21/08/2001
// Email:        Shane.Hill@dsto.defence.gov.au
// Reason:       Remove dependancy on MFC. Code should compile on any
//               platform. Tested on Windows/Linux/Irix
//////////////////////////////////////////////////////////////////////

// modified by netong (2002. 04. 16)
//////////////////////////////////////////////////////////////////////

#ifndef __AUINIFILE_H__
#define __AUINIFILE_H__

#pragma warning( push )
#pragma warning( disable:4786 )

// C++ Includes
#include <string>
#include <vector>

// C Includes
#include <stdlib.h>


#ifdef	_DEBUG
#pragma comment ( lib , "AuIniFileD" )
#else
#pragma comment ( lib , "AuIniFile" )
#endif


#define MAX_KEYNAME    128
#define MAX_VALUENAME  128
#define MAX_VALUEDATA 2048


class AuIniFile  
{
private:
	bool   caseInsensitive;
	std::string path;
	struct key {
		std::vector<std::string> names;
		std::vector<std::string> values; 
		std::vector<std::string> comments;
	};
	std::vector<key>    keys; 
	std::vector<std::string> names; 
	std::vector<std::string> comments;
	std::string CheckCase( std::string s) const;
	
	public:
		enum errors{ noID = -1};
		AuIniFile( std::string const iniPath = "");
		virtual ~AuIniFile()                            {}
		
		// Sets whether or not keynames and valuenames should be case sensitive.
		// The default is case insensitive.
		void CaseSensitive()                           {caseInsensitive = false;}
		void CaseInsensitive()                         {caseInsensitive = true;}
		
		// Sets path of ini file to read and write from.
		void Path(std::string const newPath)                {path = newPath;}
		std::string Path() const                            {return path;}
		void SetPath(std::string const newPath)             {Path( newPath);}
		
		// Reads ini file specified using path.
		// Returns true if successful, false otherwise.
		bool ReadFile();
		
		// Writes data stored in class to ini file.
		bool WriteFile(); 
		
		// Deletes all stored ini data.
		void Erase();
		void Clear()                                   {Erase();}
		void Reset()                                   {Erase();}
		
		// Returns index of specified key, or noID if not found.
		long FindKey( std::string const keyname) const;
		
		// Returns index of specified value, in the specified key, or noID if not found.
		long FindValue( unsigned const keyID, std::string const valuename) const;
		
		// Returns number of keys currently in the ini.
		unsigned NumKeys() const                       {return names.size();}
		unsigned GetNumKeys() const                    {return NumKeys();}
		
		// Add a key name.
		unsigned AddKeyName( std::string const keyname);
		
		// Returns key names by index.
		std::string KeyName( unsigned const keyID) const;
		std::string GetKeyName( unsigned const keyID) const {return KeyName(keyID);}
		
		// Returns number of values stored for specified key.
		unsigned NumValues( unsigned const keyID);
		unsigned GetNumValues( unsigned const keyID)   {return NumValues( keyID);}
		unsigned NumValues( std::string const keyname);
		unsigned GetNumValues( std::string const keyname)   {return NumValues( keyname);}
		
		// Returns value name by index for a given keyname or keyID.
		std::string ValueName( unsigned const keyID, unsigned const valueID) const;
		std::string GetValueName( unsigned const keyID, unsigned const valueID) const {
			return ValueName( keyID, valueID);
		}
		std::string ValueName( std::string const keyname, unsigned const valueID) const;
		std::string GetValueName( std::string const keyname, unsigned const valueID) const {
			return ValueName( keyname, valueID);
		}
		
		// Gets value of [keyname] valuename =.
		// Overloaded to return string, int, and double.
		// Returns defValue if key/value not found.
		std::string GetValue( unsigned const keyID, unsigned const valueID, std::string const defValue = "") const;
		std::string GetValue(std::string const keyname, std::string const valuename, std::string const defValue = "") const; 
		int    GetValueI(std::string const keyname, std::string const valuename, int const defValue = 0) const;
		bool   GetValueB(std::string const keyname, std::string const valuename, bool const defValue = false) const {
			return ( GetValueI( keyname, valuename, int( defValue)) != 0);
		}
		double   GetValueF(std::string const keyname, std::string const valuename, double const defValue = 0.0) const;
		// This is a variable length formatted GetValue routine. All these voids
		// are required because there is no vsscanf() like there is a vsprintf().
		// Only a maximum of 8 variable can be read.
		unsigned GetValueV( std::string const keyname, std::string const valuename, char *format,
			void *v1 = 0, void *v2 = 0, void *v3 = 0, void *v4 = 0,
			void *v5 = 0, void *v6 = 0, void *v7 = 0, void *v8 = 0,
			void *v9 = 0, void *v10 = 0, void *v11 = 0, void *v12 = 0,
			void *v13 = 0, void *v14 = 0, void *v15 = 0, void *v16 = 0);
		
		// Sets value of [keyname] valuename =.
		// Specify the optional paramter as false (0) if you do not want it to create
		// the key if it doesn't exist. Returns true if data entered, false otherwise.
		// Overloaded to accept string, int, and double.
		bool SetValue( unsigned const keyID, unsigned const valueID, std::string const value);
		bool SetValue( std::string const keyname, std::string const valuename, std::string const value, bool const create = true);
		bool SetValueI( std::string const keyname, std::string const valuename, int const value, bool const create = true);
		bool SetValueB( std::string const keyname, std::string const valuename, bool const value, bool const create = true) {
			return SetValueI( keyname, valuename, int(value), create);
		}
		bool SetValueF( std::string const keyname, std::string const valuename, double const value, bool const create = true);
		bool SetValueV( std::string const keyname, std::string const valuename, char *format, ...);
		
		// Deletes specified value.
		// Returns true if value existed and deleted, false otherwise.
		bool DeleteValue( std::string const keyname, std::string const valuename);
		
		// Deletes specified key and all values contained within.
		// Returns true if key existed and deleted, false otherwise.
		bool DeleteKey(std::string keyname);
		
		// Header comment functions.
		// Header comments are those comments before the first key.
		//
		// Number of header comments.
		unsigned NumHeaderComments()                  {return comments.size();}
		// Add a header comment.
		void     HeaderComment( std::string const comment);
		// Return a header comment.
		std::string   HeaderComment( unsigned const commentID) const;
		// Delete a header comment.
		bool     DeleteHeaderComment( unsigned commentID);
		// Delete all header comments.
		void     DeleteHeaderComments()               {comments.clear();}
		
		// Key comment functions.
		// Key comments are those comments within a key. Any comments
		// defined within value names will be added to this list. Therefore,
		// these comments will be moved to the top of the key definition when
		// the CIniFile::WriteFile() is called.
		//
		// Number of key comments.
		unsigned NumKeyComments( unsigned const keyID) const;
		unsigned NumKeyComments( std::string const keyname) const;
		// Add a key comment.
		bool     KeyComment( unsigned const keyID, std::string const comment);
		bool     KeyComment( std::string const keyname, std::string const comment);
		// Return a key comment.
		std::string   KeyComment( unsigned const keyID, unsigned const commentID) const;
		std::string   KeyComment( std::string const keyname, unsigned const commentID) const;
		// Delete a key comment.
		bool     DeleteKeyComment( unsigned const keyID, unsigned const commentID);
		bool     DeleteKeyComment( std::string const keyname, unsigned const commentID);
		// Delete all comments for a key.
		bool     DeleteKeyComments( unsigned const keyID);
		bool     DeleteKeyComments( std::string const keyname);
};
#pragma warning( pop )

#endif //__AUINIFILE_H__
