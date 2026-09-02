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


namespace imtdoc
{


// static attributes
static const istd::IChangeable::ChangeSet s_undoChangeSet(istd::IChangeable::CF_NO_UNDO, "UNDO");


// UndoStep

CFileBasedUndoManagerComp::UndoStep::UndoStep(const QString& filePath)
	: filePath(filePath)
{
}


CFileBasedUndoManagerComp::UndoStep::~UndoStep()
{
	if (autoRemoveOnDestroy && !filePath.isEmpty() && QFile::exists(filePath)){
		QFile::remove(filePath);
	}
}


const QString& CFileBasedUndoManagerComp::UndoStep::GetFilePath() const
{
	return filePath;
}


bool CFileBasedUndoManagerComp::UndoStep::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag filePathTag("FilePath", "Path to the file storing the document state", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag descriptionTag("Description", "Description of the undo step", iser::CArchiveTag::TT_LEAF);

	bool retVal = archive.TagAndProcess(filePathTag, filePath);
	retVal = retVal && archive.TagAndProcess(descriptionTag, description);

	return retVal;
}


void CFileBasedUndoManagerComp::UndoStep::SetAutoRemoveOnDestroy(bool value)
{
	autoRemoveOnDestroy = value;
}


// public methods

CFileBasedUndoManagerComp::CFileBasedUndoManagerComp()
:	m_uniqueFileCounter(0),
	m_isBlocked(false),
	m_isDestroying(false),
	m_isInitialized(false),
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
		return m_undoList[m_undoList.size() - stepIndex]->description;
	}

	return QString();
}


QString CFileBasedUndoManagerComp::GetRedoLevelDescription(int stepIndex) const
{
	if ((stepIndex > 0) && (stepIndex <= m_redoList.size())){
		return m_redoList[m_redoList.size() - stepIndex]->description;
	}

	return QString();
}


// reimplemented (imtdoc::IPersistentUndoManager)

void CFileBasedUndoManagerComp::InitializeDocumentContext(const QByteArray& documentId, const QByteArray& documentTypeId)
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
			if (m_undoManagerPersistenceCompPtr->LoadFromFile(*this, filePath) != ifile::IFilePersistence::OS_OK){
				SendWarningMessage(0, QObject::tr("Failed to load undo manager state from file: ") + filePath);
			}

			m_modelObserver.RegisterObject(this, &CFileBasedUndoManagerComp::OnUndoManagerStateChanged);
		}
	}
}


void CFileBasedUndoManagerComp::CleanupHistory()
{
	istd::CChangeNotifier notifier(this);
	Q_UNUSED(notifier);

	UndoStepPtr newCurrentStatePtr;
	iser::ISerializable* observedObjectPtr = GetObservedObject();
	const QString currentStateFilePath = CreateStepFilePath("CurrentState");
	if (m_currentStatePtr && m_currentStatePtr->GetFilePath() == currentStateFilePath){
		m_currentStatePtr->SetAutoRemoveOnDestroy(false);
	}
	m_currentStatePtr.reset();

	if (observedObjectPtr != nullptr
		&& !currentStateFilePath.isEmpty()
		&& m_documentPersistenceCompPtr.IsValid()
		&& m_documentPersistenceCompPtr->SaveToFile(*observedObjectPtr, currentStateFilePath) == ifile::IFilePersistence::OS_OK){
		newCurrentStatePtr.reset(new UndoStep(currentStateFilePath));
	}

	m_beginStatePtr.reset();
	m_undoList.clear();
	m_redoList.clear();
	m_currentStatePtr = newCurrentStatePtr;
}


void CFileBasedUndoManagerComp::ResetUndo()
{
	istd::CChangeNotifier notifier(this);
	Q_UNUSED(notifier);

	m_undoList.clear();
	m_redoList.clear();
	m_beginStatePtr.reset();
	m_currentStatePtr.reset();
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
		m_currentStatePtr.reset();

		return true;
	}

	return false;
}


bool CFileBasedUndoManagerComp::OnModelDetached(imod::IModel* modelPtr)
{
	if (BaseClass2::OnModelDetached(modelPtr)){
		m_currentStatePtr.reset();

		return true;
	}

	return false;
}


// reimplemented (iser::ISerializable)

bool CFileBasedUndoManagerComp::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(!archive.IsStoring() ? this : nullptr);
	Q_UNUSED(notifier);

	if (m_isDestroying){
		return true;
	}

	static iser::CArchiveTag undoStepsTag("UndoSteps", "List of stored undo steps", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag undoStepTag("UndoStep", "Single undo step", iser::CArchiveTag::TT_GROUP, &undoStepsTag);
	static iser::CArchiveTag redoStepsTag("RedoSteps", "List of stored redo steps", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag redoStepTag("RedoStep", "Single redo step", iser::CArchiveTag::TT_GROUP, &redoStepsTag);
	static iser::CArchiveTag hasCurrentStateTag("HasCurrentState", "Flag indicating whether the current observed document state is stored", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag currentStateTag("CurrentState", "Stored current observed document state", iser::CArchiveTag::TT_GROUP);

	if (archive.IsStoring()){
		m_currentStatePtr.reset();

		iser::ISerializable* observedObjectPtr = GetObservedObject();
		if (observedObjectPtr != NULL){
			UndoStep* currentStatePtr = CreateState(*observedObjectPtr, "CurrentState");
			if (currentStatePtr != NULL){
				m_currentStatePtr.reset(currentStatePtr);
			}
		}
	}
	else{
		m_undoList.clear();
		m_redoList.clear();
	}

	bool retVal = true;
	retVal = retVal && SerializeUndoList(archive, m_undoList, undoStepsTag, undoStepTag);
	retVal = retVal && SerializeUndoList(archive, m_redoList, redoStepsTag, redoStepTag);

	bool hasCurrentState = m_currentStatePtr != nullptr;
	retVal = retVal && archive.TagAndProcess(hasCurrentStateTag, hasCurrentState);
	if (retVal && hasCurrentState){
		if (!archive.IsStoring()){
			m_currentStatePtr.reset(new UndoStep);
		}

		retVal = retVal && archive.BeginTag(currentStateTag);
		retVal = retVal && m_currentStatePtr->Serialize(archive);
		retVal = retVal && archive.EndTag(currentStateTag);
	}

	if (retVal && m_currentStatePtr){
		RestoreObservedObject(*m_currentStatePtr);
	}

	return retVal;
}


// protected methods

bool CFileBasedUndoManagerComp::SerializeUndoList(
	iser::IArchive& archive,
	UndoList& undoList,
	const iser::CArchiveTag& listTag,
	const iser::CArchiveTag& stepTag) const
{
	bool retVal = true;
	int undoStepsCount = undoList.size();

	retVal = retVal && archive.BeginMultiTag(listTag, stepTag, undoStepsCount);
	if (retVal && !archive.IsStoring()){
		undoList.resize(undoStepsCount);
		for (int i = 0; i < undoStepsCount; i++){
			undoList[i].reset(new UndoStep);
		}
	}

	for (int i = 0; i < undoStepsCount; ++i){
		retVal = retVal && archive.BeginTag(stepTag);
		retVal = retVal && undoList[i]->Serialize(archive);
		retVal = retVal && archive.EndTag(stepTag);
	}
	retVal = retVal && archive.EndTag(listTag);

	return retVal;
}


void CFileBasedUndoManagerComp::SetStepFileAutoRemoveEnabled(bool value)
{
	for (const UndoStepPtr& stepPtr : m_undoList){
		if (stepPtr){
			stepPtr->SetAutoRemoveOnDestroy(value);
		}
	}

	for (const UndoStepPtr& stepPtr : m_redoList){
		if (stepPtr){
			stepPtr->SetAutoRemoveOnDestroy(value);
		}
	}

	if (m_beginStatePtr){
		m_beginStatePtr->SetAutoRemoveOnDestroy(value);
	}

	if (m_currentStatePtr){
		m_currentStatePtr->SetAutoRemoveOnDestroy(value);
	}
}


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

			QString targetPrefix = (&toList == &m_undoList) ? "Undo_" : "Redo_";
			QString targetFileName = QString("%1%2").arg(targetPrefix).arg(toList.size());
			UndoStepPtr currentStatePtr(CreateState(*objectPtr, targetFileName));
			if (currentStatePtr){
				toList.push_back(currentStatePtr);
			}

			const UndoStepPtr& sourceStatePtr = fromList[fromList.size() - steps];
			Q_ASSERT(sourceStatePtr);

			if (RestoreState(*sourceStatePtr, *objectPtr)){
				for (int i = 1; i < steps; ++i){
					toList.push_back(UndoStepPtr());
					toList.back() = fromList.back();
					fromList.pop_back();
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


CFileBasedUndoManagerComp::UndoStep* CFileBasedUndoManagerComp::CreateState(iser::ISerializable& object, const QString& stepFileName)
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

	return new UndoStep(filePath);
}


bool CFileBasedUndoManagerComp::RestoreState(const UndoStep& state, iser::ISerializable& object)
{
	if (!m_documentPersistenceCompPtr.IsValid()){
		return false;
	}

	return m_documentPersistenceCompPtr->LoadFromFile(object, state.GetFilePath()) == ifile::IFilePersistence::OS_OK;
}


bool CFileBasedUndoManagerComp::AreStatesEqual(const UndoStep& state1, const UndoStep& state2) const
{
	QFile file1(state1.GetFilePath());
	QFile file2(state2.GetFilePath());

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


bool CFileBasedUndoManagerComp::RestoreObservedObject(const UndoStep& state)
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


// reimplemented (idoc::IDocumentStateComparator)

bool CFileBasedUndoManagerComp::StoreDocumentState()
{
	return BaseClass2::StoreDocumentState();
}


bool CFileBasedUndoManagerComp::RestoreDocumentState()
{
	if (!HasStoredDocumentState()){
		return false;
	}

	Q_ASSERT(!m_isBlocked);
	m_isBlocked = true;
	bool retVal = BaseClass2::RestoreDocumentState();
	m_isBlocked = false;

	if (!retVal){
		m_undoList.clear();
		m_redoList.clear();
	}

	return retVal;
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

	if (!m_isBlocked && !m_beginStatePtr && modelPtr != nullptr){
		iser::ISerializable* objectPtr = GetObservedObject();
		if (objectPtr != NULL){
			QString stateFileName = QString("Undo_%1").arg(m_undoList.size());
			UndoStepPtr statePtr(CreateState(*objectPtr, stateFileName));

			if (statePtr && (m_undoList.isEmpty() || !AreStatesEqual(*statePtr, *m_undoList.back()))){
				m_beginStatePtr = statePtr;
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

	bool skipUndo = changeSet.ContainsExplicit(istd::IChangeable::CF_NO_UNDO, true);

	if (!m_isBlocked && !skipUndo && m_beginStatePtr != nullptr){
		iser::ISerializable* objectPtr = GetObservedObject();
		if (objectPtr != NULL){
			QString stateFileName = QString("Undo_%1").arg(m_undoList.size());
			UndoStepPtr statePtr(CreateState(*objectPtr, stateFileName));

			if (statePtr){
				if (!AreStatesEqual(*statePtr, *m_beginStatePtr)){
					istd::CChangeNotifier notifier(this);
					Q_UNUSED(notifier);

					m_undoList.push_back(UndoStepPtr());
					m_undoList.back() = m_beginStatePtr;
					m_undoList.back()->description = changeSet.GetDescription();

					m_redoList.clear();
				}
			}
			else{
				qDebug("Undo Manager: Object serialization failed");
			}
		}

		m_beginStatePtr.reset();
	}

	BaseClass2::AfterUpdate(modelPtr, changeSet);
}


// reimplemented (icomp::CComponentBase)

void CFileBasedUndoManagerComp::OnComponentDestroyed()
{
	m_isDestroying = true;
	m_modelObserver.UnregisterAllObjects();

	EnsureModelDetached();
	SetStepFileAutoRemoveEnabled(false);

	BaseClass::OnComponentDestroyed();
}


} // namespace imtdoc
