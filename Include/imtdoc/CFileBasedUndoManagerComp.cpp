// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CFileBasedUndoManagerComp.h>


// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QDir>
#include <QtCore/QFile>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CMemoryReadArchive.h>
#include <icomp/CComponentBase.h>


namespace imtdoc
{


// static attributes
static const istd::IChangeable::ChangeSet s_undoChangeSet(istd::IChangeable::CF_NO_UNDO, "UNDO");


// FileUndoState

CFileBasedUndoManagerComp::FileUndoState::FileUndoState(const QString& filePath)
:	m_filePath(filePath)
{
}


CFileBasedUndoManagerComp::FileUndoState::~FileUndoState()
{
	if (!m_filePath.isEmpty() && QFile::exists(m_filePath)){
		QFile::remove(m_filePath);
	}
}


const QString& CFileBasedUndoManagerComp::FileUndoState::GetFilePath() const
{
	return m_filePath;
}


bool CFileBasedUndoManagerComp::FileUndoState::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag filePathTag("FilePath", "Path to the file storing the document state", iser::CArchiveTag::TT_LEAF);

	return archive.BeginTag(filePathTag) && archive.Process(m_filePath) && archive.EndTag(filePathTag);
}


// CFileBasedUndoManagerComp

CFileBasedUndoManagerComp::CFileBasedUndoManagerComp()
:	m_uniqueFileCounter(0),
	m_hasStoredDocumentState(false),
	m_isBlocked(false),
	m_isDestroying(false),
	m_isInitialized(false),
	m_stateChangedFlag(DCF_UNKNOWN),
	m_isStateChangedFlagValid(false),
	m_modelObserver(*this)
{
}


// reimplemented (idoc::IUndoManager)

int CFileBasedUndoManagerComp::GetAvailableUndoSteps() const
{
	return m_undoList.size();
}


int CFileBasedUndoManagerComp::GetAvailableRedoSteps() const
{
	return m_redoList.size();
}


QString CFileBasedUndoManagerComp::GetUndoLevelDescription(int stepIndex) const
{
	if ((stepIndex > 0) && (stepIndex <= m_undoList.size())){
		return m_undoList[m_undoList.size() - stepIndex].description;
	}

	return "";
}


QString CFileBasedUndoManagerComp::GetRedoLevelDescription(int stepIndex) const
{
	if ((stepIndex > 0) && (stepIndex <= m_redoList.size())){
		return m_redoList[m_redoList.size() - stepIndex].description;
	}

	return "";
}


// reimplemented (imtdoc::IPersistentUndoManager)

void CFileBasedUndoManagerComp::Initialize(const QByteArray& documentId, const QByteArray& documentTypeId)
{
	Q_ASSERT(!m_isInitialized);
	if (m_isInitialized){
		return;
	}

	m_isInitialized = true;

	m_documentId = documentId;
	m_documentTypeId = documentTypeId;

	if (m_undoManagerFilePathCompPtr.IsValid()){
		QString computedFilePath = GetUndoManagerFilePath();
		if (!computedFilePath.isEmpty()){
			m_undoManagerFilePathCompPtr->SetPath(computedFilePath);
		}
	}

	if (m_undoManagerPersistenceCompPtr.IsValid()){
		const QString filePath = GetUndoManagerFilePath();
		if (!filePath.isEmpty()){
			m_undoManagerPersistenceCompPtr->LoadFromFile(*this, filePath);

			m_modelObserver.RegisterObject(this, &CFileBasedUndoManagerComp::OnUndoManagerStateChanged);
		}
	}
}

void CFileBasedUndoManagerComp::ResetUndo()
{
	istd::CChangeNotifier notifier(this);
	Q_UNUSED(notifier);

	m_undoList.clear();
	m_redoList.clear();
	m_beginStatePtr.Reset();
	m_currentState.statePtr.Reset();
	m_currentState.description.clear();
}


bool CFileBasedUndoManagerComp::DoUndo(int steps)
{
	return DoListShift(steps, m_undoList, m_redoList);
}


bool CFileBasedUndoManagerComp::DoRedo(int steps)
{
	return DoListShift(steps, m_redoList, m_undoList);
}


// reimplemented (imod::IObserver)

bool CFileBasedUndoManagerComp::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	if (BaseClass2::OnModelAttached(modelPtr, changeMask)){
		m_hasStoredDocumentState = false;
		m_storedStateArchive.Reset();
		m_currentState.statePtr.Reset();
		m_currentState.description.clear();

		m_stateChangedFlag = DCF_UNKNOWN;

		return true;
	}

	return false;
}


bool CFileBasedUndoManagerComp::OnModelDetached(imod::IModel* modelPtr)
{
	if (BaseClass2::OnModelDetached(modelPtr)){
		m_hasStoredDocumentState = false;
		m_storedStateArchive.Reset();
		m_currentState.statePtr.Reset();
		m_currentState.description.clear();

		m_stateChangedFlag = DCF_UNKNOWN;

		return true;
	}

	return false;
}


// reimplemented (iser::ISerializable)

bool CFileBasedUndoManagerComp::Serialize(iser::IArchive& archive)
{
	if (m_isDestroying){
		return true;
	}

	static iser::CArchiveTag undoStepsTag("UndoSteps", "List of stored undo steps", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag undoStepTag("UndoStep", "Single undo step", iser::CArchiveTag::TT_GROUP, &undoStepsTag);
	static iser::CArchiveTag redoStepsTag("RedoSteps", "List of stored redo steps", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag redoStepTag("RedoStep", "Single redo step", iser::CArchiveTag::TT_GROUP, &redoStepsTag);
	static iser::CArchiveTag hasCurrentStateTag("HasCurrentState", "Flag indicating whether the current observed document state is stored", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag currentStateTag("CurrentState", "Stored current observed document state", iser::CArchiveTag::TT_GROUP);
	static iser::CArchiveTag currentStateDescriptionTag("Description", "Human readable current state description", iser::CArchiveTag::TT_LEAF, &currentStateTag);
	static iser::CArchiveTag currentStateDataTag("State", "Stored current state data", iser::CArchiveTag::TT_GROUP, &currentStateTag);
	static iser::CArchiveTag stepDescriptionTag("Description", "Human readable step description", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag stepStateTag("State", "Stored document state", iser::CArchiveTag::TT_GROUP);

	UndoList& undoList = m_undoList;
	UndoList& redoList = m_redoList;

	bool retVal = true;
	auto serializeStep = [&](UndoStepInfo& step, const iser::CArchiveTag& containerTag, const iser::CArchiveTag& descriptionTag, const iser::CArchiveTag& stateTag)->bool {
		FileUndoState* fileStatePtr = static_cast<FileUndoState*>(step.statePtr.GetPtr());
		if (fileStatePtr == NULL){
			return false;
		}

		bool localRetVal = true;
		localRetVal = localRetVal && archive.BeginTag(containerTag);
		localRetVal = localRetVal && archive.TagAndProcess(descriptionTag, step.description);
		localRetVal = localRetVal && archive.BeginTag(stateTag);
		localRetVal = localRetVal && fileStatePtr->Serialize(archive);
		localRetVal = localRetVal && archive.EndTag(stateTag);
		localRetVal = localRetVal && archive.EndTag(containerTag);

		return localRetVal;
	};

	if (archive.IsStoring()){
		m_currentState.statePtr.Reset();
		m_currentState.description.clear();

		iser::ISerializable* observedObjectPtr = GetObservedObject();
		if (observedObjectPtr != NULL){
			IUndoState* currentStatePtr = CreateState(*observedObjectPtr, "CurrentState");
			if (currentStatePtr != NULL){
				m_currentState.statePtr.SetPtr(currentStatePtr);
			}
		}

		bool hasCurrentState = m_currentState.statePtr.IsValid();

		retVal = retVal && archive.TagAndProcess(hasCurrentStateTag, hasCurrentState);

		int undoStepsCount = undoList.size();
		retVal = retVal && archive.BeginMultiTag(undoStepsTag, undoStepTag, undoStepsCount);
		for (int i = 0; (i < undoStepsCount) && retVal; ++i){
			retVal = retVal && serializeStep(undoList[i], undoStepTag, stepDescriptionTag, stepStateTag);
		}
		retVal = retVal && archive.EndTag(undoStepsTag);

		int redoStepsCount = redoList.size();
		retVal = retVal && archive.BeginMultiTag(redoStepsTag, redoStepTag, redoStepsCount);
		for (int i = 0; (i < redoStepsCount) && retVal; ++i){
			retVal = retVal && serializeStep(redoList[i], redoStepTag, stepDescriptionTag, stepStateTag);
		}
		retVal = retVal && archive.EndTag(redoStepsTag);

		if (retVal && hasCurrentState){
			retVal = retVal && archive.BeginTag(currentStateTag);
			retVal = retVal && archive.TagAndProcess(currentStateDescriptionTag, m_currentState.description);
			retVal = retVal && archive.BeginTag(currentStateDataTag);
			FileUndoState* currentFileStatePtr = static_cast<FileUndoState*>(m_currentState.statePtr.GetPtr());
			retVal = retVal && currentFileStatePtr->Serialize(archive);
			retVal = retVal && archive.EndTag(currentStateDataTag);
			retVal = retVal && archive.EndTag(currentStateTag);
		}
	}
	else{
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		bool hasCurrentState = false;
		retVal = retVal && archive.TagAndProcess(hasCurrentStateTag, hasCurrentState);

		int undoStepsCount = 0;
		retVal = retVal && archive.BeginMultiTag(undoStepsTag, undoStepTag, undoStepsCount);
		if (!retVal){
			return false;
		}

		undoList.clear();
		redoList.clear();
		m_currentState.statePtr.Reset();
		m_currentState.description.clear();

		for (int i = 0; (i < undoStepsCount) && retVal; ++i){
			UndoStepInfo step;

			FileUndoState* fileStatePtr = new FileUndoState(QString());
			step.statePtr.SetPtr(fileStatePtr);

			retVal = retVal && archive.BeginTag(undoStepTag);
			retVal = retVal && archive.TagAndProcess(stepDescriptionTag, step.description);
			retVal = retVal && archive.BeginTag(stepStateTag);
			retVal = retVal && fileStatePtr->Serialize(archive);
			retVal = retVal && archive.EndTag(stepStateTag);
			retVal = retVal && archive.EndTag(undoStepTag);

			if (retVal){
				undoList.push_back(UndoStepInfo());
				undoList.back().description = step.description;
				undoList.back().statePtr.TakeOver(step.statePtr);
			}
		}
		retVal = retVal && archive.EndTag(undoStepsTag);

		int redoStepsCount = 0;
		retVal = retVal && archive.BeginMultiTag(redoStepsTag, redoStepTag, redoStepsCount);
		if (!retVal){
			return false;
		}
		for (int i = 0; (i < redoStepsCount) && retVal; ++i){
			UndoStepInfo step;

			FileUndoState* fileStatePtr = new FileUndoState(QString());
			step.statePtr.SetPtr(fileStatePtr);

			retVal = retVal && archive.BeginTag(redoStepTag);
			retVal = retVal && archive.TagAndProcess(stepDescriptionTag, step.description);
			retVal = retVal && archive.BeginTag(stepStateTag);
			retVal = retVal && fileStatePtr->Serialize(archive);
			retVal = retVal && archive.EndTag(stepStateTag);
			retVal = retVal && archive.EndTag(redoStepTag);

			if (retVal){
				redoList.push_back(UndoStepInfo());
				redoList.back().description = step.description;
				redoList.back().statePtr.TakeOver(step.statePtr);
			}
		}
		retVal = retVal && archive.EndTag(redoStepsTag);

		if (retVal && hasCurrentState){
			FileUndoState* fileStatePtr = new FileUndoState(QString());
			m_currentState.statePtr.SetPtr(fileStatePtr);
			retVal = retVal && archive.BeginTag(currentStateTag);
			retVal = retVal && archive.TagAndProcess(currentStateDescriptionTag, m_currentState.description);
			retVal = retVal && archive.BeginTag(currentStateDataTag);
			retVal = retVal && fileStatePtr->Serialize(archive);
			retVal = retVal && archive.EndTag(currentStateDataTag);
			retVal = retVal && archive.EndTag(currentStateTag);
		}

		if (retVal && m_currentState.statePtr.IsValid()){
			RestoreObservedObject(*m_currentState.statePtr);
		}
	}

	return retVal;
}


// protected methods

bool CFileBasedUndoManagerComp::DoListShift(int steps, UndoList& fromList, UndoList& toList)
{
	bool retVal = false;

	if ((steps > 0) && (fromList.size() >= steps)){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		iser::ISerializable* objectPtr = GetObservedObject();

		if (objectPtr != NULL){
			Q_ASSERT(!m_isBlocked);
			m_isBlocked = true;

			istd::CChangeNotifier objectNotifier(objectPtr, &s_undoChangeSet);
			Q_UNUSED(objectNotifier);

			QString* lastDescriptionPtr = NULL;

			QString targetPrefix = (&toList == &m_undoList) ? "Undo_" : "Redo_";
			QString targetFileName = QString("%1%2").arg(targetPrefix).arg(toList.size());
			UndoStatePtr currentStatePtr(CreateState(*objectPtr, targetFileName));
			if (currentStatePtr.IsValid()){
				toList.push_back(UndoStepInfo());

				UndoStepInfo& currentStep = toList.back();

				currentStep.statePtr.TakeOver(currentStatePtr);
				lastDescriptionPtr = &currentStep.description;
			}

			const UndoStatePtr& sourceStatePtr = fromList[fromList.size() - steps].statePtr;
			Q_ASSERT(sourceStatePtr.IsValid());

			if (RestoreState(*sourceStatePtr, *objectPtr)){
				for (int i = 1; i < steps; ++i){
					if (lastDescriptionPtr != NULL){
						*lastDescriptionPtr = fromList.back().description;
					}

					toList.push_back(UndoStepInfo());
					toList.back().statePtr.TakeOver(fromList.back().statePtr);
					lastDescriptionPtr = &toList.back().description;

					fromList.pop_back();
				}

				if (lastDescriptionPtr != NULL){
					*lastDescriptionPtr = fromList.back().description;
				}

				fromList.pop_back();

				retVal = true;
			}

			objectNotifier.Reset();

			m_isBlocked = false;
		}
	}

	return retVal;
}


CFileBasedUndoManagerComp::IUndoState* CFileBasedUndoManagerComp::CreateState(iser::ISerializable& object, const QString& stepFileName)
{
	if (!m_documentPersistenceCompPtr.IsValid() || !m_rootFolderCompPtr.IsValid() || m_documentId.isEmpty()){
		return NULL;
	}

	QString filePath = CreateStepFilePath(stepFileName);
	if (filePath.isEmpty()){
		return NULL;
	}

	QDir directory(GetStorageDirectoryPath());
	while (QFile::exists(filePath)){
		filePath = directory.filePath(QString("%1_%2").arg(stepFileName).arg(m_uniqueFileCounter++));
		QString extension = GetPersistenceFileExtension(m_documentPersistenceCompPtr.GetPtr());
		if (!extension.isEmpty()){
			filePath += "." + extension;
		}
	}

	if (m_documentPersistenceCompPtr->SaveToFile(object, filePath) != ifile::IFilePersistence::OS_OK){
		return NULL;
	}

	return new FileUndoState(filePath);
}


bool CFileBasedUndoManagerComp::RestoreState(const IUndoState& state, iser::ISerializable& object)
{
	if (!m_documentPersistenceCompPtr.IsValid()){
		return false;
	}

	const FileUndoState& fileState = static_cast<const FileUndoState&>(state);

	return m_documentPersistenceCompPtr->LoadFromFile(object, fileState.GetFilePath()) == ifile::IFilePersistence::OS_OK;
}


bool CFileBasedUndoManagerComp::AreStatesEqual(const IUndoState& state1, const IUndoState& state2) const
{
	const FileUndoState& fileState1 = static_cast<const FileUndoState&>(state1);
	const FileUndoState& fileState2 = static_cast<const FileUndoState&>(state2);

	QFile file1(fileState1.GetFilePath());
	QFile file2(fileState2.GetFilePath());

	if (!file1.open(QIODevice::ReadOnly) || !file2.open(QIODevice::ReadOnly)){
		return false;
	}

	if (file1.size() != file2.size()){
		return false;
	}

	QCryptographicHash hash1(QCryptographicHash::Sha256);
	QCryptographicHash hash2(QCryptographicHash::Sha256);

	if (!hash1.addData(&file1) || !hash2.addData(&file2)){
		return false;
	}

	return hash1.result() == hash2.result();
}


bool CFileBasedUndoManagerComp::RestoreObservedObject(const IUndoState& state)
{
	iser::ISerializable* objectPtr = GetObservedObject();
	if (objectPtr == NULL){
		return false;
	}

	Q_ASSERT(!m_isBlocked);
	m_isBlocked = true;

	istd::CChangeNotifier objectNotifier(objectPtr, &s_undoChangeSet);
	Q_UNUSED(objectNotifier);

	bool retVal = RestoreState(state, *objectPtr);

	objectNotifier.Reset();

	m_isBlocked = false;

	return retVal;
}


QString CFileBasedUndoManagerComp::CreateStepFilePath(const QString& stepFileName) const
{
	QString storageDirectoryPath = GetStorageDirectoryPath();
	if (storageDirectoryPath.isEmpty()){
		return QString();
	}

	QDir directory(storageDirectoryPath);
	if (!directory.exists()){
		if (!directory.mkpath(".")){
			return QString();
		}
	}

	QString filePath = directory.filePath(stepFileName);
	QString extension = GetPersistenceFileExtension(m_documentPersistenceCompPtr.GetPtr());
	if (!extension.isEmpty()){
		filePath += "." + extension;
	}

	return filePath;
}


QString CFileBasedUndoManagerComp::GetStorageDirectoryPath() const
{
	if (!m_rootFolderCompPtr.IsValid() || m_documentId.isEmpty()){
		return QString();
	}

	QDir directory(m_rootFolderCompPtr->GetPath());

	if (!m_documentTypeId.isEmpty()){
		return directory.filePath(QString::fromUtf8(m_documentTypeId) + "/" + QString::fromUtf8(m_documentId));
	}

	return directory.filePath(QString::fromUtf8(m_documentId));
}


QString CFileBasedUndoManagerComp::GetPersistenceFileExtension(ifile::IFilePersistence* persistencePtr) const
{
	if (persistencePtr == NULL){
		return QString();
	}

	QStringList extensions;
	if (!persistencePtr->GetFileExtensions(extensions, nullptr, ifile::IFileTypeInfo::QF_SAVE | ifile::IFileTypeInfo::QF_FILE, false)){
		return QString();
	}

	if (extensions.isEmpty()){
		return QString();
	}

	QString extension = extensions.first().trimmed();
	while (extension.startsWith('*') || extension.startsWith('.')){
		extension.remove(0, 1);
	}

	return extension;
}


QString CFileBasedUndoManagerComp::GetUndoManagerFilePath() const
{
	QString storageDirectoryPath = GetStorageDirectoryPath();
	if (storageDirectoryPath.isEmpty()){
		return QString();
	}

	QDir storageDirectory(storageDirectoryPath);

	QString undoManagerFileName = "UndoManager";
	QString extension = GetPersistenceFileExtension(m_undoManagerPersistenceCompPtr.GetPtr());
	if (!extension.isEmpty()){
		undoManagerFileName += "." + extension;
	}

	return storageDirectory.filePath(undoManagerFileName);
}


void CFileBasedUndoManagerComp::OnUndoManagerStateChanged(
	const istd::IChangeable::ChangeSet& /*changeSet*/,
	const istd::IChangeable* /*objectPtr*/)
{
	if (!m_undoManagerPersistenceCompPtr.IsValid()){
		return;
	}

	const QString filePath = GetUndoManagerFilePath();
	if (!filePath.isEmpty()){
		m_undoManagerPersistenceCompPtr->SaveToFile(*this, filePath);
	}
}


// reimplemented (imod::TSingleModelObserverBase<iser::ISerializable>)

iser::ISerializable* CFileBasedUndoManagerComp::CastFromModel(imod::IModel* modelPtr) const
{
	return CompCastPtr<iser::ISerializable>(modelPtr);
}


// reimplemented (imod::IObserver)

void CFileBasedUndoManagerComp::BeforeUpdate(imod::IModel* modelPtr)
{
	BaseClass2::BeforeUpdate(modelPtr);

	if (m_isDestroying){
		return;
	}

	if (!m_isBlocked && !m_beginStatePtr.IsValid() && modelPtr != nullptr){
		iser::ISerializable* objectPtr = GetObservedObject();
		if (objectPtr != NULL){
			QString stateFileName = QString("Undo_%1").arg(m_undoList.size());
			UndoStatePtr statePtr(CreateState(*objectPtr, stateFileName));

			if (		statePtr.IsValid() &&
						(m_undoList.isEmpty() || !AreStatesEqual(*statePtr, *(m_undoList.back().statePtr)))){
				m_beginStatePtr.TakeOver(statePtr);
			}
		}
	}
}


void CFileBasedUndoManagerComp::AfterUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet)
{
	if (m_isDestroying){
		BaseClass2::AfterUpdate(modelPtr, changeSet);
		return;
	}

	Q_ASSERT(!changeSet.IsEmpty());

	m_isStateChangedFlagValid = false;

	bool skipUndo = changeSet.ContainsExplicit(istd::IChangeable::CF_NO_UNDO, true);

	if (		!m_isBlocked &&
				!skipUndo &&
				m_beginStatePtr.IsValid()){
		iser::ISerializable* objectPtr = GetObservedObject();
		if (objectPtr != NULL){
			QString stateFileName = QString("Undo_%1").arg(m_undoList.size());
			UndoStatePtr statePtr(CreateState(*objectPtr, stateFileName));

			if (statePtr.IsValid()){
				if (!AreStatesEqual(*statePtr, *m_beginStatePtr)){
					istd::CChangeNotifier notifier(this);
					Q_UNUSED(notifier);

					m_undoList.push_back(UndoStepInfo());
					m_undoList.back().statePtr.TakeOver(m_beginStatePtr);
					m_undoList.back().description = changeSet.GetDescription();

					m_redoList.clear();
				}
			}
			else{
				qDebug("Undo Manager: Object serialization failed");
			}
		}

		m_beginStatePtr.Reset();
	}

	BaseClass2::AfterUpdate(modelPtr, changeSet);
}


// reimplemented (idoc::IDocumentStateComparator)

bool CFileBasedUndoManagerComp::HasStoredDocumentState() const
{
	return m_hasStoredDocumentState;
}


bool CFileBasedUndoManagerComp::StoreDocumentState()
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


bool CFileBasedUndoManagerComp::RestoreDocumentState()
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

		m_undoList.clear();
		m_redoList.clear();
	}

	return false;
}


idoc::IDocumentStateComparator::DocumentChangeFlag CFileBasedUndoManagerComp::GetDocumentChangeFlag() const
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

void CFileBasedUndoManagerComp::OnComponentDestroyed()
{
	m_isDestroying = true;
	m_modelObserver.UnregisterAllObjects();

	EnsureModelDetached();

	QString storageDirectoryPath = GetStorageDirectoryPath();
	if (!storageDirectoryPath.isEmpty()){
		QDir storageDirectory(storageDirectoryPath);
		if (storageDirectory.exists()){
			storageDirectory.removeRecursively();
		}
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtdoc
