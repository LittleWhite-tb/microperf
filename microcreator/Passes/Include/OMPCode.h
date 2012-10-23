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
  @file OMPCode.h
  @brief The OMPCode pass header is in this file 
 */

#ifndef H_OMPCODE
#define H_OMPCODE

#include "Pass.h"
#include "LoopInfo.h"
#include <set>

//Advanced declaration
class Description;
class InsertCode;
class Operand;
class RegisterOperand;
class Statement;

/**
 * @class OMPCode
 * @brief The OMPCode inserts generates C code (loop) and eventually add omp pragmas
 */
class OMPCode:public Pass
{
	protected:
		/**
		 * @brief Handle the Kernel
		 * @param kernel the Kernel we are interested in
		 * @param desc the description of the input (default value is NULL)
		 * @param regOperand set of registers declaration 
		 * @param memOperand set of register memory operand
		 */
		void handleKernel (Kernel *kernel, const Description *desc, std::set <std::string> &regOperand, std::set <std::string> &memOperand) const;

		/**
		 * @brief Handle the Statement
		 * @param instruction the Statement we are interested in
		 * @param desc the Description of the input (default value is NULL)
		 * @param regOperand set of registers declaration 
		 * @param memOperand set of register memory operand
		 */
		void handleStatement (Statement *instruction, const Description *desc, std::set <std::string> &regOperand, std::set <std::string> &memOperand) const;

		/**
		 * @brief Handle the Operand
		 * @param operand the Operand we are interested in
		 * @param regOperand set of registers declaration 
		 * @param memOperand set of register memory operand
		 */
		void handleOperand (Operand *operand, std::set <std::string> &regOperand, std::set <std::string> &memOperand) const;

		/**
		 * @brief Handle the omp parallel for option
		 * @param kernel the kernel we are interested in
		 * @param origin the outer kernel
		 */
		void handleOmpOption (Kernel *kernel, Kernel *origin) const;

		/**
		 * @brief Handle the omp parallel for option
		 * @param kernel the kernel we are interested in
		 * @param origin the outer kernel
		 * @param infoVect the vector of SLoopInfo information
		 * @param unroll the unroll factor of the kernel 
		 */
		void handleLoopHeader (Kernel *kernel, Kernel *origin, std::vector<SLoopInfo> &infoVect, unsigned int &unroll) const;

		/**
		 * @brief inline the assembly code 
		 * @param kernel the kernel we are interested in
		 * @param declRegs the map of the registers and the variables corresponding to
		 * @param infoVect the vector of SLoopInfo information
		 * @param start the start index (default = 0) 
		 */
		void handleInstruction (Kernel *kernel, std::map <std::string, std::string> &declRegs, std::vector<SLoopInfo> &infoVect, unsigned int start = 0) const;

		/**
		 * @brief Fill the register declarations 
		 * @param kernel the kernel we are interested in
		 * @param registers the register we are interested in 
		 * @param declRegs the map of the registers and the variables corresponding to
		 * @param pointer is it an immediate operand? if it is, we add asteri to the variable (default value is false)
		 */
		void addRegisterDeclaration (Kernel *kernel, std::set <std::string> registers, std::map <std::string, std::string> &declRegs, bool pointer = false) const;

		/**
		 * @brief Get the variable name corresponding to the  register name
		 * @param name The variable name we are searching from it's associated register
		 * @param reg The map of register name and variable declaration association
		 * @return the associated variable declaration
		 */
		const std::string &getRegisterDeclaration (const std::string &name, std::map <std::string, std::string> &reg) const;

		/**
		 * @brief the return statement of the C code 
		 * @param kernel the kernel we are interested in
		 * @param infoVect the vector of SLoopInfo information
		 * @param unroll the unroll factor of the kernel 
		 */
		void handleLoopReturn (Kernel *kernel, std::vector<SLoopInfo> &infoVect, unsigned int &unroll) const;

		/**
		 * @brief Handle the omp parallel for option
		 * @param option the OpenMP option (firstprivate, shared,...)
		 * @param kernel the kernel we are interested in
		 * @param origin the outer kernel
		 * @param oss the stream with the added option
		 */
		void addOptions (const std::string option, Kernel *kernel, Kernel *origin, std::ostringstream &oss) const;

	public:
		/**
		 * @brief Constructor
		 */
		OMPCode (void);

		/**
		 * @brief Destructor
		 */
		virtual ~OMPCode (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass 
		 */
		virtual std::vector<PassElement*> *entry (PassElement *pe, Description *desc = NULL) const;
};

#endif
