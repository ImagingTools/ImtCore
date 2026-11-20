// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFileNameParam.h>
#include <ifile/IFilePersistence.h>
#include <ilog/TLoggerCompWrap.h>


namespace imtfile
{


/**
	File persistence of data that other users of the computer must not read, for example passwords: the slave persistence writes into
	a new file that only its owner can read (on Windows the permissions are its access list), the folder of the file is created for its
	owner only. On Unix a file or a folder that other users can read is not loaded: the secrets in it are not trusted to be secret.
*/
class CPrivateFilePersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPrivateFilePersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFileTypeInfo);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
		I_ASSIGN(m_slavePersistenceCompPtr, "SlavePersistence", "Persistence that writes and reads the data", true, "SlavePersistence");
		I_ASSIGN(m_filePathCompPtr, "FilePath", "The file of the data: its folder is kept for its owner only (the data can be written to a temporary file first)", true, "FilePath");
	I_END_COMPONENT;

	/**
		Create the folder (and its parents) so that only its owner can open it. An existing folder is not changed.
		\return \c false if the folder cannot be created, or (on Unix) other users can open it; \c errorMessage tells why.
	*/
	static bool EnsurePrivateFolder(const QString& folderPath, QString& errorMessage);

	/**
		Check that only the owner of the file can read it (on Unix; on Windows the access list is not checked).
	*/
	static bool IsPrivateFile(const QString& filePath, QString& errorMessage);

	// reimplemented (ifile::IFilePersistence)
	virtual bool IsOperationSupported(
				const istd::IChangeable* dataObjectPtr,
				const QString* filePathPtr = nullptr,
				int flags = -1,
				bool beQuiet = true) const override;
	virtual ifile::IFilePersistence::OperationState LoadFromFile(
				istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = nullptr) const override;
	virtual ifile::IFilePersistence::OperationState SaveToFile(
				const istd::IChangeable& data,
				const QString& filePath = QString(),
				ibase::IProgressManager* progressManagerPtr = nullptr) const override;

	// reimplemented (ifile::IFileTypeInfo)
	virtual bool GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr = nullptr, int flags = -1, bool doAppend = false) const override;
	virtual QString GetTypeDescription(const QString* extensionPtr = nullptr) const override;

private:
	I_REF(ifile::IFilePersistence, m_slavePersistenceCompPtr);
	I_REF(ifile::IFileNameParam, m_filePathCompPtr);
};


} // namespace imtfile


