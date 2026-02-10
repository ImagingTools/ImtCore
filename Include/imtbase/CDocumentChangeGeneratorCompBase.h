// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtbase/IDocumentChangeGenerator.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/COperationDescription.h>


namespace imtbase
{


class CDocumentChangeGeneratorCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IDocumentChangeGenerator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CDocumentChangeGeneratorCompBase);
		I_REGISTER_INTERFACE(imtbase::IDocumentChangeGenerator);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	QByteArray InsertOperationDescription(
		imtbase::CObjectCollection& documentChangeCollection,
		const QByteArray& operationTypeId,
		const QByteArray& key,
		const QString& keyName,
		const QByteArray& oldValue = QByteArray(),
		const QByteArray& newValue = QByteArray()) const;
	void GenerateChanges(
		const QByteArrayList& oldValueList,
		const QByteArrayList& newValueList,
		QByteArrayList& addedValueList,
		QByteArrayList& removedValueList) const;
	bool IsUuid(const QByteArray& data) const;

	virtual bool CompareDocuments(
		const istd::IChangeable& oldDocument,
		const istd::IChangeable& newDocument,
		CObjectCollection& documentChangeCollection,
		QString& errorMessage) = 0;
	virtual QString CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId = QByteArray()) const;

	// reimplemented (imtbase::IDocumentChangeGenerator)
	virtual bool GenerateDocumentChanges(
				const QByteArray& operationTypeId,
				const QByteArray& documentId,
				const istd::IChangeable* documentPtr,
				CObjectCollection& documentChangeCollection,
				QString& errorMessage,
				const iprm::IParamsSet* paramsPtr) override;
	virtual QString GetOperationDescription(CObjectCollection& documentChangeCollection, const QByteArray& languageId = QByteArray()) override;
	virtual QString GetKeyNameForOperation(const QByteArray& key, const QByteArray& value) const;

private:
	imtbase::COperationDescription* CreateOperationDescription(
		const QByteArray& operationTypeId,
		const QByteArray& key,
		const QString& keyName,
		const QByteArray& oldValue,
		const QByteArray& newValue) const;

protected:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtbase


