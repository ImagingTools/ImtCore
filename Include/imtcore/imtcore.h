// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtcore.h
	\brief Main header file for the imtcore module - Core SDK Version and Metadata
	
	This file provides namespace-level documentation for the imtcore module.
	Include this file to access the complete imtcore module documentation in Doxygen.
*/

/**
	\namespace imtcore
	\brief Core SDK Version and Metadata Module
	
	The \b imtcore module provides version information and core metadata for the
	ImtCore SDK, enabling version checking, compatibility validation, and SDK
	identification across applications.
	
	\section imtcore_overview Overview
	
	This lightweight module serves as the identity provider for the ImtCore SDK:
	- SDK version identification
	- Version compatibility checking
	- Build metadata and configuration
	- SDK-wide constants and identifiers
	
	\section imtcore_architecture Architecture
	
	\subsection imtcore_version Version Management
	
	**Version Identification:**
	\code
	enum VersionId {
	    VI_IMTCORE = 1977  // ImtCore SDK version identifier
	};
	\endcode
	
	The version identifier serves as:
	- Compile-time SDK version check
	- Runtime compatibility validation
	- Plugin API version matching
	- Binary compatibility verification
	
	\section imtcore_usage Usage Examples
	
	\subsection imtcore_version_check Version Checking
	
	**Check SDK Version:**
	\code{.cpp}
	#include <imtcore/Version.h>
	
	void CheckSdkVersion()
	{
	    // Get SDK version
	    int sdkVersion = imtcore::VI_IMTCORE;
	    qDebug() << "ImtCore SDK Version:" << sdkVersion;
	    
	    // Verify minimum required version
	    const int REQUIRED_VERSION = 1977;
	    if (sdkVersion < REQUIRED_VERSION) {
	        qFatal("ImtCore SDK version too old. Required: %d, Found: %d",
	               REQUIRED_VERSION, sdkVersion);
	    }
	}
	\endcode
	
	\subsection imtcore_plugin_compatibility Plugin Compatibility
	
	**Plugin Version Validation:**
	\code{.cpp}
	class CMyPluginComp : public ACF_COMPONENT(IMyPlugin)
	{
	public:
	    int GetRequiredSdkVersion() const override
	    {
	        return imtcore::VI_IMTCORE;
	    }
	    
	    bool IsCompatibleWithSdk(int sdkVersion) const override
	    {
	        // Plugin requires exact SDK version match
	        return sdkVersion == imtcore::VI_IMTCORE;
	    }
	};
	\endcode
	
	\subsection imtcore_build_info Build Information
	
	**Display Build Information:**
	\code{.cpp}
	void DisplayAboutDialog()
	{
	    QMessageBox aboutBox;
	    aboutBox.setWindowTitle("About Application");
	    
	    QString aboutText = QString(
	        "Application Version: 1.0.0\n"
	        "ImtCore SDK Version: %1\n"
	        "Build Date: %2\n"
	        "Qt Version: %3"
	    ).arg(imtcore::VI_IMTCORE)
	     .arg(__DATE__)
	     .arg(QT_VERSION_STR);
	    
	    aboutBox.setText(aboutText);
	    aboutBox.exec();
	}
	\endcode
	
	\section imtcore_best_practices Best Practices
	
	\subsection imtcore_versioning Version Management
	
	- Check SDK version at application startup
	- Validate plugin compatibility before loading
	- Use version identifiers in binary formats
	- Document version requirements in README
	- Implement graceful degradation for version mismatches
	
	\subsection imtcore_compatibility Compatibility Considerations
	
	- Major version changes may break API compatibility
	- Check version before calling new SDK features
	- Provide fallback implementations for older versions
	- Test applications against multiple SDK versions
	- Document minimum required SDK version
	
	\section imtcore_related_modules Integration with Other Modules
	
	<b>SDK-wide Usage:</b>
	- All ImtCore modules depend on imtcore
	- Version checking in plugin systems
	- Binary format compatibility validation
	- Build configuration and metadata
	
	\section imtcore_references References
	
	<b>Related Modules:</b>
	- imtbase - Core infrastructure
	- All ImtCore modules use imtcore for versioning
	
	<b>External Documentation:</b>
	- Semantic Versioning: https://semver.org/
	- Plugin Architecture Patterns
*/

namespace imtcore
{
	// Namespace for core SDK version and metadata
}
