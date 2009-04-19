/**************************************************************************
    Lighspark, a free flash player implementation

    Copyright (C) 2009  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#ifndef ASOBJECTS_H
#define ASOBJECTS_H
#include "swftypes.h"

class Stage: public ISWFObject_impl
{
private:
	Integer width;
	Integer height;
public:
	Stage();
	SWFOBJECT_TYPE getObjectType() { return T_OBJECT; }
};

class Array: public ISWFObject_impl
{
public:
	static void constructor(Array* th, arguments* args);
	SWFOBJECT_TYPE getObjectType() { return T_OBJECT; }
	void _register();
	ISWFObject* clone()
	{
		return new Array(*this);
	}
};

#endif
