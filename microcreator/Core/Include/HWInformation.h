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
 @file HWInformation.h
 @brief The HWInformation class header is in this file
 */

#ifndef H_HWINFORMATION
#define H_HWINFORMATION

#include <libxml++/libxml++.h>
#include <map>
#include <string>
#include <vector>

#include "ParserXML.h"

/**
 * @class HWInformation
 * @brief The HWInformation of the input
 */
class HWInformation : public ParserXML
{
	protected:
		std::map<std::string, std::string> register_association; /**< @brief register associations */
		std::map<std::pair<std::string, int>, std::vector<std::string> > operation_possibility; /**< @brief operation possibility */

		/** 
		 * @brief Parse a file
		 * @param file the name of the file
		 */
		void parseFile (const std::string &file);

		/**
		 * @brief Parse a global node
		 * @param node the global node
		 */
		void parse (const xmlpp::Node *node);

		/**
		 * @brief Parse a register_association node
		 * @param node the register_association node
		 */
		void parseRegisterAssociation (const xmlpp::Node *node);

		/**
		 * @brief Parse a register node
		 * @param node the register node
		 */
		void parseRegister (const xmlpp::Node *node);

		/**
		 * @brief Parse instruction_mdl node 
		 * @param node the instruction_mdl node
		 */
		void parseOperationPossibility (const xmlpp::Node *node);

		/**
		 * @brief Parse a mov node 
		 * @param node the operation node
		 * @param opName the operation name
		 */
		void parseOperation (const xmlpp::Node *node, std::string opName);

	public:
		/**
		 * @brief Constructor
		 * @param execute The execution we need to do to produce hwFile
		 * @param hwFile the hardware file we will use for the hardware information
		 */
		HWInformation (const std::string &execute, const std::string &hwFile);

		/**
		 * @brief Destructor
		 */
		virtual ~HWInformation (void);

		/**
		 * @brief Get the physical register when considering the virtual one
		 * @param vname the virtual register name
		 * @return the physical register name
		 */
		const std::string &getPhysicalRegister (const std::string &vname) const;

		/**
		 * @brief Get the vector corresponding to the operation size
		 * @param opName the operation name
		 * @param size the MDL operation size
		 * @return the vector of the different instruction
		 */
		const std::vector<std::string> &getOpPossibility (const std::string &opName, const unsigned int size) const;

		/**
		 * @ brief Get the line number of a given node
		 * @ param node the node we are interested in
		 * @ return the line number in string format
		 */
		std::string getLine (const xmlpp::Node* node);
};
#endif
