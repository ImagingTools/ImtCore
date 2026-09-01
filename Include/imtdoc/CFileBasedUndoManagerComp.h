// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TDelPtr.h>
#include <iser/ISerializable.h>
#include <iser/CMemoryWriteArchive.h>
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileNameParam.h>
#include <idoc/IUndoManager.h>
#include <imtbase/TModelUpdateBinder.h>

// ImtCore includes
#include <imtdoc/IPersistentUndoManager.h>


namespace imtdoc
{


/**
	Implements multi-level UNDO mechanism based on storing complete object state at each step in files.

	Unlike \ref idoc::CSerializedUndoManagerComp, which keeps all undo/redo snapshots in memory, this
	component persists each document state to a separate file using an external file persistence.
	This keeps the memory footprint small and is well suited for large documents or long undo histories.

	Each undo step is written to a file named \c Undo_<index>.<ext>, each redo step to
	\c Redo_<index>.<ext>, and the current state to \c CurrentState.<ext>, where \c <ext> is taken
	from the configured persistence. Files backing steps that are no longer needed (for example when
	the redo list is cleared after a new change) are removed automatically.

	The steps are stored in a subdirectory of \b RootFolder identified by the document whose
	state is undone/redone: \c <RootFolder>/<DocumentTypeId>/<DocumentId>. The document identity
	must be provided via \ref imtdoc::IPersistentUndoManager::Initialize. Auto-persistence can be
	initialized by the owner through a component cast.
	The document type ID is optional
	and is simply omitted from the path (steps are then stored directly under
	\c <RootFolder>/<DocumentId>) if not provided.

	The complete undo/redo history together with the index of the current step can be persisted and
	restored using the iser::ISerializable interface. The observed document is not written into the
	archive itself: instead, its state at the current step is stored as a dedicated step file and,
	when loading the history, the observed document is brought back to the content it had at the
	current step by deserializing it from the file corresponding to that step.
*/
class CFileBasedUndoManagerComp:
			public icomp::CComponentBase,
			public imod::TSingleModelObserverBase<iser::ISerializable>,
			virtual public imtdoc::IPersistentUndoManager,
			virtual public iser::ISerializable
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::TSingleModelObserverBase<iser::ISerializable> BaseClass2;

	I_BEGIN_COMPONENT(CFileBasedUndoManagerComp);
		I_REGISTER_INTERFACE(idoc::IUndoManager);
		I_REGISTER_INTERFACE(idoc::IDocumentStateComparator);
		I_REGISTER_INTERFACE(imtdoc::IPersistentUndoManager);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_documentPersistenceCompPtr, "DocumentPersistence", "Persistence used to serialize document states to files", true, "DocumentPersistence");
		I_ASSIGN(m_undoManagerPersistenceCompPtr, "UndoManagerPersistence", "Persistence used to serialize CFileBasedUndoManagerComp state", false, "UndoManagerPersistence");
		I_ASSIGN(m_rootFolderCompPtr, "RootFolder", "Root folder where the state of this undo manager and the document snapshots are stored", true, "RootFolder");
		I_ASSIGN(m_undoManagerFilePathCompPtr, "UndoManagerFilePath", "File path for serializing CFileBasedUndoManagerComp state", false, "UndoManagerFilePath");
	I_END_COMPONENT;

	CFileBasedUndoManagerComp();

	// reimplemented (idoc::IUndoManager)
	virtual int GetAvailableUndoSteps() const override;
	virtual int GetAvailableRedoSteps() const override;
	virtual QString GetUndoLevelDescription(int stepIndex) const override;
	virtual QString GetRedoLevelDescription(int stepIndex) const override;
	virtual void ResetUndo() override;
	virtual bool DoUndo(int steps = 1) override;
	virtual bool DoRedo(int steps = 1) override;

	// reimplemented (imtdoc::IPersistentUndoManager)
	virtual void Initialize(const QByteArray& documentId, const QByteArray& documentTypeId) override;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	class IUndoState: virtual public istd::IPolymorphic
	{
	};

	typedef istd::TDelPtr<IUndoState> UndoStatePtr;

	struct UndoStepInfo
	{
		UndoStatePtr statePtr;
		QString description;
	};

	typedef QList<UndoStepInfo> UndoList;

	class FileUndoState: public IUndoState, virtual public iser::ISerializable
	{
	public:
		explicit FileUndoState(const QString& filePath);
		virtual ~FileUndoState();

		const QString& GetFilePath() const;

		// reimplemented (iser::ISerializable)
		virtual bool Serialize(iser::IArchive& archive) override;

	private:
		QString m_filePath;
	};

	bool DoListShift(int steps, UndoList& fromList, UndoList& toList);
	IUndoState* CreateState(iser::ISerializable& object, const QString& stepFileName);
	bool RestoreState(const IUndoState& state, iser::ISerializable& object);
	bool AreStatesEqual(const IUndoState& state1, const IUndoState& state2) const;
	bool RestoreObservedObject(const IUndoState& state);
	QString CreateStepFilePath(const QString& stepFileName) const;
	QString GetStorageDirectoryPath() const;
	QString GetPersistenceFileExtension(ifile::IFilePersistence* persistencePtr) const;
	QString GetUndoManagerFilePath() const;
	void OnUndoManagerStateChanged(const istd::IChangeable::ChangeSet& changeSet, const istd::IChangeable* objectPtr);

	// reimplemented (imod::TSingleModelObserverBase<iser::ISerializable>)
	virtual iser::ISerializable* CastFromModel(imod::IModel* modelPtr) const override;

	// reimplemented (imod::IObserver)
	virtual void BeforeUpdate(imod::IModel* modelPtr) override;
	virtual void AfterUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (idoc::IDocumentStateComparator)
	virtual bool HasStoredDocumentState() const override;
	virtual bool StoreDocumentState() override;
	virtual bool RestoreDocumentState() override;
	virtual DocumentChangeFlag GetDocumentChangeFlag() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed() override;

private:
	UndoList m_undoList;
	UndoList m_redoList;

	UndoStatePtr m_beginStatePtr;

	int m_uniqueFileCounter;
	UndoStepInfo m_currentState;

	bool m_hasStoredDocumentState;
	bool m_isBlocked;
	bool m_isDestroying;
	bool m_isInitialized;

	iser::CMemoryWriteArchive m_storedStateArchive;

	mutable DocumentChangeFlag m_stateChangedFlag;
	mutable bool m_isStateChangedFlagValid;

	QByteArray m_documentId;
	QByteArray m_documentTypeId;

	I_REF(ifile::IFilePersistence, m_documentPersistenceCompPtr);
	I_REF(ifile::IFilePersistence, m_undoManagerPersistenceCompPtr);
	I_REF(ifile::IFileNameParam, m_rootFolderCompPtr);
	I_REF(ifile::IFileNameParam, m_undoManagerFilePathCompPtr);
	imtbase::TModelUpdateBinder<istd::IChangeable, CFileBasedUndoManagerComp> m_modelObserver;
};


} // namespace imtdoc
