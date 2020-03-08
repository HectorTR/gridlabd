/** json.cpp
	Copyright (C) 2018 Regents of the Leland Stanford Junior University 

	@file json.c
	@addtogroup mapping JSON formatting
	@ingroup core

	JSON files may be extracted using using the
	\p --output file.json command line option.  The function
	json_dump() is called and when a JSON dump
	is performed, the simulation is not run.

 @{
 **/

#include "gldcore.h"

SET_MYCONTEXT(DMC_SAVE)

/////////////////////////////////////////////////////
// JSON Parser
/////////////////////////////////////////////////////

int json_white(const char *input)
{
	int n = 0;
	while ( isspace(input[n]) )
	{
		n++;
	}
	return n;

}
int json_null(const char *input, GldJson &data)
{
	int n = json_white(input);
	if ( strncmp(input+n,"null",4) == 0 )
	{
		data = GldJson(JSON_NULL);
		return n+4;
	}
	else
	{
		return n;
	}
}

int json_false(const char *input, GldJson &data)
{
	int n = json_white(input);
	if ( strncmp(input+n,"false",5) == 0 )
	{
		data = GldJson(JSON_FALSE);
		return n+5;
	}
	else
	{
		return n;
	}
}

int json_true(const char *input, GldJson &data)
{
	int n = json_white(input);
	if ( strncmp(input+n,"true",4) == 0 )
	{
		data = GldJson(JSON_TRUE);
		return n+4;
	}
	else
	{
		return n;
	}
}

int json_number(const char *input, GldJson &data)
{
	int n = json_white(input);
	double value;
	int len = sscanf(input+n,"%lg",&value);
	if ( len == 0 )
		return 0;
	else
		n += len;
	GldJson number(JSON_NUMBER);
	number.append(value)
	return 0;
}

int json_string(const char *input, GldJson &data)
{
	CString str("");
	int n = json_white(input);
	if ( input[n] != '"' )
		return 0;
	else
		n++;
	while ( input[n] != '"' )
	{
		switch (input[n]) {
		case '\\':
			n++;
			switch (input[n]) {
			case '\\':
				str += '\\';
				break;
			case '/':
				str += '/';
				break;
			case 'b':
				str += '\b';
				break;
			case 'f':
				str += '\f';
				break;
			case 'n':
				str += '\n';
				break;
			case 'r':
				str += '\r';
				break;
			case 't':
				str += '\t';
				break;
			case 'u':
				str
			}
			n++;
		default:
			str += input[n];
			break;
		}
		n++;
	}
	if ( input[n] != '"' )
		return 0;
	else
		n++;
	// TODO
	return n;
}

int json_value(const char *input, GldJson &data)
{
	int n = json_white(input);
	if ( (len=json_object(input+n,data)) > 0 
		|| (len=json_array(input+n,data)) > 0
		|| (len=json_number(input+n,data)) > 0
		|| (len=json_string(input+n,data)) > 0
		|| (len=json_true(input+n,data)) > 0
		|| (len=json_false(input+n,data)) > 0
		|| (len=json_null(input+n,data)) > 0 )
	{
		return len+n;
	}
	else
	{
		return n;
	}
}

int json_array(const char *input, GldJson &data)
{
	int n = json_white(input);
	if ( input[n] != '[' )
		return 0;
	n++;
	GldJson array(GldJson::JSON_ARRAY);
	do {
		GldJson val(GldJson::JSON_VALUE), arr(GldJson::JSON_ARRAY);
		n += json_white(input+n);
		int len = json_value(input+n,val);
		if ( len == 0 )
			return 0;
		else
			n += len;
		n += json_white(input+n);
	} while ( input[n] != ',' );
	n += json_white(input+n);
	if ( input[n] != ']' )
		return 0;
	n++;
	data = array;
	return 0;
}

int json_object(const char *input, GldJson &data)
{
	int n = json_white(input);
	if ( input[n] != '{' )
		return 0;
	n++;
	GldJson list(GldJson::JSON_OBJECT);
	do {
		GldJson str(GldJson::JSON_STRING), val(GldJson::JSON_VALUE), obj(GldJson::JSON_OBJECT);
		n += json_white(input+n);
		int len = json_string(input+n,str);
		if ( len == 0 )
			return 0;
		else
			n += len;
		n += json_white(input+n);
		len = json_value(input+n,val);
		if ( len == 0 )
			return 0;
		else 
			n += len;
		n += json_white(input+n);
		obj.append(str);
		obj.append(val);
		list.append(obj);
	} while ( input[n] == ',');
	n += json_white(input+n);
	if ( input[n] != '}' )
		return 0;
	n++;
	data = list;
	return n;
}

DEPRECATED int json_dump(const char *filename)
{
	GldJsonWriter json(filename);
	return json.dump();
}

DEPRECATED CDECL int convert_to_json(const char *s, void *data, PROPERTY *p)
{
	// TODO: implement this
	return 0;
}
DEPRECATED CDECL int convert_from_json(char *buffer, int len, void *data, PROPERTY *p)
{
	// TODO: implement this
	return 0;
}
DEPRECATED CDECL int json_create(void *ptr)
{
	// TODO: implement this
	return 0;
}

DEPRECATED CDECL double json_get_part(void *c, const char *name)
{
	return QNAN;
}
DEPRECATED CDECL int json_set_part(void *c, const char *name, const char *value)
{
	return 0;
}

void GldJson::set_data(void) 
{ 
	switch( type ) {
	case JSON_STRING:
		free(data.str.buf);
		data.str.buf = NULL;
		data.str.len = 0;
		break;
	case JSON_OBJECT:
		delete *obj;
		data.obj = NULL;
		break;
	default:
		break;
	}
}

void GldJson::set_data(double x) 
{ 
	if ( type != JSON_NUMBER ) 
	{ 
		set_data(); 
		type=JSON_NUMBER;
	} 
	data.num = x;
}

void GldJson::set_data(const char *x, size_t sz = 0) 
{
	if ( type != JSON_STRING )
	{
		set_data();
		type = JSON_STRING;
	}
	else
	{
		if ( sz == 0 )
			sz = strlen(x);
		if ( sz > data.str.len )
		{
			data.str.buf = realloc(sz+1);
			data.str.len = sz;
		}
	}
	strcpy(data.str.buf,x);
}

void GldJson::set_data(GldJson *x) 
{
	if ( type != x->get_type() )
	{
		set_data();
		type = x->get_type();
	}
	data.val = x;
}

unsigned int GldJsonWriter::version = 0;

GldJsonWriter::GldJsonWriter(const char *f)
{
	filename = strdup(f);
}

GldJsonWriter::~GldJsonWriter(void)
{
	free((void*)filename);
}

const char * escape(const char *buffer, size_t len = 1024)
{
	static char *result = NULL;
	static size_t result_len = 0;
	if ( len >= result_len*5+1 )
	{
		result_len = (len>1024?len:1024)*5+1;
		result = (char*)realloc(result,result_len);
		if ( result == NULL )
			throw_exception("escape(const char *buffer=%p, size_t len=%llu): memory allocation failed",buffer,len);
	}
	char *p = result;
	const char *c;
	for ( c = buffer ; *c != '\0' && c < buffer+len ; c++)
	{
		switch ( *c )
		{
		case '"':
			*p++ = '\\';
			*p++ = '"';
			break;
		case '\\':
			*p++ = '\\';
			*p++ = '\\';
			break;
		// DPC: solidus is in the JSON spec (http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf, Figure 5)
		// but it's not desirable or necessary (so far as I can tell) to escape it for gridlabd strings
		// case '/':
		// 	*p++ = '\\';
		// 	*p++ = '/';
		// 	break;
		case '\b':
			*p++ = '\\';
			*p++ = 'b';
			break;
		case '\f':
			*p++ = '\\';
			*p++ = 'f';
			break;
		case '\n':
			*p++ = '\\';
			*p++ = 'n';
			break;
		case '\r':
			*p++ = '\\';
			*p++ = 'r';
			break;
		case '\t':
			*p++ = '\\';
			*p++ = 't';
			break;
		default:
			if ( *c >= 32 && *c < 127 )
			{
				*p++ = *c;
			}
			else
			{
				p += sprintf(p,"\\u%04hX", (unsigned short)*c);
			}
			break;
		}
	}
	*p = '\0';
	return result;
}

int GldJsonWriter::write(const char *fmt,...)
{
	int len;
	va_list ptr;
	va_start(ptr,fmt);
	len = vfprintf(json,fmt,ptr);
	va_end(ptr);
	return len;
}

#define FIRST(N,F,V) (len += write("\n\t\t\t\"%s\" : \"" F "\"",N,V))
#define TUPLE(N,F,V) (len += write(",\n\t\t\t\"%s\" : \"" F "\"",N,V))

int GldJsonWriter::write_modules(FILE *fp)
{
	int len = 0;
	MODULE *mod;
	len += write(",\n\t\"modules\" : {");
	for ( mod = module_get_first() ; mod != NULL ; mod = mod->next )
	{
		if ( mod != module_get_first() )
			len += write(",");
		len += write("\n\t\t\"%s\" : {",mod->name);
		FIRST("major","%d",mod->major);
		TUPLE("minor","%d",mod->minor);
		// TODO more info
		len += write("\n\t\t}");
	}
	len += write("\n\t}");
	IN_MYCONTEXT output_debug("GldJsonWriter::modules() wrote %d bytes",len);
	return len;
}

int GldJsonWriter::write_properties(FILE *fp)
{
	int len = 0;
	PROPERTYTYPE ptype;
	len += write(",\n\t\"types\" : {");
	for ( ptype = property_getfirst_type() ; ptype != PT_void ; ptype = property_getnext_type(ptype) )
	{
		PROPERTYSPEC *spec = property_getspec(ptype);
		PROPERTYCOMPAREOP op;
		bool first = true;
		if ( ptype != property_getfirst_type() )
			len += write(",");
		len += write("\n\t\t\"%s\" : {",spec->name);
		len += write("\n\t\t\t\"xsdtype\" : \"%s\"",spec->xsdname);
		if ( spec->default_value != NULL )
			len += write(",\n\t\t\t\"default\" : \"%s\"",spec->default_value);
		if ( spec->size > 0 )
			len += write(",\n\t\t\t\"memsize\" : \"%d\"",spec->size);
		if ( spec->csize > 0 )
		{
			len += write(",\n\t\t\t\"strsize\" : \"%d\"",spec->csize);
			len += write(",\n\t\t\t\"has_tostr\" : \"%s\"",spec->data_to_string?"True":"False");
			len += write(",\n\t\t\t\"has_fromstr\" : \"%s\"",spec->string_to_data?"True":"False");
		}
		len += write(",\n\t\t\t\"has_create\" : \"%s\"",spec->create?"True":"False");
		len += write(",\n\t\t\t\"has_stream\" : \"%s\"",spec->stream?"True":"False");
		len += write(",\n\t\t\t\"compareops\" : {");
		for ( op = _TCOP_FIRST ; op < _TCOP_LAST ; op = PROPERTYCOMPAREOP(op+1) )
		{
			if ( spec->compare[op].fn != NULL )
			{
				if ( ! first )
					len += write(",");
				else
					first = false;
				len += write("\n\t\t\t\t\"%s\" : {\"nargs\": \"%d\"}",spec->compare[op].str, spec->compare[op].trinary ? 3 : 2);
			}
		}
		len += write("\n\t\t\t}");
		len += write("\n\t\t}");
	}
	len += write("\n\t}");
	IN_MYCONTEXT output_debug("GldJsonWriter::properties() wrote %d bytes",len);
	return len;
}


int GldJsonWriter::write_classes(FILE *fp)
{
	int len = 0;
	CLASS *oclass;
	len += write(",\n\t\"classes\" : {");
	for ( oclass = class_get_first_class() ; oclass != NULL ; oclass = oclass->next )
	{
		PROPERTY *prop;
		if ( oclass != class_get_first_class() )
			len += write(",");
		len += write("\n\t\t\"%s\" : {",oclass->name);
		FIRST("object_size","%u",oclass->size);
		if ( oclass->parent )
		{
			TUPLE("parent","%s",oclass->parent->name );
		}
		TUPLE("trl","%u",oclass->trl);
		TUPLE("profiler.numobjs","%u",oclass->profiler.numobjs);
		TUPLE("profiler.clocks","%llu",oclass->profiler.clocks);
		TUPLE("profiler.count","%u",oclass->profiler.count);
		if ( oclass->has_runtime ) TUPLE("runtime","%s",oclass->runtime);
		if ( oclass->pmap != NULL )
			len += write(",");
		for ( prop = oclass->pmap ; prop != NULL && prop->oclass == oclass ; prop=prop->next ) // note: do not output parent classes properties
		{
			KEYWORD *key;
			const char *ptype = class_get_property_typename(prop->ptype);
			if ( ptype == NULL )
				continue;
			if ( prop != oclass->pmap )
				len += write(",");
			len += write("\n\t\t\t\"%s\" : {",prop->name);
			len += write("\n\t\t\t\t\"type\" : \"%s\",",ptype);
			char access[1024] = "";
			switch ( prop->access ) {
			case PA_PUBLIC: strcpy(access,"PUBLIC"); break;
			case PA_REFERENCE: strcpy(access,"REFERENCE"); break;
			case PA_PROTECTED: strcpy(access,"PROTECTED"); break;
			case PA_PRIVATE: strcpy(access,"PRIVATE"); break;
			case PA_HIDDEN: strcpy(access,"HIDDEN"); break;
			case PA_N: strcpy(access,"NONE"); break;
			default:
				if ( prop->access & PA_R ) strcat(access,"R");
				if ( prop->access & PA_W ) strcat(access,"W");
				if ( prop->access & PA_S ) strcat(access,"S");
				if ( prop->access & PA_L ) strcat(access,"L");
				if ( prop->access & PA_H ) strcat(access,"H");
				break;
			}
			len += write("\n\t\t\t\t\"access\" : \"%s\"",access);
			for ( key = prop->keywords ; key != NULL ; key = key->next )
			{
				if ( key == prop->keywords )
				{
					len += write(",\n\t\t\t\t\"keywords\" : {");
				}
				len += write("\n\t\t\t\t\t\"%s\" : \"0x%x\"",key->name,key->value);
				if ( key->next == NULL )
					len += write("\n\t\t\t\t}");
				else
					len += write(",");
			}
			char flags[1024] = "";
			if ( prop->flags&PF_RECALC ) strcat(flags,"RECALC");
			if ( prop->flags&PF_CHARSET ) { if ( flags[0] != '\0' ) strcat(flags,"|"); strcat(flags,"CHARSET"); }
			if ( prop->flags&PF_EXTENDED ) { if ( flags[0] != '\0' ) strcat(flags,"|"); strcat(flags,"EXTENDED"); }
			if ( prop->flags&PF_DEPRECATED ) { if ( flags[0] != '\0' ) strcat(flags,"|"); strcat(flags,"DEPRECATED"); }
			if ( prop->flags&PF_DEPRECATED_NONOTICE ) { if ( flags[0] != '\0' ) strcat(flags,"|"); strcat(flags,"DEPRECATED_NONOTICE"); }
			if ( flags[0] != '\0' )
			{
				len += write(",\n\t\t\t\t\"flags\" : \"%s\"",flags);
			}
			if ( prop->unit != NULL )
			{
				write(",\n\t\t\t\t\"unit\" : \"%s\"",prop->unit->name);
			}
			if ( prop->default_value != NULL )
			{
				PROPERTYSPEC *spec = property_getspec(prop->ptype);
				if ( spec != NULL && prop->default_value != spec->default_value )
				{	
					write(",\n\t\t\t\t\"default\" : \"%s\"",prop->default_value);
				}
			}
			len += write("\n\t\t\t}");
		}
		len += write("\n\t\t}");
	}
	len += write("\n\t}");
	IN_MYCONTEXT output_debug("GldJsonWriter::classes() wrote %d bytes",len);
	return len;
}

int GldJsonWriter::write_globals(FILE *fp)
{
	int len = 0;
	GLOBALVAR *var;
	len += write(",\n\t\"globals\" : {");

	/* for each module */
	for ( var = global_find(NULL) ; var != NULL ; var = global_getnext(var) )
	{
		char buffer[1024];
		if ( global_getvar(var->prop->name,buffer,sizeof(buffer)-1) ) // only write globals that can be extracted
		{
			KEYWORD *key;
			PROPERTYSPEC *pspec = property_getspec(var->prop->ptype);
			if ( var != global_find(NULL) )
				len += write(",");
			len += write("\n\t\t\"%s\" : {", var->prop->name);
			len += write("\n\t\t\t\"type\" : \"%s\",", pspec->name);
			for ( key = var->prop->keywords ; key != NULL ; key = key->next )
			{
				if ( key == var->prop->keywords )
				{
					len += write("\n\t\t\t\"keywords\" : {");
				}
				len += write("\n\t\t\t\t\"%s\" : \"0x%x\"",key->name,key->value);
				if ( key->next == NULL )
					len += write("\n\t\t\t}");
				len += write(",");
			}
			PROPERTY *prop = var->prop;
			char access[1024] = "";
			switch ( prop->access ) {
			case PA_PUBLIC: strcpy(access,"PUBLIC"); break;
			case PA_REFERENCE: strcpy(access,"REFERENCE"); break;
			case PA_PROTECTED: strcpy(access,"PROTECTED"); break;
			case PA_PRIVATE: strcpy(access,"PRIVATE"); break;
			case PA_HIDDEN: strcpy(access,"HIDDEN"); break;
			case PA_N: strcpy(access,"NONE"); break;
			default:
				if ( prop->access & PA_R ) strcat(access,"R");
				if ( prop->access & PA_W ) strcat(access,"W");
				if ( prop->access & PA_S ) strcat(access,"S");
				if ( prop->access & PA_L ) strcat(access,"L");
				if ( prop->access & PA_H ) strcat(access,"H");
				break;
			}
			len += write("\n\t\t\t\"access\" : \"%s\",",access);			
			if ( buffer[0] == '\"' )
				len += write("\n\t\t\t\"value\" : \"%s\"", escape(buffer+1,strlen(buffer)-2));
			else
				len += write("\n\t\t\t\"value\" : \"%s\"", escape(buffer));
			len += write("\n\t\t}");
		}
	}
	len += write("\n\t}");
	IN_MYCONTEXT output_debug("GldJsonWriter::globals() wrote %d bytes",len);
	return len;
}

static const char *escape(const char *text, char *buffer, size_t len)
{
	char *p = buffer;
	while ( *text != '\0' && p < buffer+len )
	{
		switch ( *text ) {
		case '\n':
			*p++ = '\\';
			*p++ = 'n';
			text++;
			break;
		case '\t':
			*p++ = '\\';
			*p++ = 't';
			text++;
			break;
		case '\r':
			*p++ = '\\';
			*p++ = 'r';
			text++;
			break;
		case '"':
			*p++ = '\\';
			*p++ = '"';
			text++;
			break;
		default:
			*p++ = *text++;
			break;
		}
	}
	*p = '\0';
	return buffer;
}
int GldJsonWriter::write_schedules(FILE *fp) 
{
	int len = 0;
	SCHEDULE *sch;
	len += write(",\n\t\"schedules\" : {");

	/* for each module */
	bool first = true;
	for ( sch = schedule_getfirst() ; sch != NULL ; sch = schedule_getnext(sch) )
	{
		if ( sch->flags&SN_USERDEFINED )
		{
			if ( ! first )
				len += write(",");
			first = false;
			char buffer[sizeof(sch->definition)*2];
			len += write("\n\t\t\"%s\" : \"%s\"", sch->name, escape(sch->definition,buffer,sizeof(buffer)));
		}
	}
	len += write("\n\t}");
	IN_MYCONTEXT output_debug("GldJsonWriter::schedules() wrote %d bytes",len);
	return len;
}

int GldJsonWriter::write_objects(FILE *fp)
{
	int len = 0;
	OBJECT *obj;
	len += write(",\n\t\"objects\" : {");

	/* scan each object in the model */
	for ( obj = object_get_first() ; obj != NULL ; obj = obj->next )
	{
		PROPERTY *prop;
		CLASS *pclass;
		char buffer[1025];
		if ( obj != object_get_first() )
			len += write(",");
		if ( obj->oclass == NULL ) // ignore objects with no defined class
			continue;
		if ( obj->name ) 
			len += write("\n\t\t\"%s\" : {",obj->name);
		else
			len += write("\n\t\t\"%s:%d\" : {", obj->oclass->name, obj->id);
		FIRST("id","%d",obj->id);
		TUPLE("class","%s",obj->oclass->name);

		/* handle special case for powerflow module handling of parent */
		OBJECT **topological_parent = object_get_object_by_name(obj,"topological_parent");
		if ( topological_parent != NULL )
			obj->parent = *topological_parent;

		if ( obj->parent != NULL )
		{
			if ( obj->parent->name == NULL )
				len += write(",\n\t\t\t\"parent\" : \"%s:%d\"",obj->parent->oclass->name,obj->parent->id);
			else
				TUPLE("parent","%s",obj->parent->name);
		}
		if ( ! isnan(obj->latitude) ) TUPLE("latitude","%f",obj->latitude);
		if ( ! isnan(obj->longitude) ) TUPLE("longitude","%f",obj->longitude);
		if ( obj->groupid[0] != '\0' ) TUPLE("groupid","%s",(const char*)obj->groupid);
		TUPLE("rank","%u",(unsigned int)obj->rank);
		if ( convert_from_timestamp(obj->clock,buffer,sizeof(buffer)) )
			TUPLE("clock","%s",buffer);
		if ( obj->valid_to > TS_ZERO && obj->valid_to < TS_NEVER ) TUPLE("valid_to","%llu",(int64)(obj->valid_to));
		TUPLE("schedule_skew","%llu",obj->schedule_skew);
		if ( obj->in_svc > TS_ZERO && obj->in_svc < TS_NEVER ) TUPLE("in","%llu",(int64)(obj->in_svc));
		if ( obj->out_svc > TS_ZERO && obj->out_svc < TS_NEVER ) TUPLE("out","%llu",(int64)(obj->out_svc));
		TUPLE("rng_state","%llu",(int64)(obj->rng_state));
		TUPLE("heartbeat","%llu",(int64)(obj->heartbeat));
		(len += write(",\n\t\t\t\"%s\" : \"%llX%llX\"","guid",(int64)(obj->guid[0]),(int64)(obj->guid[1])));
		TUPLE("flags","0x%llx",(int64)(obj->flags));
		for ( pclass = obj->oclass ; pclass != NULL ; pclass = pclass->parent )
		{
			for ( prop = pclass->pmap ; prop!=NULL && prop->oclass == pclass->pmap->oclass; prop = prop->next )
			{
				char buffer[1025*5];
				if ( prop->access != PA_PUBLIC )
					continue;
				else if ( prop->ptype == PT_enduse )
					continue;
				else if ( prop->ptype == PT_method )
                {
                    size_t sz = object_property_getsize(obj,prop);
                    if ( sz > 0 )
                    {
	                    char *buffer = new char[sz+2];
	                    strcpy(buffer,"");
	                    object_property_to_string(obj,prop->name,buffer,sz+1);
						len += write(",\n\t\t\t\"%s\": \"%s\"", prop->name, escape(buffer));
	                    delete [] buffer;
	                }
	                else if ( sz == 0 )
	                {
						len += write(",\n\t\t\t\"%s\": \"\"", prop->name);
	                }
	                else
	                {
	                	// no output allowed for this property
	                }
                }
                else if ( prop->ptype == PT_double )
                {
                	double *x = object_get_double_quick(obj,prop);
                	if ( prop->unit )
						len += write(",\n\t\t\t\"%s\": \"%g %s\"", prop->name, *x, prop->unit->name);
					else
						len += write(",\n\t\t\t\"%s\": \"%g\"", prop->name, *x);
                }
                else if ( prop->ptype == PT_complex )
                {
					complex *c = object_get_complex_quick(obj,prop);
					if ( prop->unit )
						len += write(",\n\t\t\t\"%s\": \"%g%+gj %s\"", prop->name, c->Re(), c->Im(), prop->unit->name);
					else
						len += write(",\n\t\t\t\"%s\": \"%g%+gj\"", prop->name, c->Re(), c->Im());
                }
                else
                {
					const char *value = object_property_to_string(obj,prop->name, buffer, sizeof(buffer));
					if ( value == NULL )
						continue; // ignore values that don't convert propertly
					int size = strlen(value);
					// TODO: proper JSON formatted is needed for data that is either a dict or a list
					// if ( value[0] == '{' && value[len] == '}')
					// 	len += write(",\n\t\t\t\"%s\" : %s", prop->name, value);
					// else if ( value[0] == '[' && value[len] == ']')
					// 	len += write(",\n\t\t\t\"%s\" : %s", prop->name, value);
					// else 
					if ( value[0] == '"' && value[size-1] == '"')
					{
						len += write(",\n\t\t\t\"%s\": \"%s\"", prop->name, escape(value+1,size-2));
					}
					else
					{
						len += write(",\n\t\t\t\"%s\": \"%s\"", prop->name, escape(value,size));
					}
				}
			}
		}
		len += write("\n\t\t}");
	}

	len += write("\n\t}");
	IN_MYCONTEXT output_debug("GldJsonWriter::objects() wrote %d bytes",len);
	return len;
}

// return number of bytes written
int GldJsonWriter::write_output(FILE *fp)
{
	int len = 0;
	json = fp;
	len += write("{\t\"application\": \"gridlabd\",\n");
	len += write("\t\"version\" : \"%u.%u.%u\"",global_version_major,global_version_minor,version);
	if ( (global_filesave_options&FSO_MODULES) == FSO_MODULES )
	{
		len += write_modules(fp);
	}
	if ( (global_filesave_options&FSO_PROPERTIES) == FSO_PROPERTIES )
	{
		len += write_properties(fp);
	}
	if ( (global_filesave_options&FSO_CLASSES) == FSO_CLASSES )
	{
		len += write_classes(fp);
	}
	if ( (global_filesave_options&FSO_GLOBALS) == FSO_GLOBALS )
	{
		len += write_globals(fp);
	}
	if ( (global_filesave_options&FSO_SCHEDULES) == FSO_SCHEDULES )
	{
		len += write_schedules(fp);
	}
	if ( (global_filesave_options&FSO_OBJECTS) == FSO_OBJECTS )
	{
		len += write_objects(fp);
	}
	len += write("\n}\n");
	IN_MYCONTEXT output_debug("GldJsonWriter::output() wrote %d bytes",len);
	return len;
}

// returns 0 on success, non-zero on failure
int GldJsonWriter::dump()
{
	const char *ext, *basename;
	size_t b;
	char fname[1024];
	FILE *fp;

	/* handle default filename */
	if (filename==NULL)
		filename="gridlabd.json";

	/* find basename */
	b = strcspn(filename,"/\\:");
	basename = filename + (b<strlen(filename)?b:0);

	/* find extension */
	ext = strrchr(filename,'.');

	/* if extension is valid */
	if (ext!=NULL && ext>basename)

		/* use filename verbatim */
		strcpy(fname,filename);

	else

		/* append default extension */
		strcat(strcpy(fname,filename),".json");

	/* open file */
	fp = fopen(fname,"w");
	if (fp==NULL)
		throw_exception("GldJsonWriter::dump(const char *filename='%s'): %s", filename, strerror(errno));
		/* TROUBLESHOOT
			The system was unable to output the json data to the specified file.  
			Follow the recommended solution based on the error message provided and try again.
		 */

	/* output data */
	int len = write_output(fp);

	/* close file */
	fclose(fp);

	return len > 0;
}

int json_to_glm(const char *jsonfile, char *glmfile)
{
	// TODO: convert JSON file to GLM
	return 0;
}
