// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <ilog/TLoggerCompWrap.h>

// Acula incldes
#include <imtdev/IDeviceDataPersistence.h>


namespace imtdev
{


/**
	@brief File persistence adapter for device data
	
	CDeviceDataFilePersistenceComp implements the **Adapter Pattern** to adapt
	IDeviceDataPersistence (low-level device storage) to ifile::IFilePersistence
	(file-based persistence). This enables device data to be saved and loaded using
	standard file operations.
	
	This component realizes the implementation of the ifile::IFilePersistence interface
	using an underlying IDeviceDataPersistence instance, bridging device hardware storage
	with file system operations.
	
	@par Purpose:
	Enables device data to be treated as files by:
	- Converting device storage operations to file load/save operations
	- Supporting progress tracking for long-running operations
	- Providing file type information for UI file dialogs
	- Validating operations against device capabilities
	
	@par Key Features:
	- **Storage Abstraction**: Maps device ROM/EEPROM to file operations
	- **Progress Support**: Tracks read/write progress for UI feedback
	- **File Type Info**: Provides file extensions and descriptions
	- **Operation Validation**: Checks if operations are supported before execution
	- **Metadata Handling**: Preserves device metadata in file operations
	
	@par Component Configuration:
	- **DeviceDataPersistence**: Reference to IDeviceDataPersistence providing low-level access
	
	@par Supported Operations:
	- **LoadFromFile**: Reads device data from storage into memory object
	- **SaveToFile**: Writes memory object to device storage
	- **IsOperationSupported**: Validates whether operation can be performed
	
	@par Usage Pattern:
	@code{.cpp}
	// Obtain file persistence adapter instance (via component system)
	CDeviceDataFilePersistenceComp* pFilePersistence = // get from component system
	
	// Component is configured with IDeviceDataPersistence reference
	// Access as file persistence interface
	ifile::IFilePersistence* pPersistence = pFilePersistence;
	
	// Load device data (provide your own data object and progress manager)
	istd::IChangeable* pData = // your data object
	ibase::IProgressManager* pProgress = // your progress manager
	auto result = pPersistence->LoadFromFile(*pData, "device_data.bin", pProgress);
	
	if (result == ifile::IFilePersistence::OS_SUCCESS)
	{
		// Process loaded data
	}
	
	// Save device data
	result = pPersistence->SaveToFile(*pData, "device_data.bin", pProgress);
	
	// Get file type info for file dialog
	QStringList extensions;
	pPersistence->GetFileExtensions(extensions);  // e.g., ["*.bin", "*.dat"]
	@endcode
	
	@par File Format:
	The actual file format depends on the device and storage type (ROM/EEPROM).
	Typically binary data with optional metadata header.
	
	@see IDeviceDataPersistence
	@see ifile::IFilePersistence
	@see ibase::IProgressManager
	@ingroup imtdev
*/
class CDeviceDataFilePersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence
{
	Q_DECLARE_TR_FUNCTIONS(CDeviceDataFilePersistenceComp)
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceDataFilePersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
		I_REGISTER_INTERFACE(ifile::IFileTypeInfo);
		I_ASSIGN(m_deviceDataPersistenceCompPtr, "DeviceDataPersistence", "Low-level persistence of the device", true, "DeviceDataPersistence");
	I_END_COMPONENT;

	// reimplemented (ifile::IFilePersistence)
	virtual bool IsOperationSupported(
				const istd::IChangeable* dataObjectPtr,
				const QString* filePathPtr = NULL,
				int flags = -1,
				bool beQuiet = true) const override;
	virtual OperationState LoadFromFile(
				istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const override;
	virtual OperationState SaveToFile(
				const istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const override;

	// reimplemented (ifile::IFileTypeInfo)
	virtual bool GetFileExtensions(
				QStringList& result,
				const istd::IChangeable * dataObjectPtr = NULL,
				int flags = -1,
				bool doAppend = false) const override;
	virtual QString GetTypeDescription(const QString * extensionPtr = NULL) const override;

private:
	I_REF(imtdev::IDeviceDataPersistence, m_deviceDataPersistenceCompPtr);
};


} // namespace imtdev


