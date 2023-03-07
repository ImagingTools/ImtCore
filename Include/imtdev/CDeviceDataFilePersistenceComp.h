#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>
#include <ilog/TLoggerCompWrap.h>

// Acula incldes
#include <imtdev/IDeviceDataPersistence.h>


namespace imtdev
{


/**
	This component realizes the implementation of the \c ifile::IFilePersistence interface using underlaying \c IDeviceDataPersistence instance.
*/
class CDeviceDataFilePersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence
{
	Q_DECLARE_TR_FUNCTIONS(CDeviceDataFilePersistenceComp);
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
	virtual int LoadFromFile(
				istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = NULL) const override;
	virtual int SaveToFile(
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


