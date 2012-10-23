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
 @file Instruction.h
 @brief The Instruction class header is in this file
 */

#ifndef H_INSTRUCTION
#define H_INSTRUCTION

//For inheritance
#include "Statement.h"

#include <string>
#include <vector>

//Advance declarations
class Description;
class Operand;
class Operation;

/**
 * @class Instruction
 * @brief The Instruction class
 */
class Instruction : public Statement
{
	protected:
		Operation *op; 					/**< @brief The Operation */
		std::vector<Operand*> operands; /**< @brief The Operand variables */
		std::string comment;	/** < @brief The comment */

		bool combination; 	/**< @brief Combination or permutation? */
		bool swapBefore; 	/**< @brief Swap before unrolling? */
		bool swapAfter; 	/**< @brief Swap after unrolling? */

		bool chooseOpBefore;	/**< @brief Choose operation before unrolling */
		bool chooseOpAfter;		/**< @brief Choose operation after unrolling */

		std::vector<Operation*> operationVect; 	/**< @brief Vector of operations */

		bool immediateBefore; 	/**< @brief Choose immediate operand before unrolling */
		bool immediateAfter; 	/**< @brief Choose immediate operand after unrolling */

		/**
		 * @brief Fill the instruction
		 */
		void fillInstruction (void);

	public:
		/**
		 * @brief Constructor
		 */
		Instruction (void);

		/**
		 * @brief Destructor
		 */
		virtual ~Instruction ();

		/**
		 * @brief Get Operation 
		 * @return The Operation of this instruction
		 */
		const Operation *getOperation (void) const;

		/**
		 * @brief Get non constant Operation 
		 * @return An Operation  
		 */
		Operation *getModifiableOperation (void) const;

		/**
		 * @brief Set an Operation 
		 * @param op The Operation of this instruction
		 */
		void setOperation (Operation *op);

		/**
		 * @brief Return the number of operands
		 * @return the number of operands 
		 */
		unsigned int getNbrOperands (void) const;

		/**
		 * @brief Get an Operand
		 * @param idx the index of the operand we wish to obtain
		 * @return the operand we are interested in, NULL if no operand with that index
		 */
		const Operand *getOperand (unsigned int idx) const;

		/**
		 * @brief Set an operand
		 * @param idx the index of the operand we wish to update
		 * @param operand the operand to add
		 */
		void setOperand (unsigned int idx, Operand *operand);

		/**
		 * @brief Sets the comment
		 * @param value The new comment
		 */
		void setComment (const std::string comment);

		/**
		 * @brief Gets the comment
		 * @return Value of the comment
		 */
		const std::string getComment (void) const;

		/**
		 * @brief Get an Operand that we can modify
		 * @param idx the index of the operand we wish to obtain
		 * @return the operand without the const we are interested in, NULL if no operand with that index
		 */
		Operand *getModifiableOperand (unsigned int idx);

		/**
		 * @brief Adds the Operand to the list
		 * @param operand the Operand we are interested in
		 */
		void addOperand (Operand *operand);

		/**
		 * @brief Set the combination
		 * @param combination true if we want to
		 */
		void setCombination (bool combination);

		/**
		 * @brief Get the combination
		 * @return the value we want
		 */
		bool getCombination (void) const;

		/**
		 * @brief Set the swap before
		 * @param v the value we want
		 */
		void setSwapBefore (bool v);

		/**
		 * @brief Get the swap before
		 * @return the value we want
		 */
		bool getSwapBefore (void) const;

		/**
		 * @brief Set the swap after
		 * @param swap true if we want to
		 */
		void setSwapAfter (bool swap);

		/**
		 * @brief Get the swap after
		 * @return the value we want
		 */
		bool getSwapAfter (void) const;

		/**
		 * @brief Swap the order of operands
		 */
		void swapOperands (void);

		/**
		 * @brief Set the chooseOp before
		 * @param chooseOpBefore true if we want to
		 */
		void setChooseOpBefore (bool chooseOpBefore);

		/**
		 * @brief Get the chooseOp before
		 * @return the value we want
		 */
		bool getChooseOpBefore (void) const;

		/**
		 * @brief Set the chooseOp after
		 * @param chooseOpAfter true if we want to
		 */
		void setChooseOpAfter (bool chooseOpAfter);

		/**
		 * @brief Get the chooseOp after
		 * @return the value we want
		 */
		bool getChooseOpAfter (void) const;

		/**
		 * @brief Set the immediate operand before
		 * @param immediateBefore true if we want to
		 */
		void setImmediateBefore (bool immediateBefore);

		/**
		 * @brief Get the immediate operand before
		 * @return the value we want
		 */
		bool getImmediateBefore (void) const;

		/**
		 * @brief Set the immediate operand after
		 * @param immediateAfter true if we want to
		 */
		void setImmediateAfter (bool immediateAfter);

		/**
		 * @brief Get the immediate after
		 * @return the value we want
		 */
		bool getImmediateAfter (void) const;

		/**
		 * @brief Set the vector of operations
		 * @param opVect the vector of operations
		 */
		void setOperationVect (std::vector<Operation*> opVect);

		/**
		 * @brief Get the vector of operations
		 * @return the vector of operations
		 */
		std::vector<Operation*> getOperationVect (void) const;

		/** From Statement **/

		/**
		 * @brief Get a String
		 * @param s the string we wish to fill
		 * @param desc the description
		 * @param tab the tabulation (default: 1)
		 * @return whether or not this succeeded
		 */
		virtual bool getString (std::string &s, const Description *desc, unsigned int tab = 1) const;

		/**
		 * @brief Set Unroll information for this Instruction
		 * @param nbrIterationsAdvance number of iterations in advance
		 */
		virtual void updateUnrollInformation (int nbrIterationsAdvance);

		/**
		 * @brief Update RegisterOperand's names
		 * @param nbrIterationsAdvance number of iterations in advance
		 */
		virtual void updateRegisterName (int nbrIterationsAdvance);

		/**
		 * @brief Copy the Kernel (actually calls the copy function with true)
		 * @return the new Kernel
		 */
		virtual Statement *copy (void) const;

		/**
		 * @brief Copy the information of the statement
		 * @param stmt The statement
		 */
		virtual void copyInformation (const Statement *stmt);

		/**
		 * @brief Debug function
		 * @param out the output stream
		 * @param desc the Description
		 * @param tab tabulation
		 */
		virtual void debug (std::ofstream &out, const Description *desc, int tab) const;

		/**
		 * @brief compare this Statement to another one
		 * @param stmt the statemen we wish to compare to
		 * @return whether or not stmt is similar to this one
		 */
		virtual bool isSimilar (const Statement *stmt) const;
};
#endif
