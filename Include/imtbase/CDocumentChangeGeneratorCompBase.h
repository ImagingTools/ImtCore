// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QStringList>

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
	/**
		Canonical values used for boolean attributes. They are translated in the context
		\c Attribute when the change description is rendered.
	*/
	static const QByteArray VALUE_YES;
	static const QByteArray VALUE_NO;

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

	/**
		Read the text of the parameter \a paramId, or an empty string when it is not available.
	*/
	static QString GetTextParamValue(const iprm::IParamsSet* paramsPtr, const QByteArray& paramId);

	/**
		Insert a change entry for an attribute, but only if the old and the new value differ.
		The operation type is derived from the values, see \ref CreateOperationDescription.
		\return True if a change entry was created.
	*/
	bool InsertChange(
		imtbase::CObjectCollection& documentChangeCollection,
		const QByteArray& key,
		const QString& keyName,
		const QByteArray& oldValue,
		const QByteArray& newValue) const;
	bool InsertTextChange(
		imtbase::CObjectCollection& documentChangeCollection,
		const QByteArray& key,
		const QString& keyName,
		const QString& oldValue,
		const QString& newValue) const;
	bool InsertFlagChange(
		imtbase::CObjectCollection& documentChangeCollection,
		const QByteArray& key,
		const QString& keyName,
		bool oldValue,
		bool newValue) const;
	bool InsertNumberChange(
		imtbase::CObjectCollection& documentChangeCollection,
		const QByteArray& key,
		const QString& keyName,
		qint64 oldValue,
		qint64 newValue) const;
	bool InsertDateTimeChange(
		imtbase::CObjectCollection& documentChangeCollection,
		const QByteArray& key,
		const QString& keyName,
		const QDateTime& oldValue,
		const QDateTime& newValue) const;
	/**
		Insert a change entry for an enumeration attribute. \a enumNames is the list returned by
		the \c I_DECLARE_ENUM generated \c <Enum>GetStrings() method.
	*/
	bool InsertEnumChange(
		imtbase::CObjectCollection& documentChangeCollection,
		const QByteArray& key,
		const QString& keyName,
		int oldValue,
		int newValue,
		const QStringList& enumNames) const;
	/**
		Insert one change entry per added and per removed list item. The entries get
		\a addOperationTypeId resp. \a removeOperationTypeId as operation type, so they can be
		rendered by \ref CreateCustomOperationDescription.
	*/
	void InsertListChanges(
		imtbase::CObjectCollection& documentChangeCollection,
		const QByteArray& addOperationTypeId,
		const QByteArray& removeOperationTypeId,
		const QByteArray& key,
		const QString& keyName,
		const QByteArrayList& oldValueList,
		const QByteArrayList& newValueList) const;

	/**
		Translate \a phrase into the language \a languageId using the configured translation manager.
		Returns \a phrase unchanged when no translation is available.
	*/
	QString Translate(const QByteArray& phrase, const QByteArray& languageId, const QByteArray& context) const;

	/**
		Append the differences between the stored document and \a documentPtr to the change collection.
		Operations that carry their own semantics but also modify the document body (binding, transfer, ...)
		call this in addition to their own entries, so no attribute change is lost.
	*/
	bool AppendDocumentComparison(
		const QByteArray& documentId,
		const istd::IChangeable* documentPtr,
		CObjectCollection& documentChangeCollection,
		QString& errorMessage);

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
	QString CreateStandardOperationDescription(
		const imtbase::COperationDescription& operationDescription,
		const QByteArray& languageId) const;

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


