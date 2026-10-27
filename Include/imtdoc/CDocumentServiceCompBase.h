// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>
#include <ilog/TLoggerCompWrap.h>

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
	I_END_COMPONENT

protected:
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

protected:
	I_MULTIREF(imtdoc::IDocumentServiceEventHandler, m_handlerCompPtr);
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_FACT(idoc::IUndoManager, m_undoManagerFactPtr);
	I_FACT(imtdoc::IPersistentUndoManager, m_persistUndoManagerFactPtr);
	I_MULTIREF(imtdoc::IDocumentNameProvider, m_documentNameProviderCompPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactListCompPtr);
	I_MULTIREF(imtdoc::IDocumentValidator, m_documentValidatorCompPtr);
	I_MULTIREF(imtdoc::IDocumentInitDelegate, m_documentInitDelegateCompPtr);
};


} // namespace imtdoc
