// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDocumentDataController.h>


namespace imtqml
{


CDocumentDataController::CDocumentDataController(QObject* parent)
	: BaseClass(parent)
{
}


CDocumentDataController::~CDocumentDataController() = default;


const QString& CDocumentDataController::GetDocumentId() const
{
	return m_documentId;
}


void CDocumentDataController::SetDocumentId(const QString& id)
{
	if (m_documentId != id) {
		m_documentId = id;
		Q_EMIT documentIdChanged(m_documentId);
	}
}


const QString& CDocumentDataController::GetDocumentName() const
{
	return m_documentName;
}


void CDocumentDataController::SetDocumentName(const QString& name)
{
	if (m_documentName != name) {
		m_documentName = name;
		Q_EMIT documentNameChanged(m_documentName);
	}
}


const QString& CDocumentDataController::GetDocumentDescription() const
{
	return m_documentDescription;
}


void CDocumentDataController::SetDocumentDescription(const QString& description)
{
	if (m_documentDescription != description) {
		m_documentDescription = description;
		Q_EMIT documentDescriptionChanged(m_documentDescription);
	}
}


const QString& CDocumentDataController::GetTypeId() const
{
	return m_typeId;
}


void CDocumentDataController::SetTypeId(const QString& typeId)
{
	if (m_typeId != typeId) {
		m_typeId = typeId;
		Q_EMIT typeIdChanged(m_typeId);
	}
}


bool CDocumentDataController::GetHasRemoteChanges() const
{
	return m_hasRemoteChanges;
}


void CDocumentDataController::SetHasRemoteChanges(bool value)
{
	if (m_hasRemoteChanges != value) {
		m_hasRemoteChanges = value;
		Q_EMIT hasRemoteChangesChanged(m_hasRemoteChanges);
	}
}


const QVariant& CDocumentDataController::GetDocumentModel() const
{
	return m_documentModel;
}


void CDocumentDataController::SetDocumentModel(const QVariant& model)
{
	m_documentModel = model;
	// Mirror the QML base behaviour: clear remote-change flag and re-emit.
	SetHasRemoteChanges(false);
	Q_EMIT documentModelChanged();
	Q_EMIT modelChanged();
}


// public Q_SLOTS

QString CDocumentDataController::getDocumentId() const
{
	return m_documentId;
}


QString CDocumentDataController::getDocumentName() const
{
	return m_documentName;
}


QString CDocumentDataController::getDocumentTypeId() const
{
	return m_typeId;
}


QVariant CDocumentDataController::getDocumentModel() const
{
	return m_documentModel;
}


QString CDocumentDataController::getDocumentDescription() const
{
	return m_documentDescription;
}


void CDocumentDataController::updateDocumentModel()
{
	Q_EMIT modelChanged();
}


void CDocumentDataController::insertDocument()
{
	Q_EMIT saved(m_documentId, m_documentName);
}


void CDocumentDataController::saveDocument()
{
	Q_EMIT saved(m_documentId, m_documentName);
}


} // namespace imtqml
