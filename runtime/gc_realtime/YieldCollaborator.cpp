/*******************************************************************************
 * Copyright IBM Corp. and others 2001
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

#include "YieldCollaborator.hpp"

#include "Task.hpp"

void
MM_YieldCollaborator::resumeWorkersFromYield(MM_EnvironmentBase *env)
{
	omrthread_monitor_enter(*_mutex);
	_yieldCount = 0;
	_yieldIndex += 1;
	_resumeEvent = fromYield;
	omrthread_monitor_notify_all(*_mutex);
	omrthread_monitor_exit(*_mutex);
}

void
MM_YieldCollaborator::yield(MM_EnvironmentBase *env)
{
	omrthread_monitor_enter(*_mutex);

	_yieldCount += 1;
	uintptr_t index = _yieldIndex;
	
	/* because of sync sections nesting we have to do >= (instead of ==) */
	if (_yieldCount + *_count >= env->_currentTask->getThreadCount() || env->_currentTask->isSynchronized() /* only main active */) {
		/* CMVC 142132 We change the resume event, even if we are main thread (ie we do not explicitly notify ourselves).
		 * This is to "clear" any pending event (like newPacket) that may wake up worker after this point
		 * (when main thread thinks that every other GC thread is blocked) */
		_resumeEvent = notifyMain;
		if (env->isMainThread()) {
			/* all workers yielded/blocked - return right away */
			omrthread_monitor_exit(*_mutex);
			return;
		} else {
			/* notify main last thread synced/yielded */
			omrthread_monitor_notify_all(*_mutex);
		}
	}
	
	/* yielding */
	if (env->isMainThread()) {
		do {
			/* main waiting for workers to notify all of them synced/yielded */
			omrthread_monitor_wait(*_mutex);
		} while (_resumeEvent != notifyMain);
	} else {
		do {
			/* workers waiting for main to notify about the start of new quantum */
			omrthread_monitor_wait(*_mutex);
		} while (index == _yieldIndex);	
	}
	
	omrthread_monitor_exit(*_mutex);
}
