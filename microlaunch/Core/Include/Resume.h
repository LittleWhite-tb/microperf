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

#ifndef H_resume
#define H_resume

#define RESUME_PATH "resumeData/resumeData"
#define RESUME_PATH_COUNTERS "resumeData/ResumeCounters"
#define RESUME_PATH_JOB_DONE "resumeData/ResumeJobDone"

/*
 * @brief Loads the execution parametres from the checkpoint file specified in desc
 * @param desc the sDescription structure to use and modify
 * @return -1 in case of error, 0 otherwise
 */
int resumeLoad(struct sDescription *desc);

/*
 * @brief Saves the execution parametres to the checkpoint file specified in desc
 * @param desc the sDescription structure to save
 * @return -1 in case of error, 0 otherwise
 */
int resumeSave(struct sDescription *desc);

/*
 * @brief Loads the execution variables from the checkpoint file specified in desc
 * @param desc the sDescription structure to use and modify
 * @return -1 in case of error, 0 otherwise
 */
int resumeLoadCounters(struct sDescription *desc);

/*
 * @brief Saves the execution variables to the checkpoint file specified in desc
 * @param desc the sDescription structure to save
 * @return -1 in case of error, 0 otherwise
 */
int resumeSaveCounters(struct sDescription *desc);

/*
 * @brief Creates a file in the output folder flagging a task as being done
 * @param desc the sDescription structure of which the task was completed
 */
void resumeSignalJobDone(struct sDescription *desc);

/*
 * @brief Modifies an iterator if the current branch of the execution tree is being resumed
 * @param desc the sDescription structure to use
 * @param val the value to init the counter to if resuming
 * @return 1 if the value was initialized as the branch is being resumed, 0 otherwise
 */
int resumeInitCounter (unsigned *ptr, unsigned val);

/*
 * @brief Modifies an array if the current branch of the execution tree is being resumed
 * @param desc the sDescription structure to use
 * @param src the values to init the array to if resuming
 * @return 1 if the value was initialized as the branch is being resumed, 0 otherwise
 */
int resumeInitData (void *dest, void *src, size_t size);

/*
 * @brief Specifies the current branch has to be resumed
 */
void resumeEnableResuming (void);

/*
 * @brief Specifies the current branch has NOT to be resumed
 */
void resumeDisableResuming (void);

/*
 * @brief Tells whether or not the current branch is being resumed
 * @return 1 if resuming, 0 otherwise
 */
int resumeIsResuming (void);

#endif
