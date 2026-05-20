// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdoc/CDocumentServiceBase.h>
#include <imtdoc/IDocumentNameProvider.h>
#include <imtdoc/IDocumentValidator.h>


namespace imtdoc
{


class CCollectionDocumentServiceComp: public ilog::CLoggerComponentBase, virtual public CDocumentServiceBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef CDocumentServiceBase BaseClass2;

	I_BEGIN_COMPONENT(CCollectionDocumentServiceComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentService)
		I_ASSIGN_MULTI_0(m_handlerCompPtr, "DocumentServiceEventHandler", "Document service event handler", false);
		I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIdList", "List of the object type-IDs according to the list of factories ('ObjectFactories')", true);
		I_ASSIGN(m_undoManagerFactPtr, "UndoManager", "Factory of the undo manager", false, "UndoManager");
		I_ASSIGN_MULTI_0(m_documentNameProviderCompPtr, "DocumentNameProviders", "List of document name providers related to registered type-IDs ('ObjectTypeIdList')", false);
		I_ASSIGN_MULTI_0(m_objectFactListCompPtr, "ObjectFactories", "List of object factories related to registered type-IDs ('ObjectTypeIdList')", true);
		I_ASSIGN_MULTI_0(m_documentValidatorCompPtr, "DocumentValidators", "List of document data validators related to registered type-IDs ('ObjectTypeIdList')", false);
		I_ASSIGN(m_collectionCompPtr, "Collection", "Document collection containing related documents", true, "Collection");
		I_ASSIGN(m_isSingleCopyModeAttrPtr, "IsSingleCopyMode", "When enabled, all users share a single copy of each document", false, false);
	I_END_COMPONENT

	// reimplemented (imtdoc::IDocumentService)
	virtual QByteArray CreateNewDocument(
		const QByteArray& userId,
		const QByteArray& documentTypeId,
		const QByteArray& proposedSourceDocumentId = QByteArray()) override;
	virtual QByteArray OpenDocument(const QByteArray& userId, const QUrl& url) override;
	virtual OperationStatus SetDocumentName(const QByteArray& userId, const QByteArray& documentId, const QString& documentName) override;
	virtual OperationStatus SaveDocument(
		const QByteArray& userId,
		const QByteArray& documentId,
		const QString& documentName = QString(),
		QString* errorMessage = nullptr) override;
	virtual OperationStatus CloseDocument(const QByteArray& userId, const QByteArray& documentId) override;

protected:
	// Returns the collection used for document storage.
	// Virtual to allow subclasses (e.g. test doubles) to supply a different collection.
	virtual imtbase::IObjectCollection* GetCollection() const;

	// reimplemented (imtdoc::CDocumentServiceBase)
	virtual bool IsSingleCopyMode() const override;
	virtual QString GetDefaultDocumentName(const WorkingDocument& document) const override;
	virtual bool HasDocumentNameProvider(const QByteArray& typeId) const override;
	virtual bool ValidateDocumentData(
		const WorkingDocument& document,
		OperationStatus& status,
		QString* errorMessage = nullptr) const override;
	virtual QList<imtdoc::IDocumentServiceEventHandler*> GetDocumentServiceEventHandlers() const override;
	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId) const override;
	virtual idoc::IUndoManagerUniquePtr CreateUndoManager() const override;

private:
	// Reserve 170000000-170000099 for imtdoc validation warnings to avoid collisions with other message IDs.
	static constexpr int kValidationFailureWarningId = 170000000;
	static constexpr int kValidationFailureWithoutMessageWarningId = 170000001;

	int GetObjectFactoryIndex(const QByteArray& typeId) const;
	const imtdoc::IDocumentNameProvider* GetDocumentNameProvider(const QByteArray& typeId) const;
	const imtdoc::IDocumentValidator* GetDocumentValidator(const QByteArray& typeId) const;

	QMap<QByteArray, QByteArray> m_proposedSourceDocumentIds;

	I_REF(imtbase::IObjectCollection, m_collectionCompPtr);
	I_ATTR(bool, m_isSingleCopyModeAttrPtr);

protected:
	I_MULTIREF(imtdoc::IDocumentServiceEventHandler, m_handlerCompPtr);
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_FACT(idoc::IUndoManager, m_undoManagerFactPtr);
	I_MULTIREF(imtdoc::IDocumentNameProvider, m_documentNameProviderCompPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactListCompPtr);
	I_MULTIREF(imtdoc::IDocumentValidator, m_documentValidatorCompPtr);
};


} // namespace imtdoc
