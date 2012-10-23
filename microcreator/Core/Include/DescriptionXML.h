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
 @file DescriptionXML.h
 @brief The DescriptionXML class header is in this file
 */

#ifndef H_DESCRIPTIONXML
#define H_DESCRIPTIONXML

#include <libxml++/libxml++.h>

#include "Description.h"
#include "LoopInfo.h"
#include "ParserXML.h"

class Kernel;
class ImmediateOperand;
class IndirectMemoryOperand;
class InductionOperand;
class Instruction;
class MemoryOperand;
class Operation;
class RegisterOperand;
class Statement;

/**
 * @class DescriptionXML
 * @brief The DescriptionXML is the XML version of the data extractor
 */
class DescriptionXML : public Description, public ParserXML
{
	public:
		/**
		 * @brief Constructor
		 * @param filePath the path of the file
		 */
		DescriptionXML (const std::string &filePath);

		/**
		 * @brief Destructor
		 */
		virtual ~DescriptionXML (void);

		/**
		 * @brief Initialization function
		 */
		void init (void);

		/**
		 * @brief Parsing the description file
		 * @param filePath the path of the xml file to be parsed
		 */
		virtual void parse (const std::string &filePath);

	protected:
		/**
		 * @brief Parsing from a node
		 * @param node the root node from where we start to parse
		 */
		void parse (const xmlpp::Node* node);

		/**
		 * @brief Called when a XML flag's name is branch_information is reached in the file which is a node,
		 * it will call different method depending from his children flag
		 * @param node a branch_information node
		 * @param kernel the Kernel we are adding the node
		 */
		void passBranchInformation (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Called when a XML flag's name is benchmark_amount is reached in the file which is a node
		 * @param nodeg a benchmark_amount node
		 */
		void passBenchmark_amount (const xmlpp::Node* nodeg);

		/**
		 * @brief Called when a XML flag's name is concerning the code insertions
		 * @param node a code insertion node
		 */
		void passCodeInsert (const xmlpp::Node *node);

		/**
		 * @brief Called when a XML flag's name is description is reached in the file which is a node,
		 *  it will call different method depending from his children flag
		 * @param nodeg a description node
		 * @return whether or not things were done correctly
		 */
		bool passDescription (const xmlpp::Node* nodeg);

		/**
		 * @brief Called when a XML flag's name is kernel is reached in the file which
		 * is a node, it will call different method depending from his children flag
		 * @param node A kernel node
		 * @param kernel the Kernel we are adding the node
		 * @return whether or not things were done correctly
		 */
		bool passKernel (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Called when a XML flag's name is concerning the hardware detector
		 * @param node A Hardware detector node
		 */
		void passHardwareDetector (const xmlpp::Node *node);

		/**
		 * @brief Called when a XML flag's name is concerning an induction variable 
		 * @param node An induction node
		 * @param kernel the Kernel we are adding the node
		 */
		void passInductionOperand (const xmlpp::Node *node, Kernel *kernel);

		/**
		 * @brief Called when a XML flag's name is concerning an induction linked variable 
		 * @param node An induction linked node
		 * @param kernel the Kernel we are adding the node
		 * @return the InductionOperand we are linked to
		 */
		InductionOperand *passInductionLinked (const xmlpp::Node* node,
				Kernel *kernel);

		/**
		 * @brief Called when a XML flag's name is concerning an induction variable 
		 * @param node An induction node
		 * @param minStride the Minimum stride
		 * @param maxStride the Maximum stride
		 * @param progressStride the progress stride
		 */
		void passInductionStride (const xmlpp::Node *node, int &minStride,
				int &maxStride, int &progressStride);

		/**
		 * @brief Called when a XML flag's name is insert_code is reached in the file which is a node
		 * @param node an instruction node
		 * @param kernel the Kernel we are adding the node
		 */
		void passInsertCode (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Called when a XML flag's name is instruction is reached in the file 
		 * which is a node, it will call different method depending from his children flag
		 * @param node an instruction node
		 * @param kernel the Kernel we are adding the node
		 * @return whether or not things were done correctly
		 */
		bool passInstruction (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Called when a XML flag's name is Label is reached in the file which is a node,
		 * it will set the label value of Description with his content
		 * @param node Label node
		 * @param label the Label to fill
		 */
		void passLabel (const xmlpp::Node* node, std::string &label);

		/**
		 * @brief Called when a XML flag's name is alignment 
		 * @param node an alignment node
		 * @param kernel the Kernel we are adding to
		 */
		void passAlignment (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Compute the alignment value
		 * @param value The value from which we get the alignment value
		 * @return The alignment value
		 */
		int getAlignmentValue (int value);

		/**
		 * @brief Called when a XML flag's name is operation is reached in the file which is a node
		 * and will set the Instruction's Operation
		 * @param node an operation node
		 * @return the Instruction to fill
		 */
		Operation* passOperation (const xmlpp::Node* node);

		/**
		 * @brief Called when a XML flag's name is memory is reached in the file which is a node
		 * it would create an MemoryOperand corresponding to his children content
		 * @param node a mermory node
		 * @return MemoryOperand created during the parsing
		 */
		MemoryOperand* passMemory (const xmlpp::Node* node);

		/**
		 * @brief Called when a XML flag's name is indirect memory is reached in the file which
		 * is a node it would create an IndirectMemoryOperand corresponding to his children content
		 * @param node An indirect mermory node
		 * @return IndirectMemoryOperand created during the parsing
		 */
		IndirectMemoryOperand* passIndirectMemory (const xmlpp::Node* node);

		/**
		 * @brief Called when a XML flag's name is register is reached in the file which is a node
		 * it would create a RegisterOperand depending of calls of different method from his children flag
		 * @param node A register node
		 * @param nodeName the name used for this register name, default is "register"
		 * @return RegisterOperand created during the parsing
		 */
		RegisterOperand* passRegister (const xmlpp::Node* node,
				const std::string &nodeName = "register");

		/**
		 * @brief Called when a XML flag's name is offset is reached in the file which is a node,
		 * it whould fill the MemoryOperand of his value
		 * @param node an offset node
		 * @param memoryOperand a MemoryOperand which will be given the value of the offset
		 */
		void passOffset (const xmlpp::Node* node, MemoryOperand* memoryOperand);

		/**
		 * @brief Called when a XML flag's name is repetition is reached in the file which is a node
		 * and will set the statement's repetition
		 * @param node an operation node
		 * @param stmt the Statement 
		 */
		void passRepetition (const xmlpp::Node* node, Statement* stmt);

		/**
		 * @brief Called when a XML flage's name test is reached in the file which is a node, it will 
		 * it will fill the labelInstruction of the Description file
		 * @param node a test node
		 * @param instruction to fill
		 */
		void passTest (const xmlpp::Node* node, std::string &instruction);

		/**
		 * @brief Called when a XML flag's name is unrolling is reached in the file which is a node
		 * @param node an unrolling node
		 * @param kernel the Kernel we are adding the node
		 * @return whether or not things were done correctly
		 */
		bool passUnrolling (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Called when a XML flag's name is address is reached in the file which is a node,
		 * it would create an ImmediateOperand corresponding to his value
		 * @param node an address node
		 * @param instruction containing the immediate operand
		 * @return A MemoryOperand which will be set with the value of the offset
		 */
		ImmediateOperand* passImmediate (const xmlpp::Node* node,
				Instruction *instruction);

		/**
		 * @brief Called when an XML flag's name is loop_info
		 * @param node a for loop node
		 * @param kernel the Kernel we are adding to
		 */
		void passLoopFor (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Called when an XML flag's name is count_iteration
		 * @param node a count iteration node
		 * @param kernel the Kernel we are adding the node
		 */
		void passCountIteration (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Called when an XML flag's name is schedule
		 * @param node a schedule node
		 * @param kernel the Kernel we are adding the node
		 */
		void passScheduleInfo (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Called when an XML flag's name is omp_option (OpenMP)
		 * @param node a count iteration node
		 * @param kernel the Kernel we are adding the node
		 */
		void passOmpOption (const xmlpp::Node* node, Kernel *kernel);

		/**
		 * @brief Called when an XML flag's name is of the OpenMP option
		 * @param node
		 * @param nodeName the name of the node
		 * @param kernel the Kernel we are interesting in
		 */
		void passOmpOptionRegister (const xmlpp::Node* node,
				const std::string &nodeName, Kernel *kernel);

		/**
		 * @brief Handle the node containing min/max/progress flag
		 * @param node the node we are interested in
		 * @param min the node called min 
		 * @param max the node called max
		 * @param progress the node called progress 
		 * @param stmt the statement that could be used for the linked
		 */
		void minMaxProgress (const xmlpp::Node* node, int &min, int &max,
				int &progress, Statement *stmt = NULL);

		/**
		 * @ brief Get the line number of a given node
		 * @ param node the node we are interested in
		 * @ return the line number in string format
		 */
		std::string getLine (const xmlpp::Node* node);

		/**
		 * @brief Called when a XML flag's name is register, it concerns the loop info 
		 * @param node A register node
		 * @return SRegName structure containing the name and the offset
		 */
		SRegName passRegisterFor (const xmlpp::Node* node);

};
#endif
