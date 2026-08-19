// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CSerializedUndoManagerCompBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/CMemoryReadArchive.h>
#include <icomp/CComponentBase.h>


namespace imtbase
{


// static attributes
static const istd::IChangeable::ChangeSet s_undoChangeSet(istd::IChangeable::CF_NO_UNDO, "UNDO");


CSerializedUndoManagerCompBase::UndoStepInfo::UndoStepInfo()
:	stepId(-1)
{
}


CSerializedUndoManagerCompBase::CSerializedUndoManagerCompBase()
:	m_beginStepId(-1),
	m_hasStoredDocumentState(false),
	m_isBlocked(false),
	m_stateChangedFlag(DCF_UNKNOWN),
	m_isStateChangedFlagValid(false)
{
}


// reimplemented (idoc::IUndoManager)

int CSerializedUndoManagerCompBase::GetAvailableUndoSteps() const
{
	return m_undoList.size();
}


int CSerializedUndoManagerCompBase::GetAvailableRedoSteps() const
{
	return m_redoList.size();
}


QString CSerializedUndoManagerCompBase::GetUndoLevelDescription(int stepIndex) const
{
	if ((stepIndex > 0) && (stepIndex <= m_undoList.size())){
		return m_undoList[m_undoList.size() - stepIndex].description;
	}

	return "";
}


QString CSerializedUndoManagerCompBase::GetRedoLevelDescription(int stepIndex) const
{
	if ((stepIndex > 0) && (stepIndex <= m_redoList.size())){
		return m_redoList[m_redoList.size() - stepIndex].description;
	}

	return "";
}


void CSerializedUndoManagerCompBase::ResetUndo()
{
	istd::CChangeNotifier notifier(this);
	Q_UNUSED(notifier);

	ClearList(m_undoList);
	ClearList(m_redoList);

	if (m_beginStepId >= 0){
		ReleaseStep(m_beginStepId);
		m_beginStepId = -1;
	}

	NotifyCurrentStepChanged();
}


bool CSerializedUndoManagerCompBase::DoUndo(int steps)
{
	return DoListShift(steps, m_undoList, m_redoList);
}


bool CSerializedUndoManagerCompBase::DoRedo(int steps)
{
	return DoListShift(steps, m_redoList, m_undoList);
}


// reimplemented (imod::IObserver)

bool CSerializedUndoManagerCompBase::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	if (BaseClass2::OnModelAttached(modelPtr, changeMask)){
		m_hasStoredDocumentState = false;
		m_storedStateArchive.Reset();

		m_stateChangedFlag = DCF_UNKNOWN;

		return true;
	}

	return false;
}


bool CSerializedUndoManagerCompBase::OnModelDetached(imod::IModel* modelPtr)
{
	if (BaseClass2::OnModelDetached(modelPtr)){
		m_hasStoredDocumentState = false;
		m_storedStateArchive.Reset();

		m_stateChangedFlag = DCF_UNKNOWN;

		return true;
	}

	return false;
}


// protected methods

void CSerializedUndoManagerCompBase::OnCurrentStepChanged(int /*currentStep*/)
{
}


bool CSerializedUndoManagerCompBase::DoListShift(int steps, UndoList& fromList, UndoList& toList)
{
	bool retVal = false;

	if ((steps > 0) && (fromList.size() >= steps)){
		iser::ISerializable* objectPtr = GetObservedObject();

		if (objectPtr != NULL){
			istd::CChangeNotifier notifier(this);
			Q_UNUSED(notifier);

			Q_ASSERT(!m_isBlocked);
			m_isBlocked = true;

			istd::CChangeNotifier objectNotifier(objectPtr, &s_undoChangeSet);
			Q_UNUSED(objectNotifier);

			// stores pointer to last processed description container, necessary due of description shift
			// in undo list the state corresponds to state before changes, in redo - after changes. It causes description shift.
			QString* lastDescriptionPtr = NULL;

			int currentStepId = WriteStep(*objectPtr);
			if (currentStepId >= 0){
				toList.push_back(UndoStepInfo());

				UndoStepInfo& currentStep = toList.back();

				currentStep.stepId = currentStepId;
				lastDescriptionPtr = &currentStep.description;
			}

			int sourceStepId = fromList[fromList.size() - steps].stepId;

			if (ReadStep(sourceStepId, *objectPtr)){
				for (int i = 1; i < steps; ++i){
					if (lastDescriptionPtr != NULL){
						*lastDescriptionPtr = fromList.back().description;
					}

					toList.push_back(fromList.back());
					lastDescriptionPtr = &toList.back().description;

					fromList.pop_back();
				}

				if (lastDescriptionPtr != NULL){
					*lastDescriptionPtr = fromList.back().description;
				}

				ReleaseStep(fromList.back().stepId);
				fromList.pop_back();

				retVal = true;
			}

			objectNotifier.Reset();

			m_isBlocked = false;
		}
	}

	if (retVal){
		NotifyCurrentStepChanged();
	}

	return retVal;
}


void CSerializedUndoManagerCompBase::ClearList(UndoList& list)
{
	for (UndoList::Iterator iter = list.begin(); iter != list.end(); ++iter){
		ReleaseStep(iter->stepId);
	}

	list.clear();
}


// reimplemented (imod::TSingleModelObserverBase<iser::ISerializable>)

iser::ISerializable* CSerializedUndoManagerCompBase::CastFromModel(imod::IModel* modelPtr) const
{
	return CompCastPtr<iser::ISerializable>(modelPtr);
}


// reimplemented (imod::IObserver)

void CSerializedUndoManagerCompBase::BeforeUpdate(imod::IModel* modelPtr)
{
	BaseClass2::BeforeUpdate(modelPtr);

	if (!m_isBlocked && (m_beginStepId < 0)){
		iser::ISerializable* objectPtr = GetObservedObject();
		if (objectPtr != NULL){
			if (m_undoList.isEmpty() || !IsStepEqualToState(m_undoList.back().stepId, *objectPtr)){
				int stepId = WriteStep(*objectPtr);
				if (stepId >= 0){
					m_beginStepId = stepId;
				}
			}
		}
	}
}


void CSerializedUndoManagerCompBase::AfterUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet)
{
	Q_ASSERT(!changeSet.IsEmpty());

	m_isStateChangedFlagValid = false;

	bool skipUndo = changeSet.ContainsExplicit(istd::IChangeable::CF_NO_UNDO, true);

	if (		!m_isBlocked &&
				!skipUndo &&
				(m_beginStepId >= 0)){
		iser::ISerializable* objectPtr = GetObservedObject();

		bool committed = false;

		if (objectPtr != NULL){
			if (!IsStepEqualToState(m_beginStepId, *objectPtr)){
				istd::CChangeNotifier notifier(this);
				Q_UNUSED(notifier);

				m_undoList.push_back(UndoStepInfo());
				m_undoList.back().stepId = m_beginStepId;
				m_undoList.back().description = changeSet.GetDescription();

				committed = true;

				if (m_maxBufferSizeAttrPtr.IsValid() && (GetUsedMemorySize() > qint64(*m_maxBufferSizeAttrPtr) * (1 << 20))){
					ReleaseStep(m_undoList.front().stepId);
					m_undoList.pop_front();
				}

				ClearList(m_redoList);

				NotifyCurrentStepChanged();
			}
		}

		if (!committed){
			ReleaseStep(m_beginStepId);
		}

		m_beginStepId = -1;
	}

	BaseClass2::AfterUpdate(modelPtr, changeSet);
}


// reimplemented (idoc::IDocumentStateComparator)

bool CSerializedUndoManagerCompBase::HasStoredDocumentState() const
{
	return m_hasStoredDocumentState;
}


bool CSerializedUndoManagerCompBase::StoreDocumentState()
{
	istd::CChangeNotifier notifier(this);
	Q_UNUSED(notifier);

	m_storedStateArchive.Reset();

	iser::ISerializable* serializablePtr = GetObservedObject();
	if ((serializablePtr != NULL) && serializablePtr->Serialize(m_storedStateArchive)){
		m_stateChangedFlag = DCF_EQUAL;
		m_hasStoredDocumentState = true;
	}
	else{
		m_stateChangedFlag = DCF_UNKNOWN;
		m_hasStoredDocumentState = false;
	}
	m_isStateChangedFlagValid = true;

	return m_hasStoredDocumentState;
}


bool CSerializedUndoManagerCompBase::RestoreDocumentState()
{
	iser::CMemoryReadArchive restoreArchive(m_storedStateArchive);

	if (m_hasStoredDocumentState){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		iser::ISerializable* objectPtr = GetObservedObject();
		if (objectPtr != NULL){
			Q_ASSERT(!m_isBlocked);
			m_isBlocked = true;

			if (objectPtr->Serialize(restoreArchive)){
				m_stateChangedFlag = DCF_EQUAL;
				m_isStateChangedFlagValid = true;

				m_isBlocked = false;

				return true;
			}

			m_isBlocked = false;
		}

		m_stateChangedFlag = DCF_UNKNOWN;
		m_isStateChangedFlagValid = true;

		ClearList(m_undoList);
		ClearList(m_redoList);

		NotifyCurrentStepChanged();
	}

	return false;
}


idoc::IDocumentStateComparator::DocumentChangeFlag CSerializedUndoManagerCompBase::GetDocumentChangeFlag() const
{
	if (!m_isStateChangedFlagValid){
		m_stateChangedFlag = DCF_UNKNOWN;

		if (m_hasStoredDocumentState){
			iser::CMemoryWriteArchive compareArchive;

			iser::ISerializable* serializablePtr = GetObservedObject();
			if ((serializablePtr != NULL) && const_cast<iser::ISerializable*>(serializablePtr)->Serialize(compareArchive)){
				m_stateChangedFlag = (compareArchive != m_storedStateArchive)? DCF_DIFFERENT: DCF_EQUAL;
			}
		}

		m_isStateChangedFlagValid = true;
	}

	return m_stateChangedFlag;
}


// reimplemented (icomp::CComponentBase)

void CSerializedUndoManagerCompBase::OnComponentDestroyed()
{
	EnsureModelDetached();

	BaseClass::OnComponentDestroyed();
}


// private methods

qint64 CSerializedUndoManagerCompBase::GetUsedMemorySize() const
{
	qint64 memorySize = 0;

	for (UndoList::ConstIterator iter = m_undoList.constBegin(); iter != m_undoList.constEnd(); ++iter){
		memorySize += GetStepSize(iter->stepId);
	}

	return memorySize;
}


void CSerializedUndoManagerCompBase::NotifyCurrentStepChanged()
{
	OnCurrentStepChanged(m_undoList.size());
}


} // namespace imtbase
