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
 @file Statement.h
 @brief The Statement class header is in this file
 */

#ifndef H_STATEMENT
#define H_STATEMENT

#include <string>
#include <fstream>

//Advanced declaration
class Description;

/**
 * @class Statement
 * @brief Statement is the instructions class
 */
class Statement
{
	protected:
		unsigned int minRepeat; 		/**< @brief How many times can we repeat this statement (min value) */
		unsigned int maxRepeat; 		/**< @brief How many times can we repeat this statement (max value) */
		unsigned int progressRepeat; 	/**< @brief step of progress repetition */
		std::string name; 				/**< @brief name of the statement */
		std::string linked; 			/**< @brief the linked name of the statement */

		const Statement *origin; 	/**< @brief Statement that we originated from */
		unsigned int color; 		/**< @brief Color of Statement */

		bool fileNaming; /**< @brief info about the Statement, displayed in the file name */

		/**
		 * @brief Set tabulations to string
		 * @param s the string
		 * @param tab the tabulation we wish to obtain
		 */
		void setTabulations (std::string &s, unsigned int tab) const;

	public:
		/**
		 * @brief Constructor
		 */
		Statement (void);

		/**
		 * @brief Destructor
		 */
		virtual ~Statement ();

		/**
		 * @brief Copy function: must set the origin member 
		 * @return returns a copy of the Statement
		 */
		virtual Statement* copy (void) const = 0;

		/**
		 * @brief Copy the information of the statement    
		 * @param stmt The statement
		 */
		virtual void copyInformation (const Statement *stmt) = 0;

		/**
		 * @brief Get a String
		 * @param s the string we wish to fill
		 * @param desc the description
		 * @param tab the tabulation (default: 1)
		 * @return whether or not this succeeded
		 */
		virtual bool getString (std::string &s, const Description *desc, unsigned int tab = 1) const = 0;

		/**
		 * @brief Set Unroll information for this Instruction
		 * @param nbrIterationsAdvance number of iterations in advance
		 */
		virtual void updateUnrollInformation (int nbrIterationsAdvance) = 0;

		/**
		 * @brief Update RegisterOperand's names
		 * @param nbrIterationsAdvance number of iterations in advance
		 */
		virtual void updateRegisterName (int nbrIterationsAdvance) = 0;

		/**
		 * @brief compare this Statement to another one
		 * @param stmt the statemen we wish to compare to
		 * @return whether or not stmt is similar to this one
		 */
		virtual bool isSimilar (const Statement *stmt) const = 0;

		/**
		 * @brief Set the minimal repetition
		 * @param min the value we want
		 */
		void setMinRepeat (unsigned int min);

		/**
		 * @brief Get the minimal repetition
		 * @return the value we want
		 */
		unsigned int getMinRepeat (void) const;

		/**
		 * @brief Set the maximal repetition
		 * @param max the value we want
		 */
		void setMaxRepeat (unsigned int max);

		/**
		 * @brief Get the maximal repetition
		 * @return the value we want
		 */
		unsigned int getMaxRepeat (void) const;

		/**
		 * @brief Set the progress repetition
		 * @param progress the value we want
		 */
		void setProgressRepeat (unsigned int progress);

		/**
		 * @brief Get the progress repetition
		 * @return the value we want
		 */
		unsigned int getProgressRepeat (void) const;

		/**
		 * @brief Get the name of the statement
		 * return string the name of the statement  
		 */
		const std::string &getName (void) const;

		/**
		 * @brief Set the name of the statement
		 * param str the name of the statement  
		 */
		void setName (std::string str);

		/**
		 * @brief Get the linked name of the statement
		 * return string the linked name of the statement  
		 */
		const std::string &getLinked (void) const;

		/**
		 * @brief Set the linked name of the statement
		 * param str the linked name of the statement  
		 */
		void setLinked (std::string str);

		/** 
		 * @brief Do we wish add the additional information in file names
		 * @param fileName the value of fileNaming
		 */
		void setFileNaming (bool fileName);

		/** 
		 * @brief Do we wish to add additional information in file names
		 * @return whether or not we want to add additional information in file names
		 */
		bool getFileNaming (void) const;

		/**
		 * @brief Set the color
		 * @param v the value we want
		 */
		void setColor (unsigned int v);

		/**
		 * @brief Get the color
		 * @return the value we want
		 */
		unsigned int getColor (void) const;

		/**
		 * @brief Clear color
		 */
		virtual void clearColor (void);

		/**
		 * @brief Debug function
		 * @param outStream the output stream
		 * @param desc the Description
		 * @param tab tabulation
		 */
		virtual void debug (std::ofstream &outStream, const Description *desc, int tab) const = 0;

		/**
		 * @brief Find the Statement that is a copy of the original
		 * @param origin the original Statement
		 * @return the Statement that is the copy of Statement
		 */
		virtual Statement *findOrigin (const Statement *origin);
};

#endif
