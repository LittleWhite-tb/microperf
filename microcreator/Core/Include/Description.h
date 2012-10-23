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
 @file Description.h
 @brief The Description class header is in this file
 */

#ifndef H_DESCRIPTION
#define H_DESCRIPTION

#include <string>
#include <map>
#include <vector>

//Advanced declaration
class HWInformation;
class Kernel;

/**
 * @class Description
 * @brief The Description of the input
 */
class Description
{
	protected:
		Kernel *kernel; /**< @brief Main Kernel defined in the Description file */
		HWInformation *hwInformation; /**< @brief Hardware information */

		int seed; /**< @brief seed for the ranmakedom generation */

		unsigned long int maxBenchmarks; /**< @brief Maximum benchmarks generated */

		std::string separator; /**< @brief The separator between operands */
		std::string outputMotif; /**< @brief Motif of the output files */
		std::string outputExtension; /**< @brief Motif of the output files */

		bool verbose; /**< @brief Do we want the output to be verbose or not */
		bool asmVolatile; /**< @brief Add asm volatile the output */

		std::string prologue; /**< @brief The name of the prologue file */
		std::string epilogue; /**< @brief The name of the epilogue file */

		int fileCnt; /**< @brief File counter number */

		bool c_code; /**< @brief is it a c code or not? */

	public:
		/**
		 * @brief Constructor
		 */
		Description (void);

		/**
		 * @brief Destructor
		 */
		virtual ~Description (void);

		/**
		 * @brief Initialization function
		 */
		void init (void);

		/**
		 * @brief Get the pool
		 */
		const Kernel *getKernel (void) const;

		/**
		 * @brief Add the separator
		 * @param s the string we wish to add the separator
		 */
		void addSeparator (std::string &s) const;

		/**
		 * @brief Set the separator
		 * @param s the new separator
		 */
		void setSeparator (const std::string &s);

		/**
		 * @brief Parsing the description file
		 */
		virtual void parse (const std::string &filePath);

		/**
		 * @brief Get the output file name
		 * @return the name of the output file
		 */
		std::string getOutputFileName (void);

		/**
		 * @brief Get output motif
		 * @return the output motif
		 */
		const std::string &getOutputMotif (void) const;

		/**
		 * @brief Set the output motif
		 * @param s the output motif
		 */
		void setOutputMotif (const std::string &s);

		/**
		 * @brief Get output extension
		 * @return the output extension
		 */
		const std::string &getOutputExtension (void) const;

		/**
		 * @brief Set the output extension
		 * @param s the output extension
		 */
		void setOutputExtension (const std::string &s);

		/**
		 * @brief Get the seed
		 * @return the seed value
		 */
		const int &getSeed (void) const;

		/**
		 * @brief Set the seed
		 * @param seedValue the seed value
		 */
		void setSeed (int seedValue);

		/**
		 * @brief Get the hardware information 
		 */
		const HWInformation *getHWInformation (void) const;

		/**
		 * @brief Get the hardware information without const
		 */
		HWInformation *getModifHWInformation (void);

		/**
		 * @brief Get the max number of benchmarks
		 * @return the max number of benchmarks
		 */
		unsigned long int getMaxBenchmarks (void) const;

		/**
		 * @brief Set the max Benchmark number
		 * @param maxBench the max Benchmarks number
		 */
		void setMaxBenchmarks (unsigned long int maxBench);

		/**
		 * @brief Do we want a verbose execution or not?
		 */
		bool getVerbose (void) const;

		/**
		 * @brief Set the verbose
		 * @param isVerbose the value of verbose
		 */
		void setVerbose (bool isVerbose);

		/**
		 * @brief Do we add asm to the instructions?
		 */
		bool getAsmVolatile (void) const;

		/**
		 * @brief Set the AsmVolatile
		 * @param isAsmVolatile the value of asmVolatile
		 */
		void setAsmVolatile (bool isAsmVolatile);

		/**
		 * @brief Set the prologue name 
		 * @param name the prologue name
		 */
		void setPrologue (const std::string &name);

		/**
		 * @brief Get prologue file name
		 * @return the prologue file name
		 */
		const std::string &getPrologue (void) const;

		/**
		 * @brief Set the epilogue name
		 * @param name the epilogue name
		 */
		void setEpilogue (const std::string &name);

		/**
		 * @brief Get epilogue file name
		 * @return the epilogue file name
		 */
		const std::string &getEpilogue (void) const;

		/**
		 * @brief Set the c_code
		 * @param c the value of c_code
		 */
		void setC_code (bool c);

		/**
		 * @brief Do we generate a C code
		 */
		bool getC_code (void) const;

		/**
		 * @brief Set file counter start
		 * @param nbr the file counter start
		 */
		void setFileCounterStart (int nbr);
};

#endif
