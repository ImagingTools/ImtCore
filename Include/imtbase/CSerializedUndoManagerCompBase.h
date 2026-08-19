// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <iser/ISerializable.h>
#include <iser/CMemoryWriteArchive.h>
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <idoc/IUndoManager.h>


namespace imtbase
{


/**
	Base class for multi-level UNDO managers that store a complete document state at each step
	using serialization.

	This component implements the whole UNDO/REDO bookkeeping (undo and redo stacks, step
	descriptions, document state comparison and observation of the document model). The actual
	storage of a single serialized document state is delegated to derived classes through a set
	of abstract step operations. This allows different backends (e.g. in-memory buffers or files)
	to reuse the same UNDO/REDO logic.

	A single step is identified by an integer handle allocated by the derived class in
	\ref WriteStep. The base class keeps these handles in its undo and redo lists and asks the
	derived class to read, compare, release or measure the corresponding storage.

	\par Component Attributes
	- \b MaxBufferSize - Maximum memory size for the undo buffer in megabytes (default: 100 MB)

	\sa idoc::IUndoManager, idoc::IDocumentStateComparator, CFileSerializedUndoManagerComp
*/
class CSerializedUndoManagerCompBase:
			public icomp::CComponentBase,
			public imod::TSingleModelObserverBase<iser::ISerializable>,
			virtual public idoc::IUndoManager
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imod::TSingleModelObserverBase<iser::ISerializable> BaseClass2;

	I_BEGIN_BASE_COMPONENT(CSerializedUndoManagerCompBase);
		I_REGISTER_INTERFACE(idoc::IUndoManager);
		I_REGISTER_INTERFACE(idoc::IDocumentStateComparator);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_ASSIGN(m_maxBufferSizeAttrPtr, "MaxBufferSize", "Maximal memory size of the Undo-buffer in MByte", false, 100);
	I_END_COMPONENT;

	CSerializedUndoManagerCompBase();

	// reimplemented (idoc::IUndoManager)
	virtual int GetAvailableUndoSteps() const override;
	virtual int GetAvailableRedoSteps() const override;
	virtual QString GetUndoLevelDescription(int stepIndex) const override;
	virtual QString GetRedoLevelDescription(int stepIndex) const override;
	virtual void ResetUndo() override;
	virtual bool DoUndo(int steps = 1) override;
	virtual bool DoRedo(int steps = 1) override;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;

protected:
	struct UndoStepInfo
	{
		UndoStepInfo();

		int stepId;
		QString description;
	};
	typedef QList<UndoStepInfo> UndoList;

	/**
		Store the current state of \a object into a newly allocated step storage.
		\return non-negative identifier of the created step on success, or -1 on failure.
	*/
	virtual int WriteStep(iser::ISerializable& object) = 0;

	/**
		Restore the state stored in step \a stepId into \a object.
	*/
	virtual bool ReadStep(int stepId, iser::ISerializable& object) const = 0;

	/**
		\return \c true if the current state of \a object is equal to the state stored in step \a stepId.
	*/
	virtual bool IsStepEqualToState(int stepId, iser::ISerializable& object) const = 0;

	/**
		Release the storage associated with step \a stepId.
	*/
	virtual void ReleaseStep(int stepId) = 0;

	/**
		\return size in bytes used by the storage of step \a stepId.
	*/
	virtual qint64 GetStepSize(int stepId) const = 0;

	/**
		Called whenever the current position within the UNDO/REDO history changes.
		\param currentStep number of currently available UNDO steps (i.e. the current history index).
		The default implementation does nothing.
	*/
	virtual void OnCurrentStepChanged(int currentStep);

	bool DoListShift(int steps, UndoList& fromList, UndoList& toList);

	/**
		Release the storage of every step in \a list and clear it.
	*/
	void ClearList(UndoList& list);

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
	qint64 GetUsedMemorySize() const;
	void NotifyCurrentStepChanged();

	UndoList m_undoList;
	UndoList m_redoList;

	int m_beginStepId;

	bool m_hasStoredDocumentState;
	bool m_isBlocked;

	iser::CMemoryWriteArchive m_storedStateArchive;

	mutable DocumentChangeFlag m_stateChangedFlag;
	mutable bool m_isStateChangedFlagValid;

	I_ATTR(int, m_maxBufferSizeAttrPtr);
};


} // namespace imtbase
