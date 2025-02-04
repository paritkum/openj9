
/*******************************************************************************
 * Copyright IBM Corp. and others 1991
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] https://openjdk.org/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#include "j9cfg.h"

#include "VerboseTraceOutput.hpp"
#include "VerboseEvent.hpp"
#include "GCExtensions.hpp"
#include "VerboseBuffer.hpp"
#include "EnvironmentBase.hpp"
#include <string.h>

#define _UTE_STATIC_                  
#include "ut_j9vgc.h"

#define INPUT_STRING_SIZE 236
#define INDENT_SPACER "  "

/**
 * Formats and output data.
 * Called by events, formats the data passed into verbose form and outputs it.
 * @param indent The current level of indentation.
 * @param format String to format the data into.
 */
void
MM_VerboseTraceOutput::formatAndOutput(J9VMThread *vmThread, UDATA indent, const char *format, ...)
{
	char inputString[INPUT_STRING_SIZE];
	char localBuf[256];
	va_list args;
	
	PORT_ACCESS_FROM_VMC(vmThread);
	
	localBuf[0] = '\0';
	for(UDATA i=0; i < indent; i++) {
		strcat(localBuf, INDENT_SPACER);
	}
	
	va_start(args, format);
	j9str_vprintf(inputString, INPUT_STRING_SIZE, format, args);
	va_end(args);
	
	strcat(localBuf, inputString);
	
	if(!_componentLoaded) {
		/* If this is the first time in, we have to load the j9vgc trace component.
		 * Can't do it at startup because the trace engine initializes too late */
		UT_MODULE_LOADED(J9_UTINTERFACE_FROM_VM(vmThread->javaVM));
		_componentLoaded = true;
	}
	
	/* Call the tracepoint that outputs the line of verbosegc */
	Trc_VGC_Verbosegc(vmThread, localBuf);
}

/**
 * Create a new MM_VerboseTraceOutput instance.
 * @return Pointer to the new MM_VerboseTraceOutput.
 */
MM_VerboseTraceOutput *
MM_VerboseTraceOutput::newInstance(MM_EnvironmentBase *env)
{
	MM_GCExtensions *extensions = MM_GCExtensions::getExtensions(env->getOmrVM());
	
	MM_VerboseTraceOutput *agent = (MM_VerboseTraceOutput *)extensions->getForge()->allocate(sizeof(MM_VerboseTraceOutput), MM_AllocationCategory::DIAGNOSTIC, J9_GET_CALLSITE());
	if (agent) {
		new(agent) MM_VerboseTraceOutput(env);
		if(!agent->initialize(env)){
			agent->kill(env);
			agent = NULL;
		}
	}
	return agent;
}

/**
 * Initializes the MM_VerboseTraceOutput instance.
 */
bool
MM_VerboseTraceOutput::initialize(MM_EnvironmentBase *env)
{
	return true;
}

/**
 */
void
MM_VerboseTraceOutput::endOfCycle(J9VMThread *vmThread)
{
}

/**
 * Closes the agents output stream.
 */
void
MM_VerboseTraceOutput::closeStream(MM_EnvironmentBase *env)
{
}
