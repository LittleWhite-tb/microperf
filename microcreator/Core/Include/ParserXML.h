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
 @file ParserXML.h
 @brief The ParserXML class header is in this file
 */

#ifndef H_PARSERXML
#define H_PARSERXML

#include <libxml++/libxml++.h>

/**
 * @class ParserXML
 * @brief The ParserXML contains generic functions that we have for parsing XML
 */
class ParserXML
{
	protected:
		bool parsingIsOkay; /**< @brief If parsing is ok */

		/**
		 * @brief Constructor
		 */
		ParserXML (void);

		/**
		 * @brief Destructor
		 */
		virtual ~ParserXML (void);

		/**
		 * @brief Extratcing the string contained by a node
		 * @param node from where we have to extract the string
		 * @return string read in the node
		 */
		std::string extractString (const xmlpp::Node* node) const;

		/**
		 * @brief Verifying that the name of the node read match with 
		 * the desired string name
		 * @param node which we have to test his name
		 * @param name the name of the desired node
		 * @return true if the name match false otherwise
		 */
		bool verifyNodeName (const xmlpp::Node* node, const std::string &name) const;

		/**
		 * @brief Verifying that the name of the node read match with 
		 * the desired string name
		 * @param node which we have to test his name
		 * @return the node name
		 */
		std::string extractNodeName (const xmlpp::Node* node) const;

		/**
		 * @brief Verifying that we can get the node's name
		 * @param node which we have to test his name
		 * @return true if we can extract the name via verifyNodeName
		 */
		bool verifyCanGetName (const xmlpp::Node* node) const;

		/**
		 * @brief Convert a string value to an int
		 * @param a which has to be translated
		 * @return int translated from the string
		 */
		int convertStringInt (const std::string &a);

		/**
		 * @brief Convert a string value to a float
		 * @param a which has to be translated
		 * @return float translated from the string
		 */
		float convertStringFloat (const std::string &a) const;

	public:
		/**
		 * @brief Whether the input file parsing is good
		 */
		bool getParsingIsOkay (void) const;

		/**
		 * @brief Whether the input file parsing is good
		 */
		void setParsingIsOkay (bool val);
};
#endif
