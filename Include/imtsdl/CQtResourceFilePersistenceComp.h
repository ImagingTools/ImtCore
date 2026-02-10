// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFileInfo>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFilePersistenceInfo.h>
#include <iprm/IParamsManager.h>


namespace imtsdl
{

/// \todo move to base framework; add comfort methods
class CQtResourceFilePersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence,
			virtual public ifile::IFilePersistenceInfo
{
public:


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CQtResourceFilePersistenceComp)
		I_REGISTER_INTERFACE(ifile::IFileTypeInfo)
		I_REGISTER_INTERFACE(ifile::IFilePersistence)
		I_REGISTER_INTERFACE(ifile::IFilePersistenceInfo)
	I_END_COMPONENT

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
	virtual bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = NULL, int flags = -1, bool doAppend = false) const override;
	virtual QString GetTypeDescription(const QString* extensionPtr = NULL) const override;

	// reimplemented (ifile::IFilePersistenceInfo)
	virtual QString GetLastFilePath(OperationType operationType = OT_UNKNOWN, PathType pathType = PT_COMPLETE) const override;

protected:
	QString GetPathForType(const QFileInfo& fileInfo, PathType pathType) const;

private:

	mutable QFileInfo m_lastOpenInfo;
	mutable QFileInfo m_lastSaveInfo;
};


} // namespace imtsdl
