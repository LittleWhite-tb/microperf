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
 @file Description.cpp
 @brief The Description class is in this file
 */

#include <sstream>

#include "Description.h"
#include "Logging.h"
#include "DescriptionXML.h"
#include "HWInformation.h"
#include "Kernel.h"

Description::Description (void)
{
	Logging::log (0, "Loading description", NULL);
	init ();
	Logging::log (0, "Loaded description", NULL);
}

Description::~Description (void)
{
	delete kernel, kernel = NULL;
	delete hwInformation, hwInformation = NULL;
}

void Description::init (void)
{
	kernel = new Kernel ();
	separator = ", ";
	seed = 0;
	maxBenchmarks = ~0;
	verbose = false;
	asmVolatile = false;
	fileCnt = 0;

	outputMotif = "output/example";
	outputExtension = ".s";
	hwInformation = NULL;

	prologue = "";
	epilogue = "";

	c_code = false;
}

const Kernel *Description::getKernel (void) const
{
	return kernel;
}

void Description::addSeparator (std::string &s) const
{
	s += separator;
}

void Description::setSeparator (const std::string &s)
{
	separator = s;
}

void Description::parse (const std::string &filePath)
{
	(void) filePath;
	Logging::log (2, "Error parsing", NULL);
}

std::string Description::getOutputFileName (void)
{
	std::ostringstream oss;

	//First the motif
	oss << getOutputMotif ();

	//We want leading 0s, let's say 4 digits minimum
	//Count digits
	int nbr = fileCnt;
	int digits = 1;
	while (nbr >= 10)
	{
		nbr /= 10;
		digits++;
	}

	//Now check digits
	while (digits < 5)
	{
		digits++;
		oss << "0";
	}

	//Now the number
	oss << fileCnt;

	//Increment counter
	fileCnt++;

	return oss.str ();
}

void Description::setFileCounterStart (int nbr)
{
	fileCnt = nbr;
}

const std::string &Description::getOutputExtension (void) const
{
	return outputExtension;
}

void Description::setOutputExtension (const std::string &s)
{
	outputExtension = s;
}

const std::string &Description::getOutputMotif (void) const
{
	return outputMotif;
}

void Description::setOutputMotif (const std::string &s)
{
	outputMotif = s;
}

const int &Description::getSeed (void) const
{
	return seed;
}

void Description::setSeed (int seedValue)
{
	seed = seedValue;
}

const HWInformation *Description::getHWInformation (void) const
{
	return hwInformation;
}

HWInformation *Description::getModifHWInformation (void)
{
	return hwInformation;
}

unsigned long int Description::getMaxBenchmarks (void) const
{
	return maxBenchmarks;
}

void Description::setMaxBenchmarks (unsigned long int maxBench)
{
	maxBenchmarks = maxBench;
}

bool Description::getVerbose (void) const
{
	return verbose;
}

void Description::setVerbose (bool isVerbose)
{
	verbose = isVerbose;
}

bool Description::getAsmVolatile (void) const
{
	return asmVolatile;
}

void Description::setAsmVolatile (bool isAsmVolatile)
{
	asmVolatile = isAsmVolatile;
}

void Description::setPrologue (const std::string &name)
{
	prologue = name;
}

const std::string &Description::getPrologue (void) const
{
	return prologue;
}

void Description::setEpilogue (const std::string &name)
{
	epilogue = name;
}

const std::string &Description::getEpilogue (void) const
{
	return epilogue;
}

void Description::setC_code (bool c)
{
	c_code = c;
}

bool Description::getC_code (void) const
{
	return c_code;
}

