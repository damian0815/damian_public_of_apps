/*
 *  SharedData.h
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <map>
using namespace std;

class SharedData
{
public:
	
	static float getFloat( string name ) { return float_vars[name]; }
	static string getString( string name ) { return string_vars[name]; }

	static void setFloat( string name, float f ) { float_vars[name] = f; }
	static void setString( string name, string s ) { string_vars[name] = s; }


private:
	
	static map<string, float > float_vars;
	static map<string, string> string_vars;





};