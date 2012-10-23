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
 @file Kernel.h
 @brief The Kernel class header is in this file
 */

#ifndef H_KERNEL
#define H_KERNEL

//For inheritance
#include "Statement.h"
#include "LoopInfo.h"
#include "ScheduleInfo.h"

#include <map>
#include <string>
#include <vector>

//Advanced declaration
class InductionOperand;

/**
 * @class Kernel
 * @brief The Kernel of the input
 */
class Kernel : public Statement
{
	protected:
		std::vector<Statement *> statements; 					/**< @brief Current statement list */
		std::map<std::string, InductionOperand*> inductions; 	/**< @brief Current induction list */
		bool randomize; 										/**< @brief if the kernel can be randomized */
		bool combination; 										/**< @brief combination value for this kernel */

		int minUnroll; 		/**< @brief min value of the unroll */
		int maxUnroll; 		/**< @brief max value of the unroll */
		int progressUnroll; /**< @brief progress value of the unroll */
		int actualUnroll; 	/**< @brief value of the actual unroll */

		std::string labelName; 			/**< @brief branch_information label */
		std::string labelInstruction; 	/**< @brief branch_information instruction */

		bool MDLBasicSchedule; /**< @brief Do we want to schedule the MDL instructions */
		unsigned int nbBundle; /**< @brief The number of bundles (MDL scheduling) in the MDL output */

		int minBundle; 		/**< @brief min value of the nbBundle */
		int maxBundle; 		/**< @brief max value of the nbBundle */
		int progressBundle; /**< @brief progress value of the nbBundle */

		std::vector<SLoopInfo> loopInfo; /**< @brief information about the loop */

		bool ompOption; 		/**< @brief Do we add the omp clause */
		bool ompParallelFor; 	/**< @brief Do we add the omp parallel for clause instead of omp for */

		SScheduleInfo scheduleInfo; /**< @brief information about the schedule of the omp options */

		std::vector<std::string> sharedVariables; 		/**< @brief Current shared variable list */
		std::vector<std::string> privateVariables; 		/**< @brief Current private variable list */
		std::vector<std::string> firstprivateVariables; /**< @brief Current firstprivate variable list */
		std::vector<std::string> lastprivateVariables; 	/**< @brief Current lastprivate variable list */
		std::vector<std::string> usedVariables; 		/**< @brief The list of the used variables */

		int alignment; /**< @brief value of the alignment */

		/**
		 * @brief Initialization function
		 */
		void init (void);

	public:
		/**
		 * @brief Constructor
		 */
		Kernel (void);

		/**
		 * @brief Destructor
		 */
		virtual ~Kernel (void);

		/**
		 * @brief Clear the Statement vector
		 */
		void clearStatements (void);

		/**
		 * @brief Clear the InductionOperand vector
		 */
		void clearInductionVariables (void);

		/**
		 * @brief Get number of Statement
		 * @return the number of Statement
		 */
		unsigned int getNbrStatements (void) const;

		/**
		 * @brief Get an Statement
		 * @param idx the index to the Statement
		 * @return returns an Statement, NULL if index is invalid
		 */
		const Statement *getStatement (unsigned int idx) const;

		/**
		 * @brief Get an Statement without the const
		 * @param idx the index to the Statement
		 * @return returns an Statement without the const, NULL if index is invalid
		 */
		Statement *getModifiableStatement (unsigned int idx);

		/**
		 * @brief Add an instruction
		 * @param inst the Statement we wish to add, this instruction is copied, therefore the original is not touched and not deleted
		 */
		void addStatement (const Statement *inst);

		/**
		 * @brief Add an instruction
		 * @param inst the Statement we wish to add, this instruction is NOT copied, therefore the original must be forgotten and NOT deleted by caller
		 */
		void addStatement (Statement *inst);

		/**
		 * @brief Add an instruction
		 * @param inst the Statement we wish to add
		 * @param idx the index where we wish insert Statement
		 */
		void addStatementAt (Statement *inst, unsigned int idx);

		/**
		 * @brief replace an instruction
		 * @param inst the Statement we wish to replace, this instruction is copied, therefore the original is not touched and not deleted
		 * @param idx the index we wish to return, if not valid, nothing is done
		 */
		//void replaceStatement (const Statement *inst, unsigned int idx);

		/**
		 * @brief replace an instruction
		 * @param inst the Statement we wish to replace, this instruction is NOT copied, therefore the original must be forgotten and NOT deleted by caller
		 * @param idx the index we wish to return, if not valid, nothing is done
		 * @param free if we did or not delete the replaced statement (default value is true)
		 */
		void replaceStatement (Statement *inst, unsigned int idx, bool free = true);

		/**
		 * @brief replace an instruction, this is recursive, it will look into the sub-Kernels
		 * @param inst the Statement we wish to replace, this instruction is copied, therefore the original is not touched and not deleted
		 * @param origin is the Statement we are looking for, if not found, nothing is done
		 * @return whether we found inst or not
		 */
		bool replaceStatement (const Statement *inst, Statement *origin);

		/**
		 * @brief replace an instruction, this is recursive, it will look into the sub-Kernels
		 * @param inst the Statement we wish to replace, this instruction is NOT copied, therefore the original must be forgotten and NOT deleted by caller
		 * @param origin is the Statement we are looking for, if not found, nothing is done
		 * @return whether we found inst or not
		 */
		bool replaceStatement (Statement *inst, Statement *origin);

		/**
		 * @brief Copy the Kernel
		 * @param kernel the Kernel that we want to append to ours
		 */
		void append (const Kernel *kernel);

		/**
		 * @brief Get an InductionOperand variable
		 * @param name the name of the variable
		 * @return the InductionOperand variable (NULL if not found)
		 */
		InductionOperand *getInduction (const std::string &name) const;

		/** 
		 * @brief Do we wish to randomize this kernel ?
		 * @param b the value we wish to set for randomize
		 */
		void setRandomize (bool b);

		/** 
		 * @brief Do we wish to randomize this kernel ?
		 * @return whether or not we want the instructions in this Kernel to be randomized
		 */
		bool getRandomize (void) const;

		/** 
		 * @brief Do we wish to have combinations or permutations for this kernel ?
		 * @param b the value we wish to set for combination
		 */
		void setCombination (bool b);

		/** 
		 * @brief Do we wish to have combinations or permutations for this kernel ?
		 * @return whether or not we want the instructions to be permuted or used as combinations
		 */
		bool getCombination (void) const;

		/**
		 * @brief Get an InductionOperand variable
		 * @param i the index to the variable
		 * @return the InductionOperand variable (NULL if not found)
		 */
		const InductionOperand *getInduction (unsigned int i) const;

		/**
		 * @brief Get an InductionOperand variable
		 * @param i the index to the variable
		 * @return the InductionOperand variable (NULL if not found)
		 */
		InductionOperand *getModifiableInduction (unsigned int i);

		/**
		 * @brief Add an InductionOperand
		 * @param induction the InductionOperand
		 */
		void addInduction (InductionOperand *induction);

		/**
		 * @brief Add an InductionOperand
		 * @param reg register name
		 * @param induction the InductionOperand
		 */
		void addInduction (const std::string &reg, InductionOperand *induction);

		/**
		 * @brief Copy the Induction map
		 * @param inductions the Induction map
		 */
		void copyInductionVariables (const std::map<std::string, InductionOperand*> &inductions);

		/**
		 * @brief Replace the Induction map
		 * @param inductions we wish to use now
		 */
		void replaceInductionVariables (std::vector<InductionOperand*> &inductions);

		/**
		 * @brief Handle the InductionOperand linked information
		 * @param copy the Induction vector we are basing our linked information from
		 */
		void handleOurInductionLinks (std::vector<const InductionOperand*> &copy);

		/**
		 * @brief Handle the InductionOperand linked information
		 * @param copy the Induction vector we are modifying to reflect our links
		 */
		void handleTheirInductionLinks (std::vector<InductionOperand*> &copy);

		/**
		 * @brief Handle our Operands to link them up to the induction variables
		 */
		void handleInductionVariables (void);

		/**
		 * @brief Update the Induction variables with unrolling information
		 * @param iterations number of iterations we unrolled
		 */
		void updateInductionUnrolling (int iterations);

		/**
		 * @brief Get number of Induction
		 * @return the number of Induction
		 */
		unsigned int getNbrInductions (void) const;

		/**
		 * @brief Get the min value of the unroll
		 * @return the minimal value of the unroll
		 */
		const int &getMinUnroll (void) const;

		/**
		 * @brief set the min value of the unroll
		 * @param min the minimal value of the unroll
		 */
		void setMinUnroll (int min);

		/**
		 * @brief Get the max value of the unroll
		 * @return the maximal value of the unroll
		 */
		const int &getMaxUnroll (void) const;

		/**
		 * @brief set the max value of the unroll
		 * @param max the maximal value of the unroll
		 */
		void setMaxUnroll (int max);

		/**
		 * @brief Get the progression value of the unroll
		 * @return the progession value of the unroll
		 */
		const int &getProgressUnroll (void) const;

		/**
		 * @brief Set the progression value of the unroll
		 * @return progress the progession value of the unroll
		 */
		void setProgressUnroll (int progress);

		/**
		 * @brief Get the actual value of the unroll
		 * @return the actual value of the unroll 
		 */
		int getActualUnroll (void) const;

		/**
		 * @brief set the actual value of the unroll
		 * @param value the actual value of the unroll
		 */
		void setActualUnroll (int value);

		/**
		 * @brief Get the min value of the nbBundle
		 * @return the minimal value of the nbBundle 
		 */
		int getMinBundle (void) const;

		/**
		 * @brief set the min value of the nbBundle
		 * @param min the minimal value of the nbBundle
		 */
		void setMinBundle (int min);

		/**
		 * @brief Get the max value of the nbBundle
		 * @return the maximal value of the nbBundle
		 */
		int getMaxBundle (void) const;

		/**
		 * @brief set the max value of the nbBundle
		 * @param max the maximal value of the nbBundle
		 */
		void setMaxBundle (int max);

		/**
		 * @brief Get the progression value of the nbBundle
		 * @return the progession value of the nbBundle
		 */
		int getProgressBundle (void) const;

		/**
		 * @brief Set the progression value of the nbBundle
		 * @return progress the progession value of the nbBundle
		 */
		void setProgressBundle (int progress);

		/**
		 * @brief Get the Label name
		 * @return std::string of the name of the label
		 */
		const std::string &getLabelName (void) const;

		/**
		 * @brief Get the Label name
		 * @return std::string of the name of the label
		 */
		void setLabelName (std::string name);

		/**
		 * @brief Get the Label instruction
		 * @return std::string of the instruction of the label
		 */
		const std::string &getLabelInstruction (void) const;

		/**
		 * @brief Get the Label instruction
		 * @return std::string of the instruction of the label
		 */
		void setLabelInstruction (const std::string instruction);

		/**
		 * @brief Get the alignment value
		 * @return the alignment value 
		 */
		int getAlignment (void) const;

		/**
		 * @brief set the alignement value
		 * @param value the alignement value
		 */
		void setAlignment (int value);

		/**
		 * @brief Get loop information
		 * @return vector of loop information
		 */
		const std::vector<SLoopInfo> &getLoopInfo (void) const;

		/**
		 * @brief Add the loop information
		 * @param info the loop information
		 */
		void addLoopInfo (SLoopInfo info);

		/**
		 * @brief Get schedule information
		 * @return information about scheduling type and size
		 */
		const SScheduleInfo &getScheduleInfo (void) const;

		/**
		 * @brief Add the schedule information
		 * @param info the schedule information
		 */
		void setScheduleInfo (const SScheduleInfo &info);

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
		 * @return the new copied Kernel
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
		 * @brief Find the Statement that is a copy of the original
		 * @param o the original Statement
		 * @return the Statement that is the copy of Statement
		 */
		virtual Statement *findOrigin (const Statement *o);

		/**
		 * @brief compare this Statement to another one
		 * @param stmt the statemen we wish to compare to
		 * @return whether or not stmt is similar to this one
		 */
		virtual bool isSimilar (const Statement *stmt) const;

		/**
		 * @brief Clear color
		 */
		virtual void clearColor (void);

		/**
		 * @brief Do we schedule the MDL instructions?
		 */
		bool getMDLBasicSchedule (void) const;

		/**
		 * @brief Set the MDLBasicSchedule
		 * @param schedule the value of MDLBasicSchedule
		 */
		void setMDLBasicSchedule (bool schedule);

		/**
		 * @brief Get the number of bundles per output
		 * @return number of bundles
		 */
		int getNbBundle (void) const;

		/**
		 * @brief Set the number of the bundle
		 * @param val the number of bundle
		 */
		void setNbBundle (int val);

		/** 
		 * @brief Do we wish to add omp parallel clause?
		 * @param omp the value we wish to set for ompOption 
		 */
		void setOmpOption (bool omp);

		/** 
		 * @brief Do we wish to add omp clause?
		 * @return whether or not we want to add the omp parallel clause 
		 */
		bool getOmpOption (void) const;

		/** 
		 * @brief Do we wish to add omp parallel for?
		 * @param omp the value we wish to set for ompParallelFor 
		 */
		void setOmpParallelFor (bool omp);

		/** 
		 * @brief Do we wish to add omp parallel clause?
		 * @return the value of ompParallelFor
		 */
		bool getOmpParallelFor (void) const;

		/**
		 * @brief Get number of shared variables
		 * @return the number of shared variables
		 */
		unsigned int getNbrSharedVariables (void) const;

		/**
		 * @brief Get a shared variable 
		 * @param idx the index to the shared variable 
		 * @return shared variable, empty string if index is invalid
		 */
		std::string &getSharedVariable (unsigned int idx);

		/**
		 * @brief Add a shared variable 
		 * @param var the shared variable we wish to add 
		 */
		void addSharedVariable (const std::string var);

		/**
		 * @brief Get number of private variables
		 * @return the number of private variables
		 */
		unsigned int getNbrPrivateVariables (void) const;

		/**
		 * @brief Get a private variable
		 * @param idx the index to private variable
		 * @return private variabe, empty string if index is invalid
		 */
		std::string &getPrivateVariable (unsigned int idx);

		/**
		 * @brief Add a private variable 
		 * @param var the private variable we wish to add 
		 */
		void addPrivateVariable (const std::string var);

		/**
		 * @brief Get number of firstprivate variables
		 * @return the number of firstprivate variables
		 */
		unsigned int getNbrFirstPrivateVariables (void) const;

		/**
		 * @brief Get a firstprivate variable
		 * @param idx the index to firstprivate variable
		 * @return firstprivate variabe, empty string if index is invalid
		 */
		std::string &getFirstPrivateVariable (unsigned int idx);

		/**
		 * @brief Add a fisrtprivate variable 
		 * @param var the firstprivate variable we wish to add 
		 */
		void addFirstPrivateVariable (const std::string var);

		/**
		 * @brief Get number of lastprivate variables
		 * @return the number of lastprivate variables
		 */
		unsigned int getNbrLastPrivateVariables (void) const;

		/**
		 * @brief Get a lastprivate variable
		 * @param idx the index to lastprivate variable
		 * @return lastprivate variabe, empty string if index is invalid
		 */
		std::string &getLastPrivateVariable (unsigned int idx);

		/**
		 * @brief Add a lastprivate variable 
		 * @param var the lastprivate variable we wish to add 
		 */
		void addLastPrivateVariable (const std::string var);

		/**
		 * @brief Get number of the used variables
		 * @return the number of used variables
		 */
		unsigned int getNbrUsedVariables (void) const;

		/**
		 * @brief Get a used variable
		 * @param idx the index to used variable
		 * @return used variable, empty string if index is invalid
		 */
		std::string &getUsedVariable (unsigned int idx);

		/**
		 * @brief Add a used variable 
		 * @param var the used variable we wish to add 
		 */
		void addUsedVariable (const std::string var);

		/**
		 * @brief Return the list of used variables
		 * @return The list of used variables
		 */
		std::vector<std::string> &getListUsedVariables (void);
};

#endif
