/*[INCLUDE-IF Sidecar18-SE]*/
/*******************************************************************************
 * Copyright IBM Corp. and others 2012
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
package com.ibm.java.diagnostics.utils.plugins;

import java.util.Map;

/**
 * DTFJ specific version of the plugin config
 *
 * @author adam
 */
public class DTFJPluginConfig extends PluginConfig {
	protected String version = "1.*";
	protected boolean runtime = false;
	protected boolean image = false;
	protected String csv = null;

	public DTFJPluginConfig(Entry entry) {
		super(entry);
	}

	/**
	 * Process the annotations found on this class.
	 *
	 * @throws IllegalArgumentException if the DTFJPlugin annotation is not present
	 */
	public void processAnnotations() {
		ClassInfo info = entry.getData();
		Annotation annotation = info.getAnnotation(DTFJPluginManager.ANNOTATION_CLASSNAME);
		if (annotation == null) {
			throw new IllegalArgumentException("The entry " + entry.getName() + " does not have the DTFJ plugin annotation"); //$NON-NLS-1$ //$NON-NLS-2$
		}

		for (Map.Entry<String, Object> entry : annotation.getValues().entrySet()) {
			String key = entry.getKey();
			Object value = entry.getValue();

			if (key.equals("version")) { //$NON-NLS-1$
				this.version = (String) value;
			} else if (key.equals("runtime")) { //$NON-NLS-1$
				this.runtime = ((Boolean) value).booleanValue();
			} else if (key.equals("image")) { //$NON-NLS-1$
				this.image = ((Boolean) value).booleanValue();
			} else if (key.equals("cacheOutput")) { //$NON-NLS-1$
				this.cacheOutput = ((Boolean) value).booleanValue();
			}
		}
	}

	public String getVersion() {
		return version;
	}

	public boolean isRuntime() {
		return runtime;
	}

	public boolean isImage() {
		return image;
	}

	@Override
	public String toCSV() {
		if (csv == null) {
			ClassInfo info = entry.getData();
			StringBuilder builder = new StringBuilder();
			builder.append(info.getClassname());
			builder.append(',');
			builder.append(version);
			builder.append(",true,");
			builder.append(info.getURL().toString());
			builder.append(',');
			if (null != t) {
				builder.append(t.getClass().getName());
			}
			csv = builder.toString();
		}
		return csv;
	}

	@Override
	public String toString() {
		return "DTFJ Plugin " + getClassName();
	}

}
