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
package j9vm.test.classunloading;

import java.io.IOException;

/**
 *	A classloader which fails to find a class unless it has a particular name.
 *
 *		Named instances of SelectiveJarClassLoader report their own finalization to 
 *	the FinalizationIndicator class, which may be querried to determine whether any
 *	given instance has been finalized.
 **/
public class SelectiveJarClassLoader extends SimpleJarClassLoader {
	String allowedClassName;
public SelectiveJarClassLoader(String name, String jarFileName, ClassLoader parent, String allowedClassName) 
	throws IOException
{
	super(name, jarFileName, parent);
	this.allowedClassName = allowedClassName;
}
public SelectiveJarClassLoader(String name, String jarFileName, String allowedClassName) 
	throws IOException
{
	super(name, jarFileName);
	this.allowedClassName = allowedClassName;
}
protected Class findClass(String name)
   	throws ClassNotFoundException
{
	if(!allowedClassName.equals(name))
		throw new ClassNotFoundException(name);
	return super.findClass(name);
}
}
