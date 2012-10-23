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
 @file PassEngine.cpp
 @brief The PassEngine class is in this file
 */

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>

#include "Description.h"
#include "Kernel.h"
#include "Logging.h"
#include "Pass.h"
#include "PassElement.h"
#include "PassEngine.h"

PassEngine::PassEngine (void)
{
	memset (&gateInfo, 0, sizeof(gateInfo));
}

PassEngine::~PassEngine (void)
{
	//Delete it
	for (std::list<Pass *>::iterator it = listPasses.begin (); it != listPasses.end (); it++)
	{
		Pass *p = *it;
		delete p, p = NULL;
	}
	listPasses.clear ();
}

bool PassEngine::addPass (Pass *newPass)
{
	//Check if it doesn't exist
	for (std::list<Pass *>::iterator it = listPasses.begin (); it != listPasses.end (); it++)
	{
		Pass *p = *it;

		if ((newPass == p) || (newPass->getName () == p->getName ()))
		{
			Logging::log (1, "Warning: The pass: '", newPass->getName ().c_str (), "' already exists", NULL);
			return false;
		}
	}

	listPasses.push_back (newPass);

	return true;
}

std::list<Pass*> &PassEngine::getListPasses (void)
{
	return listPasses;
}

bool PassEngine::addPassAfter (const std::string &after, Pass *newPass)
{
	bool exists = false;
	for (std::list<Pass *>::iterator it = listPasses.begin (); it != listPasses.end (); it++)
	{
		Pass *p = *it;
		assert (p);

		//Check current element
		if (after.compare (p->getName ()) == 0)
		{
			//We want to insert after
			it++;
			listPasses.insert (it, newPass);
			exists = true;
			break;
		}
	}

	//If we got here, add to the end
	if (exists == false)
	{
		Logging::log (1, "Warning: The pass '", after.c_str (), "' doesn't exist", NULL);
		listPasses.push_back (newPass);
	}

	return true;
}

bool PassEngine::replacePass (const std::string &name, Pass *newPass)
{
	bool exists = false;
	for (std::list<Pass *>::iterator it = listPasses.begin (); it != listPasses.end (); it++)
	{
		Pass *p = *it;
		assert (p);

		//Check current element
		if (name.compare (p->getName ()) == 0)
		{
			*it = newPass;
			delete p, p = NULL;
			exists = true;
			break;
		}
	}

	if (exists == false)
	{
		Logging::log (1, "Warning: The pass '", name.c_str (), "' doesn't exist", NULL);
	}

	return exists;
}

bool PassEngine::addPassBefore (const std::string &before, Pass *newPass)
{
	bool exists = false;
	for (std::list<Pass *>::iterator it = listPasses.begin (); it != listPasses.end (); it++)
	{
		Pass *p = *it;
		assert (p);

		//Check current element
		if (before.compare (p->getName ()) == 0)
		{
			//We want to insert before 
			listPasses.insert (it, newPass);
			exists = true;
			break;
		}
	}

	//If we got here, add to the beginning
	if (exists == false)
	{
		Logging::log (1, "Warning: The pass '", before.c_str (), "' doesn't exist", NULL);
		listPasses.push_front (newPass);
	}

	return true;
}

bool PassEngine::removePass (const std::string &name)
{
	bool exists = false;
	for (std::list<Pass *>::iterator it = listPasses.begin (); it != listPasses.end (); it++)
	{
		Pass *p = *it;
		assert (p);

		//Check current element
		if (name.compare (p->getName ()) == 0)
		{
			listPasses.remove (p);
			delete p, p = NULL;
			exists = true;
			break;
		}
	}

	if (exists == false)
	{
		Logging::log (1, "Warning: The pass '", name.c_str (), "' doesn't exist", NULL);
	}

	return exists;
}

bool PassEngine::setPluginGate (const SPluginGateInfo &info)
{
	gateInfo = info;
	return true;
}

void PassEngine::drivePasses (Description *description)
{
	Logging::log (0, "PassEngine is starting", NULL);

	//We have nothing to do if size is 0
	if (listPasses.size () == 0)
	{
		Logging::log (1, "Warning: PassEngine has no passes, it is stopping", NULL);
		return;
	}

	//Create the first element
	Pass *start = listPasses.front ();

	PassElement *newElement = new PassElement ();
	newElement->setPass (start);

	std::vector<PassElement*> elements;
	elements.push_back (newElement);

	unsigned int progress = 0;

	while (elements.size () > 0)
	{
		std::ostringstream output;

		//Get current pass element
		PassElement *current = elements.back ();

		//Paranoid
		assert (current != NULL);

		if (description->getVerbose () == true)
		{
			//This is ugly but it works for our purpose: get filename
			static int cnt = 0;
			cnt++;
			output << "output_" << cnt << "_" << current->getPassName () << ".out";

			//Set the logging
			Logging::setLogFile (output.str ());
		}

		//Log something, this will empty the file
		Logging::log (0, "Starting new kernel pass", NULL);

		if (description->getVerbose () == true)
		{
			//Ok now debug the kernel, before the pass
			debug (output.str (), current, description);
		}

		Logging::log (0, "\nNow execute the pass", current->getPassName ().c_str (), NULL);

		Pass *nextPass = findNextPass (current->getPassName ());

		progress++;

		fprintf (stderr, "\rProgress %d ", progress);

		//Pop the vector
		elements.pop_back ();

		//If we have a pluginGate
		bool doPass = true;

		if (gateInfo.gate != NULL)
		{
			doPass = gateInfo.gate (description, current);
		}
		else
		{
			doPass = current->gate (description, current->getKernel ());
		}

		if (doPass == true)
		{
			//Do the work of the pass
			std::vector<PassElement*> *todo = current->entry (description);
			bool foundCurrent = false;

			//If anything to do
			if (todo != NULL)
			{
				int size = todo->size () - 1;

				//Add todo for every one
				while (size >= 0)
				{
					PassElement *pe = (*todo)[size];
					size--;

					//BEFORE ANYTHING, let's set back the induction variables for all, this is important since things might have gotten switched up
					if (pe->getKernel () != NULL)
						pe->getKernel ()->handleInductionVariables ();

					//Check if current
					if (pe == current)
						foundCurrent = true;

					//If the pass element doesn't have a pass, we define it as being the next one
					if (pe->getPass () == NULL)
					{
						pe->setPass (nextPass);
					}

					std::ostringstream oss;
					oss << current->getKernel ();

					//Log information
					Logging::log (0, "\n\nGot a new kernel output ", oss.str ().c_str (), ":\n\n", NULL);

					if (description->getVerbose () == true)
					{
						//Debug this one
						debug (output.str (), pe, description);
					}

					//define the max number of benchmarks
					if (description->getMaxBenchmarks () > elements.size ())
					{
						elements.push_back (pe);
					}
					else
					{
						//don't push the current kernel
						delete pe;
					}
				}

				//Empty and delete
				todo->clear ();
				delete todo, todo = NULL;
			}

			//If we didn't find the current element
			if (foundCurrent == false)
			{
				//Delete the current element
				delete current, current = NULL;
			}
		}
		else
		{
			//Otherwise current is to go to the next element
			current->setPass (nextPass);

			//Add it to job element if there is a pass
			if (current->getPass () != NULL)
			{
				std::ostringstream oss;
				oss << current->getKernel ();

				//Log information
				Logging::log (0, "\n\nGot a new kernel output:\n\n", oss.str ().c_str (), NULL);
				if (description->getVerbose () == true)
				{
					//Debug this one
					debug (output.str (), current, description);
				}

				elements.push_back (current);
			}
			else
			{
				//It's over
				delete current, current = NULL;
			}
		}

		if (description->getVerbose () == true)
		{
			//Set the logging back
			Logging::setLogFile ("Log.txt", false);
		}
	}

	//Now we delete all the remaining elements
	for (std::vector<PassElement*>::const_iterator it = elements.begin (); it != elements.end (); it++)
	{
		PassElement *pe = *it;
		delete pe, pe = NULL;
	}

	Logging::log (0, "PassEngine is stopping", NULL);
}

Pass *PassEngine::findNextPass (const std::string &name)
{
	for (std::list<Pass*>::const_iterator it = listPasses.begin (); it != listPasses.end (); it++)
	{
		Pass *pass = *it;

		if (name.compare (pass->getName ()) == 0)
		{
			it++;

			if (it == listPasses.end ())
				return NULL;

			Pass *res = *it;
			return res;
		}
	}
	return NULL;
}

void PassEngine::debug (const std::string &fileName, PassElement *pe, Description *desc) const
{
	std::ofstream out (fileName.c_str (), std::ios::app);

	assert (out.is_open () == true);

	Kernel *kernel = pe->getKernel ();

	if (kernel != NULL)
	{
		kernel->debug (out, desc, 0);
	}

	out.close ();
}
