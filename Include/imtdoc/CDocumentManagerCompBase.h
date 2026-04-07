// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentManager.h>
#include <idoc/IUndoManager.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdoc/CDocumentManagerBase.h>
#include <imtdoc/IDocumentNameProvider.h>
#include <imtdoc/IDocumentValidator.h>


namespace imtdoc
{


class CDocumentManagerCompBase: public ilog::CLoggerComponentBase, virtual public imtdoc::CDocumentManagerBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imtdoc::CDocumentManagerBase BaseClass2;

	I_BEGIN_BASE_COMPONENT(CDocumentManagerCompBase)
		I_ASSIGN_MULTI_0(m_handlerCompPtr, "DocumentManagerEventHandler", "Document manager event handler", false);
		I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIdList", "List of the object type-IDs according to the list of factories ('ObjectFactories')", true);
		I_ASSIGN(m_undoManagerFactPtr, "UndoManager", "Factory of the undo manager", false, "UndoManager");
		I_ASSIGN_MULTI_0(m_documentNameProviderCompPtr, "DocumentNameProviders", "List of document name providers related to registered type-IDs ('ObjectTypeIdList')", false);
		I_ASSIGN_MULTI_0(m_objectFactListCompPtr, "ObjectFactories", "List of object factories related to registered type-IDs ('ObjectTypeIdList')", true);
		I_ASSIGN_MULTI_0(m_documentValidatorCompPtr, "DocumentValidators", "List of document data validators related to registered type-IDs ('ObjectTypeIdList')", false);
	I_END_COMPONENT

protected:
	// reimplemented (imtdoc::CDocumentManagerBase)
	virtual QString GetDefaultDocumentName(const WorkingDocument& document) const override;
	virtual bool HasDocumentNameProvider(const QByteArray& typeId) const override;
	virtual bool ValidateDocumentData(
		const WorkingDocument& document,
		OperationStatus& status,
		QString* errorMessage = nullptr) const override;
	virtual QList<imtdoc::IDocumentManagerEventHandler*> GetDocumentManagerEventHandlers() const override;
	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId) const override;
	virtual idoc::IUndoManagerUniquePtr CreateUndoManager() const override;

private:
	// Reserve 170000000-170000099 for imtdoc validation warnings to avoid collisions with other message IDs.
	static constexpr int kValidationFailureWarningId = 170000000;
	static constexpr int kValidationFailureWithoutMessageWarningId = 170000001;

	int GetObjectFactoryIndex(const QByteArray& typeId) const;
	const imtdoc::IDocumentNameProvider* GetDocumentNameProvider(const QByteArray& typeId) const;
	const imtdoc::IDocumentValidator* GetDocumentValidator(const QByteArray& typeId) const;

protected:
	I_MULTIREF(imtdoc::IDocumentManagerEventHandler, m_handlerCompPtr);
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_FACT(idoc::IUndoManager, m_undoManagerFactPtr);
	I_MULTIREF(imtdoc::IDocumentNameProvider, m_documentNameProviderCompPtr);
	I_MULTIFACT(istd::IChangeable, m_objectFactListCompPtr);
	I_MULTIREF(imtdoc::IDocumentValidator, m_documentValidatorCompPtr);
};


} // namespace imtdoc
