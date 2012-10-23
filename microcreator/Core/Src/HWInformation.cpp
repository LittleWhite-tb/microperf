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

/**
 @file HWInformation.cpp
 @brief The HWInformation class is in this file
 */

#include <cassert>
#include <iostream>
#include <libxml++/libxml++.h>
#include <stdlib.h>

#include "HWInformation.h"
#include "Logging.h"

HWInformation::HWInformation (const std::string &execute, const std::string &hwFile)
{
	Logging::log (0, "HWInformation: Executing: ", execute.c_str (), NULL);

	int res = system (execute.c_str ());

	if (res != 0)
	{
		Logging::log (2, "HWInformation: Error: Problem when executing: ", execute.c_str (), NULL);
		setParsingIsOkay (false);
	}

	Logging::log (0, "HWInformation: Done executing: ", execute.c_str (), NULL);

	//Now we can parse the output
	parseFile (hwFile);

	Logging::log (0, "HWInformation: Finished parsing: ", hwFile.c_str (), NULL);
}

HWInformation::~HWInformation (void)
{
	register_association.clear ();
}

void HWInformation::parseFile (const std::string &file)
{
	xmlpp::DomParser parser;
	parser.set_substitute_entities ();
	parser.parse_file (file);

	if (parser)
	{
		//Walk the tree:
		const xmlpp::Node* pNode = parser.get_document ()->get_root_node (); //deleted by DomParser.
		parse (pNode);
	}
}

void HWInformation::parse (const xmlpp::Node *node)
{
	if (verifyNodeName (node, "description"))
	{
		//Recurse through child nodes:
		xmlpp::Node::NodeList list = node->get_children ();

		for (xmlpp::Node::NodeList::iterator iter = list.begin (); iter != list.end (); ++iter)
		{
			const xmlpp::Node *tmp = *iter;

			//Be paranoid
			assert (tmp != NULL);

			if (verifyCanGetName (tmp))
			{
				if (verifyNodeName (tmp, "register_association"))
				{
					parseRegisterAssociation (tmp);
				}
				else
					if (verifyNodeName (tmp, "description_mdl"))
					{
						parseOperationPossibility (tmp);
					}
					else
					{
						Logging::log (1, "HWInformation: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in description node at: ", getLine (tmp).c_str (), NULL);
					}
			}
		}
	}
	else
	{
		Logging::log (1, "HWInformation: Warning: Unsupported name '", extractNodeName (node).c_str (), "' at: ", getLine (node).c_str (), NULL);
	}
}

void HWInformation::parseRegisterAssociation (const xmlpp::Node *node)
{
	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();

	for (xmlpp::Node::NodeList::iterator iter = list.begin (); iter != list.end (); ++iter)
	{
		const xmlpp::Node *tmp = *iter;

		//Be paranoid
		assert (tmp != NULL);

		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "register"))
			{
				parseRegister (tmp);
			}
			else
			{
				Logging::log (1, "HWInformation: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in register_association node at: ", getLine (tmp).c_str (), NULL);
			}
		}
	}
}

void HWInformation::parseRegister (const xmlpp::Node *node)
{
	std::string vname = "";
	std::string pname = "";

	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();

	for (xmlpp::Node::NodeList::iterator iter = list.begin (); iter != list.end (); ++iter)
	{
		const xmlpp::Node *tmp = *iter;

		//Paranoid
		assert (tmp != NULL);

		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "virtual"))
			{
				vname = extractString (tmp);
			}
			else
			{
				if (verifyNodeName (tmp, "physical"))
				{
					pname = extractString (tmp);
				}
				else
				{
					Logging::log (1, "HWInformation: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in register node at: ", getLine (tmp).c_str (), NULL);
				}
			}
		}
	}

	//Ok now check what we have
	if (vname[0] == '\0')
	{
		Logging::log (1, "Warning: virtual register is empty, register ignored", NULL);
	}

	if (pname[0] == '\0')
	{
		Logging::log (1, "Warning: virtual register is empty, register ignored", NULL);
	}

	//Now actually add it if both are not empty
	if (vname[0] != '\0' && pname[0] != '\0')
	{
		//Check if not yet there
		std::map<std::string, std::string>::const_iterator it;
		it = register_association.find (vname);

		if (it != register_association.end ())
		{
			Logging::log (1, "Warning: virtual register already defined", NULL);
		}

		register_association[vname] = pname;
	}
}

void HWInformation::parseOperationPossibility (const xmlpp::Node *node)
{
	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();

	for (xmlpp::Node::NodeList::iterator iter = list.begin (); iter != list.end (); ++iter)
	{
		const xmlpp::Node *tmp = *iter;

		assert (tmp != NULL);

		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "load"))
				parseOperation (tmp, "load");
			else
				if (verifyNodeName (tmp, "store"))
					parseOperation (tmp, "store");
				else
					if (verifyNodeName (tmp, "add"))
						parseOperation (tmp, "add");
					else
						if (verifyNodeName (tmp, "mul"))
							parseOperation (tmp, "mul");
						else
							if (verifyNodeName (tmp, "random"))
								parseOperation (tmp, "random");
							else
							{
								Logging::log (1, "HWInformation: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in description_mdl node at: ", getLine (tmp).c_str (), NULL);
							}
		}
	}
}

void HWInformation::parseOperation (const xmlpp::Node *node, std::string opName)
{
	unsigned int size = 0;
	std::vector < std::string > vectPossibility;

	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();

	for (xmlpp::Node::NodeList::iterator iter = list.begin (); iter != list.end (); ++iter)
	{
		const xmlpp::Node *tmp = *iter;

		//Be paranoid
		assert (tmp != NULL);

		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "size"))
			{
				size = convertStringInt (extractString (tmp));
			}
			else 
				if (verifyNodeName (tmp, "possibility"))
				{
					vectPossibility.push_back (extractString (tmp));
				}
				else
				{
					Logging::log (1, "HWInformation: Warning: Node not handled in MDL parseOperation of HWInformation", NULL);
				}
		}
	}

	//Add the operation to the map
	std::map<std::pair<std::string, int>, std::vector<std::string> >::const_iterator it;
	std::pair<std::string, int> val;
	val = make_pair (opName, size);
	it = operation_possibility.find (val);

	//Check if not yet there
	if (it != operation_possibility.end ())
	{
		Logging::log (1, "HWInformation: Warning: operation already defined", NULL);
	}

	operation_possibility[val] = vectPossibility;
}

const std::string &HWInformation::getPhysicalRegister (const std::string &vname) const
{
	static const std::string res = "";

	std::map<std::string, std::string>::const_iterator it;

	//Find it
	it = register_association.find (vname);

	//Did we find it ?
	if (it != register_association.end ())
	{
		return it->second;
	}

	return res;
}

const std::vector<std::string> &HWInformation::getOpPossibility (const std::string &opName, const unsigned int size) const
{
	static std::vector<std::string> vect;

	std::map<std::pair<std::string, int>, std::vector<std::string> >::const_iterator it;

	//find the vector	
	std::pair<std::string, int> val;
	val = make_pair (opName, size);
	it = operation_possibility.find (val);

	//Did we find it ?
	if (it != operation_possibility.end ())
	{
		return it->second;
	}

	return vect;
}

std::string HWInformation::getLine (const xmlpp::Node *node)
{
	std::ostringstream oss;
	oss << node->get_line ();
	return oss.str ();
}

