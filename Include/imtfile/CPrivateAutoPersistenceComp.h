// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QMutex>

// STL includes
#include <atomic>

// ACF includes
#include <ifile/IFileNameParam.h>
#include <ifile/IFilePersistence.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtfile/ISavedFileDigestProvider.h>


namespace imtfile
{


/**
	Automatic persistence of an object in a private file: the object is restored from the file on begin, stored into it on each change
	and reloaded when another program changes the file.
	Unlike ifile::CAutoPersistenceComp the data never go to the temporary folder of the system, where other accounts can create files:
	the loader is given the file itself, it must replace the file as a whole and keep it private.
	The own saves are not loaded again: \c SavedFileDigestProvider tells the digest of what the loader has written (usually the loader
	itself), the file is not read back, another program could have changed it already. A special file in place of the file (a FIFO) is
	not waited for.
	\note The component has no interface: activate 'Automatically create instance' or initialize it explicitly.
*/
class CPrivateAutoPersistenceComp:
			public ilog::CLoggerComponentBase,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	enum ModelId
	{
		MI_OBJECT,
		MI_FILE_PATH
	};

	I_BEGIN_COMPONENT(CPrivateAutoPersistenceComp);
		I_ASSIGN(m_objectCompPtr, "Object", "Object that is restored from the file and stored into it", true, "Object");
		I_ASSIGN_TO(m_objectModelCompPtr, m_objectCompPtr, false);
		I_ASSIGN(m_fileLoaderCompPtr, "FileLoader", "Persistence of the file that replaces it as a whole", true, "FileLoader");
		I_ASSIGN(m_savedFileDigestProviderCompPtr, "SavedFileDigestProvider", "Digest of the contents that the file loader has saved: the own saves are not loaded again", true, "SavedFileDigestProvider");
		I_ASSIGN(m_filePathCompPtr, "FilePath", "File of the object", true, "FilePath");
		I_ASSIGN_TO(m_filePathModelCompPtr, m_filePathCompPtr, false);
		I_ASSIGN(m_restoreOnBeginAttrPtr, "RestoreOnBegin", "Restore the object from the file on begin", true, true);
		I_ASSIGN(m_storeOnBeginAttrPtr, "StoreOnBegin", "Store the object on begin if its file does not exist", true, false);
		I_ASSIGN(m_storeOnChangeAttrPtr, "StoreOnChange", "Store the object on each change", true, true);
		I_ASSIGN(m_reloadOnFileChangeAttrPtr, "AutoReload", "Reload the object when the file is changed", true, false);
	I_END_COMPONENT;

	CPrivateAutoPersistenceComp();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	QString GetFilePath() const;
	bool LoadObject();
	bool StoreObject();

	/**
		Watch the file and its folder: a file that is replaced (renamed over) is watched again.
	*/
	void WatchFile();

	/**
		Watch the file and its folder that a save has created: before they existed there was nothing to watch. In the thread of the
		watcher, it is not thread safe.
	*/
	void WatchCreatedFile();

	void OnFileSystemChanged();

private:
	I_REF(istd::IChangeable, m_objectCompPtr);
	I_REF(imod::IModel, m_objectModelCompPtr);
	I_REF(ifile::IFilePersistence, m_fileLoaderCompPtr);
	I_REF(ISavedFileDigestProvider, m_savedFileDigestProviderCompPtr);
	I_REF(ifile::IFileNameParam, m_filePathCompPtr);
	I_REF(imod::IModel, m_filePathModelCompPtr);
	I_ATTR(bool, m_restoreOnBeginAttrPtr);
	I_ATTR(bool, m_storeOnBeginAttrPtr);
	I_ATTR(bool, m_storeOnChangeAttrPtr);
	I_ATTR(bool, m_reloadOnFileChangeAttrPtr);

	QFileSystemWatcher m_fileWatcher;

	/**
		The loading and the storing of the object: the object can be changed in other threads.
	*/
	QMutex m_fileMutex;

	/**
		The contents of the file as they were last loaded or stored: the own changes of the file are not loaded again.
	*/
	QByteArray m_fileHash;

	/**
		The thread that loads the object: its notifications of the change are not stored back.
	*/
	std::atomic<Qt::HANDLE> m_loadingThreadId;
};


} // namespace imtfile


