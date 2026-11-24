// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ACF includes
#include <ifile/IFileNameParam.h>
#include <ifile/IFilePersistence.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtfile/ISavedFileDigestProvider.h>


namespace imtfile
{


/**
	File persistence of data that other users of the computer must not read, for example passwords.

	The data never leave the private folder of the file (the folder of \c FilePath): another path, for example a temporary file of
	ifile::CAutoPersistenceComp, is refused; use CPrivateAutoPersistenceComp for the automatic persistence. The slave persistence writes
	into a new file of the owner next to the file, the file is replaced by it as a whole only when the data are complete: a failed save
	keeps the previous file. The new file is private by itself (Unix: mode 600; Windows: its own protected access list, nothing of the
	folder is inherited) and is checked before the slave writes anything into it. The data are written to the disk before the file is
	replaced (on Unix also the folder after it): after a power failure the file is the previous or the new one, complete. On Windows
	another program can keep the file open for a moment and not allow to replace it (an antivirus checks a file that has just been
	written): the save tries again and waits for it at most a second.

	The slave persistence writes into the file that it gets, in place. A slave that writes a temporary file of its own and renames it
	over the file creates a file with the default access: on Unix every save is refused then. The slave reads the file by the path that
	it gets, after the checks.

	The private folder:
	- belongs to the account of the process, nobody else can open it: on Unix its mode is 700, on Windows only this account, the system
	  and the administrators are in its access list;
	- gives nobody else any access to its new files either: on Windows its access list has no entry for another account, also not one
	  for the contained files only; on macOS its access list (it gives its rights before the mode) has no entry that allows anything;
	- is created so when it does not exist, with the missing folders above it; it is private from the start, it is never open. On Unix
	  the folders get their mode at once (700, the folders above 755), the umask can only take rights away; on macOS what the parent
	  gives to the new folders (its inherited access list) is removed from a new folder right after it is created;
	- if it exists already, it is only checked, its owner and its permissions are never changed: a folder that another account owns
	  could be taken back by it, and a folder that was open could have been read or given a planted file. Such a folder is refused,
	  the reason (and what to do) goes to the log.

	The folders above the private folder may be readable by others, they need not be private. But nobody else may replace what is in
	them: otherwise another account could rename the private folder and put its own one in its place. A folder above:
	- on Unix belongs to this account or to root; if others can write into it, it must have the sticky bit (as /tmp has: only the
	  owner of an entry can remove or rename it). Examples: /etc (root, 755) and /tmp (root, 1777) are accepted, a folder with mode 777
	  or one of another user is refused. On macOS the access list may not allow the others to delete, rename or change the access
	  either. The path is resolved as the system resolves it, element by element: each folder on the way is checked, also the folders
	  that a link leads to and the links that a link leads to. A link must belong to this account or to root, the folder that contains
	  it is checked as any other: a link in a folder that the others can change could be replaced by them;
	- on Windows belongs to this account, the system, the administrators or the installer of Windows; other accounts may read it and
	  create new entries in it, but not delete, rename or change the access of what is in it (DELETE, FILE_DELETE_CHILD, WRITE_DAC,
	  WRITE_OWNER, GENERIC_ALL). The usual access of ProgramData and of a folder created in it is accepted, but only if the service or
	  an administrator created it: the creator of a folder becomes its owner and gets full access (CREATOR OWNER), so a folder created
	  by an ordinary user is refused. "Everyone: full control" is refused. A junction or a link (a reparse point) on the way is
	  refused: the access is read through it, the folders that it leads to are not checked.
	The folders that exist are checked before the missing ones are created: nothing is created in a refused folder. When all the folders
	on the way are checked so, only this account and the system can change where the path leads: the files are written and read by the
	path (the slave persistence takes a path) and get to the checked folder.

	The file belongs to this account and nobody else can read it (Unix: a regular file with mode 600, not a link; Windows: its owner and
	access list as of the private folder, not a link). A file or a folder that does not satisfy all of it is neither loaded nor saved:
	when in doubt (the access cannot be read, an entry of an unknown kind), the persistence refuses. A file that does not exist is not
	passed to the slave persistence either: a link in its place counts as a file and is refused. IsOperationSupported() with a path to
	load answers the same: a caller can check the file before it reads it.

	The data of the computer (\c TrustAdministrators), as the settings of a service that an administrator changes with another program:
	on Windows the folder and the files that a process of the system or of an administrator creates give access to the system and the
	administrators only, not to the account of the process, so they are the same whoever creates them. Without it the account that
	creates them is in their access list as well, and the other accounts refuse them: a service of the system refuses the files that an
	administrator has saved. The checks are the same in both ways. On Unix it changes nothing: the private folder and the file belong to
	the account of the process, the program of the administrator runs as the account of the service.

	The digest of the contents of each save (ISavedFileDigestProvider) is taken from the new file before it replaces the file.

	Every save checks the whole path, writes the file and its digest and writes them to the disk: the data that are changed often (for
	example while a slider moves) should be saved less often, not on every change.
*/
class CPrivateFilePersistenceComp:
			public ilog::CLoggerComponentBase,
			virtual public ifile::IFilePersistence,
			virtual public ISavedFileDigestProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPrivateFilePersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFileTypeInfo);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
		I_REGISTER_INTERFACE(ISavedFileDigestProvider);
		I_ASSIGN(m_slavePersistenceCompPtr, "SlavePersistence", "Persistence that writes and reads the data", true, "SlavePersistence");
		I_ASSIGN(m_filePathCompPtr, "FilePath", "The file of the data: only the files of its folder are written and read, the folder is kept for its owner only", true, "FilePath");
		I_ASSIGN(m_trustAdministratorsAttrPtr, "TrustAdministrators", "The data of the computer: on Windows the new folder and files of the system or an administrator are for the system and the administrators only, not for the account that creates them, so a service and the program of an administrator share them. Both must have it enabled", true, false);
	I_END_COMPONENT;

	/**
		Create the folder for its owner only (on Windows: this account, the system and the administrators) and the missing folders above
		it, after the existing folders above it are checked. An existing folder is only checked, never changed.
		\param areAdministratorsTrusted	on Windows a process of the system or of an administrator creates the folder for the system and
										the administrators only, see \c TrustAdministrators.
		\return \c false if the folder cannot be created, another account owns or can open it, or another account can replace a folder
		above it; \c errorMessage tells why.
	*/
	static bool EnsurePrivateFolder(const QString& folderPath, QString& errorMessage, bool areAdministratorsTrusted = false);

	/**
		Check that the file belongs to this account and nobody else can read it (on Windows: its owner and its access list).
	*/
	static bool IsPrivateFile(const QString& filePath, QString& errorMessage);

	// reimplemented (imtfile::ISavedFileDigestProvider)
	virtual QByteArray GetSavedFileDigest(const QString& filePath) const override;

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
	/**
		The path of the file to write or read: \c filePath, or the file of the component if it is empty. It must be in the folder of the
		file of the component.
	*/
	bool GetPrivatePath(const QString& filePath, QString& path, QString& errorMessage) const;

private:
	I_REF(ifile::IFilePersistence, m_slavePersistenceCompPtr);
	I_REF(ifile::IFileNameParam, m_filePathCompPtr);
	I_ATTR(bool, m_trustAdministratorsAttrPtr);

	/**
		The digests of the last saves by the path of the file. The saves can run in several threads: the mutex is held across the
		replacement of the file and the publication of its digest, they stay in the same order.
	*/
	mutable QMutex m_savedDigestsMutex;
	mutable QMap<QString, QByteArray> m_savedDigests;
};


} // namespace imtfile


