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
 @file ParserXML.cpp
 @brief The ParserXML class is in this file
 */

#include <cassert>
#include <iostream>
#include <libxml++/libxml++.h>

#include "Logging.h"
#include "ParserXML.h"

ParserXML::ParserXML (void)
{
	setParsingIsOkay (true);
}

ParserXML::~ParserXML (void)
{
}

bool ParserXML::verifyNodeName (const xmlpp::Node* node, const std::string &name) const
{
	return (extractNodeName (node) == name);
}

std::string ParserXML::extractNodeName (const xmlpp::Node* node) const
{
	const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*> (node);
	const xmlpp::CommentNode* nodeComment = dynamic_cast<const xmlpp::CommentNode*> (node);
	std::string readLine = "";
	Glib::ustring nodename = node->get_name ();

	if (nodeText && (nodeText->is_white_space () || nodeText->get_content () == ""))
		return "";

	if (!nodeText && !nodeComment && !nodename.empty ())
		readLine = node->get_name ();

	return (readLine);
}

bool ParserXML::verifyCanGetName (const xmlpp::Node* node) const
{
	const xmlpp::TextNode* nodeText = dynamic_cast<const xmlpp::TextNode*> (node);
	const xmlpp::CommentNode* nodeComment = dynamic_cast<const xmlpp::CommentNode*> (node);
	Glib::ustring nodename = node->get_name ();

	if (nodeText && (nodeText->is_white_space () || nodeText->get_content () == ""))
		return false;

	return (!nodeText && !nodeComment);
}

std::string ParserXML::extractString (const xmlpp::Node* node) const
{
	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();
	std::string str;

	assert (!list.empty ());

	const xmlpp::ContentNode* nodeContent = dynamic_cast<const xmlpp::ContentNode*> (*list.begin ());

	assert (nodeContent != NULL);

	return nodeContent->get_content ();
}

int ParserXML::convertStringInt (const std::string &a)
{
	std::stringstream iss (a);
	int tmp;

	iss >> tmp;

	if (iss.eof () == false)
	{
		Logging::log (1, "XML: Warning: Unsupported integer conversion :", a.c_str (), NULL);
		setParsingIsOkay (false);
	}

	return tmp;
}

float ParserXML::convertStringFloat (const std::string &a) const
{
	std::stringstream iss (a);
	float tmp;

	iss >> tmp;

	return tmp;
}

bool ParserXML::getParsingIsOkay (void) const
{
	return parsingIsOkay;
}

void ParserXML::setParsingIsOkay (bool val)
{
	parsingIsOkay = val;
}
