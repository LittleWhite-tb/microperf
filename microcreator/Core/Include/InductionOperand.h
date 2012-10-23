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
 @file InductionOperand.h
 @brief The InductionOperand class header is in this file
 */

#ifndef H_INDUCTIONOPERAND
#define H_INDUCTIONOPERAND

//Advanced declaration
class Instruction;

#include <string>
#include "RegisterOperand.h"

/**
 * @class InductionOperand
 * @brief InductionOperand is the operand of the induction variables
 */
class InductionOperand : public RegisterOperand
{
	protected:
		int stride; 		/**< @brief value of the stride */
		int increment; 		/**< @brief value of the increment */
		bool affectedUnroll;/**< @brief Is this induction variable affected by the unrolling? */
		bool lastInduction; /**<@brief Is this induction variable the last one to be displayed */
		int offset; 		/**< @brief Offset */
		int unroll; 		/**< @brief Value unrolled */
		int minStride; 		/**< @brief Minimum stride */
		int maxStride; 		/**< @brief Maximum stride */
		int progressStride;	/**< @brief Progress stride */
		int origStride; 	/**< @brief Original stride */
		bool noGenerated; 	/**<@brief Is this induction variable displayed or not */
		InductionOperand *linked; /**< @brief We are linked to this variable */
		std::string comment;	/**< @brief A comment to issue in the output */

	public:
		/**
		 * @brief Constructor
		 * @param RegOpVirtualName the name of the RegisterOperand
		 * @param RegOpPhysicalName the physical name of the RegisterOperand
		 * @param offset the offset
		 * @param increment the increment
		 * @param minStride the minimum stride for this InductionOperand variable
		 * @param maxStride the maximum stride for this InductionOperand variable
		 * @param progressStride the progress stride for this induction variable
		 * @param unroll the affected unroll
		 * @param last the last induction
		 * @param link the linked InductionOperand
		 * @param no_generated the noGenerated  
		 */
		InductionOperand (const std::string &RegOpVirtualName, const std::string &RegOpPhysicalName = "",
				int offset = 0, int increment = 0, int minStride = 1, int maxStride = 1, int progressStride = 1, bool unroll = true,
				bool last = false, InductionOperand *link = NULL, bool no_generated = false, const std::string comment = "");

		/**
		 * @brief Destructor
		 */
		~InductionOperand (void);

		/**
		 * @brief Initialisation function of the InductionOperand class
		 */
		void init (void);

		/**
		 * @brief Get the stride
		 * @return int value of the stride 
		 */
		virtual int getStride (void) const;

		/**
		 * @brief Set the stride value
		 * @param value value to set 
		 */
		void setStride (int value);

		/**
		 * @brief Get the base increment without the stride
		 * @return int value of the base increment without the stride
		 */
		virtual int getBaseIncrement (void) const;

		/**
		 * @brief Get the increment
		 * @return int value of the increment 
		 */
		virtual int getIncrement (void) const;

		/**
		 * @brief Set the increment value
		 * @param value value to set 
		 */
		void setIncrement (int value);

		/**
		 * @brief Get the base offset without the stride
		 * @return int value of the base offset without the stride
		 */
		virtual int getBaseOffset (void) const;

		/**
		 * @brief Get the offset
		 * @return int value of the offset 
		 */
		virtual int getOffset (void) const;

		/**
		 * @brief Set the offset value
		 * @param value value to set 
		 */
		void setOffset (int value);

		/**
		 * @brief Get the unrolling value
		 * @return int value unrolling
		 */
		virtual int getUnroll (void) const;

		/**
		 * @brief Set the unroll value
		 * @param value value to set
		 */
		void setUnroll (int value);

		/**
		 * @brief Get the linked
		 * @return InductionOperand* value of the linked 
		 */
		virtual InductionOperand* getLinked (void) const;

		/**
		 * @brief Set the linked value
		 * @param value value to set 
		 */
		void setLinked (InductionOperand* value);

		/**
		 * @brief Get the maxStride
		 * @return value of the maxStride 
		 */
		int getMaximumStride (void) const;

		/**
		 * @brief Set the maxStride value
		 * @param value value to set 
		 */
		void setMaximumStride (int value);

		/**
		 * @brief Get the minStride
		 * @return value of the minStride 
		 */
		int getMinimumStride (void) const;

		/**
		 * @brief Set the minStride value
		 * @param value value to set 
		 */
		void setMinimumStride (int value);

		/**
		 * @brief Get the progressStrise
		 * @return value of the progressStride
		 */
		int getProgressStride (void) const;

		/**
		 * @brief Set the progressStride
		 * @param value value to set
		 */
		void setProgressStride (int value);

		/**
		 * @brief Get the affected
		 * @return bool value of the affected 
		 */
		bool getAffected (void) const;

		/**
		 * @brief Set the affected value
		 * @param value value to set 
		 */
		void setAffected (bool value);

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
		 * @brief Copy this operand
		 * @return a copy of this operand
		 */
		virtual Operand *copy (void) const;

		/**
		 * @brief Get the Instruction version of this InductionOperand
		 * @return the corresponding Instruction
		 */
		Instruction *getInstruction (void) const;

		/**
		 * @brief Update unrolling (if applicable)
		 * @param iterations number of iterations
		 */
		void updateUnrolling (int iterations);

		/**
		 * @brief Set if this Induction is the last one to be displayed
		 * @param last the statement of the lastInduction variable
		 */
		void setLastInduction (bool last);

		/**
		 * @brief Get if this Operand is the last one to be displayed
		 * @return the statement
		 */
		int getLastInduction (void) const;

		/**
		 * @brief Get the orig stride
		 * @return int value of origStride
		 */
		int getOrigStride (void) const;

		/**
		 * @brief Set if this Induction is displayed 
		 * @param val the value to set
		 */
		void setNoGenerated (bool val);

		/**
		 * @brief Get if this induction will be displayed
		 * @return the value of noGenrated 
		 */
		int getNoGenerated (void) const;

		/**
		 * @brief compare this Operand to another one
		 * @param op the Operand we wish to compare to
		 * @return whether or not op is similar to this one
		 */
		virtual bool isSimilar (const Operand *op) const;
};
#endif
