/*
Copyright (C) 2011 Exascale Research Center

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <libxml/parser.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Log.h"
#include "Config.h"
#include "Defines.h"
#include "Description.h"
#include "Toolkit.h"

enum { C_INT = 0, C_LONG, C_DOUBLE, C_STRING };

static inline int Config_getNodeAttribute (xmlNodePtr node, const char *attr_name, int type, void *data)
{
	assert (node != NULL && attr_name != NULL && data != NULL);
	xmlChar *str = xmlGetProp (node, (const xmlChar *)attr_name);
	char *end;
	if (str == NULL)
	{
		Log_output (-1, "Error: Cannot get attribute \"%s\"\n", attr_name);
		return 0;
	}
	
	switch (type)
	{
		case C_INT:
		{
			int *val = data;
			*val = strtol ((char*)str, &end, 10);
			xmlFree (str);
			return ((char*)str != end);
		}
		case C_LONG:
		{
			long *val = data;
			*val = strtol ((char*)str, &end, 10);
			xmlFree (str);
			return ((char*)str != end);
		}
		case C_DOUBLE:
		{
			double *val = data;
			*val = strtod ((char*)str, &end);
			xmlFree (str);
			return ((char*)str != end);
		}
		case C_STRING:
		{
			char *val = data;
			strncpy (val, (char*)str, STRBUF_MAXLEN);
			xmlFree (str);
			return 1;
		}
		default:
			break;
	}
	
	return 0;
}

static inline int Config_isSetNode (xmlNodePtr node, const char *node_name)
{
	return !xmlStrcmp (node->name, (const xmlChar *)node_name);
}

static inline xmlNodePtr Config_getChildNode (const xmlNodePtr node)
{
	assert (node != NULL);
	xmlNodePtr new = node->xmlChildrenNode;
	
	if (new == NULL)
	{
		Log_output (-1, "Error: \"<%s>\" DOM element has no child.\n", node->name);
		exit (EXIT_FAILURE);
	}
	return new;
}

static inline xmlNodePtr Config_getXmlNode (xmlNodePtr node, const char *node_name)
{
	assert (node != NULL);
	
	xmlNodePtr tmp = node;
	while (tmp != NULL && xmlStrcmp (tmp->name, (const xmlChar *)node_name))
	{
		tmp = tmp->next;
	}
	
	if (tmp == NULL)
	{
		Log_output (-1, "Error: Cannot find \"<%s>\" child in \"<%s>\" DOM element...\n", node_name, node->name);
		exit (EXIT_FAILURE);
	}
	
	return tmp;
}

int Config_parseXMLSpecialArgs (SDescription *desc, const char *filename)
{
	assert (filename != NULL);
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr tmp;
	int val;
	
	/* XML file parsing */
	doc = xmlParseFile (filename);
	if (doc == NULL)
	{
		Log_output (-1, "Error: Cannot parse file \"%s\".\n", filename);
		return -1;
	}
	
	/* Root level <microlaunch> */
	root = xmlDocGetRootElement (doc);
	if (root == NULL)
	{
		Log_output (-1, "Error: Cannot get root element of the document.\n");
		xmlFreeDoc (doc);
		return -1;
	}
	
	/* <microlaunch> level */
	if (!Config_isSetNode (root, "microlaunch"))
	{
		Log_output (-1, "Error: The root name \"%s\" is not correct.\n", root->name);
		xmlFreeDoc (doc);
		return -1;
	}
	
	/* <config> level */
	tmp = Config_getChildNode (root);
	tmp = Config_getXmlNode (tmp, "config");
	
	/* <options> level */
	tmp = Config_getChildNode (tmp);
	
	/* For each node of the <config> node */
	while (tmp != NULL)
	{
		if (Config_isSetNode (tmp, "nbVector")) // <nbVector>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setNbVectors (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "evalLib")) // <evalLib>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_parseEvaluationLibraryInput (desc, buf);
			}
		}
		
		tmp = tmp->next; // Go to the next element
	}
	
	/* Cleaning everything else */
	xmlFreeDoc (doc);
	
	return 0;
}

int Config_parseXML (SDescription *desc, const char *filename)
{
	assert (filename != NULL);
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr tmp;
	int val;
	
	/* XML file parsing */
	doc = xmlParseFile (filename);
	if (doc == NULL)
	{
		Log_output (-1, "Error: Cannot parse file \"%s\".\n", filename);
		return -1;
	}
	
	/* Root level <microlaunch> */
	root = xmlDocGetRootElement (doc);
	if (root == NULL)
	{
		Log_output (-1, "Error: Cannot get root element of the document.\n");
		xmlFreeDoc (doc);
		return -1;
	}
	
	/* <microlaunch> level */
	if (!Config_isSetNode (root, "microlaunch"))
	{
		Log_output (-1, "Error: The root name \"%s\" is not correct.\n", root->name);
		xmlFreeDoc (doc);
		return -1;
	}
	
	/* <config> level */
	tmp = Config_getChildNode (root);
	tmp = Config_getXmlNode (tmp, "config");
	
	/* <options> level */
	tmp = Config_getChildNode (tmp);
	
	/* For each node of the <config> node */
	while (tmp != NULL)
	{
		/**************************
		 * KERNEL MODE PARAMETERS *
		 **************************/
		if (Config_isSetNode (tmp, "startVector")) // <startVector>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setStartVectorSize (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "endVector")) // <endVector>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setEndVectorSize (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "stepVector")) // <stepVector>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setVectorSizeStep (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "iterationCount")) // <iterationCount>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setIterationCount (desc, val);
				Description_iterationCountEnable (desc);
			}
		}
		
		if (Config_isSetNode (tmp, "metaRepetition")) // <metaRepetition>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setMetaRepetition (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "repetition")) // <repetition>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setRepetition (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "executeRepetition")) // <executeRepetition>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setExecuteRepets (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "kernelName")) // <kernelName>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				if (isFile (buf))
				{
					Description_setKernelFileNamesTabSize (desc, 1);
					Description_setKernelFileNameAt (desc, buf, 0);
				}
				else if (isDirectory (buf))
				{
					parseDir (buf, desc);
				}
				else
				{
					Log_output (-1, "Error: Cannot resolve kernel name filetype.\n");
					exit (EXIT_FAILURE);
				}
				Description_setKernelFileName (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "kernelFunction")) // <kernelFunction>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_setDynamicFunctionName (desc,buf);
			}
		}
		
		if (Config_isSetNode (tmp, "dataSize")) // <dataSize>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				/* If user specifies "float", let's allocate sizeof(float) */
				if (strncmp ("float", buf, STRBUF_MAXLEN) == 0)
				{
					Description_setVectorElementSize (desc, sizeof (float));
					Log_output (-1, "setting vector elem size float\n");
				}
				/* If he specifies "double", let's allocate sizeof(double) */
				else if (strncmp ("double", buf, STRBUF_MAXLEN) == 0)
				{
					Description_setVectorElementSize (desc, sizeof (double));
					Log_output (-1, "setting vector elem size double\n");
				}
				/* Else, he must have specified a numeric constant */
				else
				{
					Config_getNodeAttribute (tmp, "value", C_INT, &val);
					Description_setVectorElementSize (desc, val);
					Log_output (-1, "setting vector elem size %d\n", val);
				}
			}
		}
		
		if (Config_isSetNode (tmp, "initFunction")) // <initFunction>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_setKernelInitFunctionName (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "nbSizes")) // <nbSizes>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_nbSizesEnable (desc);
				parseVectorSizes (desc, optarg);
			}
		}
		
		if (Config_isSetNode (tmp, "baseName")) // <baseName>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_setBaseName (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "sizeDummy")) // <sizeDummy>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setDummySize (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "nbProcess")) // <nbProcess>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setNbProcess (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "ompPath")) // <ompPath>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_setOmpPath (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "maxStride")) // <maxStride>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setMaxStride (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "vectSurveyor")) // <vectSurveyor>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				parse_vect_surveyor (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "pageSize")) // <pageSize>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setPageSize (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "pageCollide")) // <pageCollide>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setNbPagesForCollision (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "logOutput")) // <logOutput>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				FILE *f = fopen (buf, "w");
				Log_setOutput (f);
			}
		}
		
		if (Config_isSetNode (tmp, "logVerbosity")) // <logVerbosity>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Log_setVerbosity (val);
			}
		}
		
		if (Config_isSetNode (tmp, "noOutput")) // <noOutput>
		{
			Description_setPromptOutputCsv (desc, 0);
		}
		
		if (Config_isSetNode (tmp, "vectorSpacing")) // <vectorSpacing>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setVectorSpacing (desc, val);
			}
		}
		
		if (Config_isSetNode (tmp, "logOutput")) // <logOutput>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				FILE *f = fopen (buf, "w");
				Log_setOutput (f);
			}
		}
		
		if (Config_isSetNode (tmp, "outputDir")) // <outputDir>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_setOutputPath (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "outputSameDir")) // <outputSameDir>
		{
			Description_setOutputPath (desc, ".");
		}
		
		if (Config_isSetNode (tmp, "cpuPin")) // <cpuPin>
		{
			if (Config_getNodeAttribute (tmp, "value", C_INT, &val))
			{
				Description_setCPUDest (desc, val);
				Description_defineCpuPin (desc);
			}
		}
		
		if (Config_isSetNode (tmp, "allocLib")) // <allocLib>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_setOutputPath (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "summary")) // <summary>
		{
			Description_enableSummary (desc);
		}
		
		if (Config_isSetNode (tmp, "verifyLib")) // <verifyLib>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_setVerificationLibraryName (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "info")) // <info>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_parseInfoDisplayedInput (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "noEvalStack")) // <noEvalStack>
		{
			Description_disableEvalStack (desc);
		}
		
		/******************************
		 * EXECUTABLE MODE PARAMETERS *
		 ******************************/
		
		if (Config_isSetNode (tmp, "execName")) // <execName>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_setExecFileName (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "execArgs")) // <execArgs>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				unsigned nb_args;
				char **argv = getArgs (desc, buf, &nb_args);
				
				Description_setExecArgc (desc, nb_args);
				Description_setExecArgv (desc, argv);
			}
		}
		
		if (Config_isSetNode (tmp, "execOutput")) // <execOutput>
		{
			char buf[STRBUF_MAXLEN];
			if (Config_getNodeAttribute (tmp, "value", C_STRING, &buf))
			{
				Description_setExecFileName (desc, buf);
			}
		}
		
		if (Config_isSetNode (tmp, "suppressOutput")) // <suppressOutput>
		{
			Description_setSuppressOutput (desc, 1);
		}
		
		if (Config_isSetNode (tmp, "noThreadPin")) // <noThreadPin>
		{
			Description_pinThreadDisable (desc);
		}
		
		if (Config_isSetNode (tmp, "allMetricOutput")) // <allMetricOutput>
		{
			Description_allProcessOutputEnable (desc);
		}
		
		tmp = tmp->next; // Go to the next element
	}
	
	/* Cleaning everything else */
	xmlFreeDoc (doc);
	
	return 0;
}
