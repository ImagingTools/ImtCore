// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <atomic>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QMutex>

// ACF includes
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtdoc/CDocumentServiceBase.h>
#include <imtdoc/IDocumentInitDelegate.h>
#include <imtdoc/IDocumentNameProvider.h>
#include <imtdoc/IPersistentUndoManager.h>
#include <imtdoc/IDocumentValidator.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief ACF component base for single-collection document services.

	\c CDocumentServiceCompBase extends \c CDocumentServiceBase with the ACF
	component wiring needed to use it as a concrete server component.  It
	provides:

	- **Object factories** — a positionally-matched list of
	  \c "ObjectFactories" (factories) and \c "ObjectTypeIdList" (type IDs)
	  that supply document data objects.
	- **Undo manager factory** — a single \c "UndoManager" factory used to
	  create per-document undo managers.
	- **Name providers** — an optional \c "DocumentNameProviders" list
	  (one per type-ID) that computes default document names from document
	  data.
	- **Validators** — an optional \c "DocumentValidators" list (one per
	  type-ID) that validates document data before saving.
	- **Event handlers** — an optional \c "DocumentServiceEventHandler"
	  multi-reference list of \c IDocumentServiceEventHandler components
	  wired through the ACF component descriptor.  At runtime, these are
	  merged with handlers registered via
	  \c IDocumentService::RegisterEventHandler.
	- **State persistence** — optional \c "StateFilePath" and
	  \c "StatePersistenceEnabler".  When enabled, the list of opened
	  documents is restored from the JSON state file on component creation
	  and saved to it after each change of the service data model.  The data
	  object of a restored document is created by \c CreateObjectFromSourceRef
	  or, if it fails, by \c CreateObject.  If the state file cannot be read,
	  the persisted data of all documents is removed
	  (\c IPersistentUndoManager::RemoveAllDocumentContexts).

	\note This class is an abstract base; it does not implement \c GetCollection
	or \c IsSingleCopyMode.  Use \c CCollectionDocumentServiceComp for a
	complete, ready-to-use component.
*/
class CDocumentServiceCompBase: public ilog::CLoggerComponentBase, public imtdoc::CDocumentServiceBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imtdoc::CDocumentServiceBase BaseClass2;

	I_BEGIN_BASE_COMPONENT(CDocumentServiceCompBase)
		I_ASSIGN_MULTI_0(m_handlerCompPtr, "DocumentServiceEventHandler", "Document service event handler", false);
		I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIdList", "List of the object type-IDs according to the list of factories ('ObjectFactories')", true);
		I_ASSIGN(m_undoManagerFactPtr, "UndoManager", "Factory of the undo manager", false, "UndoManager");
		I_ASSIGN_TO(m_persistUndoManagerFactPtr, m_undoManagerFactPtr, false);
		I_ASSIGN_MULTI_0(m_documentNameProviderCompPtr, "DocumentNameProviders", "List of document name providers related to registered type-IDs ('ObjectTypeIdList')", false);
		I_ASSIGN_MULTI_0(m_objectFactListCompPtr, "ObjectFactories", "List of object factories related to registered type-IDs ('ObjectTypeIdList')", true);
		I_ASSIGN_MULTI_0(m_documentValidatorCompPtr, "DocumentValidators", "List of document data validators related to registered type-IDs ('ObjectTypeIdList')", false);
		I_ASSIGN_MULTI_0(m_documentInitDelegateCompPtr, "DocumentInitDelegates", "List of document initialization delegates related to registered type-IDs ('ObjectTypeIdList')", false);
		I_ASSIGN(m_stateFilePathCompPtr, "StateFilePath", "File path used to save/restore the list of opened documents", false, "StateFilePath");
		I_ASSIGN(m_statePersistenceEnablerCompPtr, "StatePersistenceEnabler", "Enables restoring of opened documents on component creation and saving on each data model change", false, "StatePersistenceEnabler");
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (istd::IChangeable)
	virtual void OnEndChanges(const ChangeSet& changeSet) override;

	// reimplemented (imtdoc::CDocumentServiceBase)
	virtual QString GetDefaultDocumentName(const WorkingDocument& document) const override;
	virtual bool HasDocumentNameProvider(const QByteArray& typeId) const override;
	virtual bool ValidateDocumentData(
				const WorkingDocument& document,
				OperationStatus& status,
				QString* errorMessage = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QList<imtdoc::IDocumentServiceEventHandler*> GetDocumentServiceEventHandlers() const override;
	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId) const override;
	virtual idoc::IUndoManagerUniquePtr CreateUndoManager() const override;
	virtual bool OnDocumentCreated(const QByteArray& typeId, const iprm::IParamsSet* initParams, istd::IChangeable& document, QString& errorMessage) override;

private:
	// Reserve 170000000-170000099 for imtdoc validation warnings to avoid collisions with other message IDs.
	static constexpr int kValidationFailureWarningId = 170000000;
	static constexpr int kValidationFailureWithoutMessageWarningId = 170000001;

	int GetObjectFactoryIndex(const QByteArray& typeId) const;
	const imtdoc::IDocumentNameProvider* GetDocumentNameProvider(const QByteArray& typeId) const;
	const imtdoc::IDocumentValidator* GetDocumentValidator(const QByteArray& typeId) const;
	imtdoc::IDocumentInitDelegate* GetDocumentInitDelegate(const QByteArray& typeId) const;

	struct DocumentInfo
	{
		QByteArray documentId;
		QByteArray typeId;
		QByteArray documentSourceRef;
		QString name;
		bool isReadOnly = false;
		bool singleDocumentInstance = false;
	};
	typedef QMap<QByteArray, DocumentInfo> DocumentInfoMap;

	struct ServiceState
	{
		DocumentInfoMap documents; ///< documentId → document description.
		UserDocumentIdsMap userDocumentIds; ///< userId → IDs of the documents opened by the user.
	};

	bool LoadFromFile(const QString& filePath);
	bool SaveToFile(const QString& filePath) const;
	void PrepareServiceState(ServiceState& state) const;
	static QJsonObject ServiceStateToJson(const ServiceState& state);
	static bool ServiceStateFromJson(const QJsonObject& rootObject, ServiceState& state);
	bool IsStatePersistenceEnabled() const;
	QString GetStateFilePath() const;
	/**
		Remove persisted data (undo/redo history, unsaved changes) of all documents.
		Used when the state file cannot be read, since the data cannot be associated to any document.
	*/
	void RemovePersistedDocumentData() const;
	bool RestoreServiceState(const ServiceState& state);
	void RemoveAllDocuments();
	void RequestSaveToFile(const QString& filePath);
	void ProcessSaveRequests();
	bool RestoreSharedDocument(const DocumentInfo& documentInfo, const QByteArray& userId);
	bool RestoreDocument(const DocumentInfo& documentInfo, const QByteArray& userId);

protected:
	I_MULTIREF(imtdoc::IDocumentServiceEventHandler, m_handlerCompPtr);
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_FACT(idoc::IUndoManager, m_undoManagerFactPtr);
	I_FACT(imtdoc::IPersistentUndoManager, m_persistUndoManagerFactPtr);
	I_MULTIREF(imtdoc::IDocumentNameProvider, m_documentNameProviderCompPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactListCompPtr);
	I_MULTIREF(imtdoc::IDocumentValidator, m_documentValidatorCompPtr);
	I_MULTIREF(imtdoc::IDocumentInitDelegate, m_documentInitDelegateCompPtr);
	I_REF(ifile::IFileNameParam, m_stateFilePathCompPtr);
	I_REF(iprm::IEnableableParam, m_statePersistenceEnablerCompPtr);

private:
	std::atomic<bool> m_isStateLoading{false};
	mutable QMutex m_stateFileMutex;
	mutable QMutex m_saveRequestMutex;
	QString m_lastRequestedStateFilePath;
	bool m_isSaveRequestProcessingQueued = false;
};


} // namespace imtdoc
