// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDocumentRepresentationController.h>


// Qt includes
#include <QtCore/QDebug>


namespace imtqml
{


// public methods

CDocumentRepresentationController::CDocumentRepresentationController(QObject* parent)
	:BaseClass(parent)
{
	// Mirror the QML __internal.signalMonitor: keep an internal book
	// of pending start/done so we can warn on protocol violations from
	// subclasses.
	connect(this, &CDocumentRepresentationController::startUpdateRepresentation,
			this, &CDocumentRepresentationController::OnStartUpdateRepresentation);
	connect(this, &CDocumentRepresentationController::representationUpdated,
			this, &CDocumentRepresentationController::OnRepresentationUpdated);
	connect(this, &CDocumentRepresentationController::updateRepresentationFailed,
			this, &CDocumentRepresentationController::OnUpdateRepresentationFailed);
	connect(this, &CDocumentRepresentationController::startUpdateDocument,
			this, &CDocumentRepresentationController::OnStartUpdateDocument);
	connect(this, &CDocumentRepresentationController::documentUpdated,
			this, &CDocumentRepresentationController::OnDocumentUpdated);
	connect(this, &CDocumentRepresentationController::updateDocumentFailed,
			this, &CDocumentRepresentationController::OnUpdateDocumentFailed);
}


CDocumentRepresentationController::~CDocumentRepresentationController() = default;


const QString& CDocumentRepresentationController::GetDocumentId() const
{
	return m_documentId;
}


void CDocumentRepresentationController::SetDocumentId(const QString& documentId)
{
	if (m_documentId != documentId){
		m_documentId = documentId;
		Q_EMIT documentIdChanged(m_documentId);
	}
}


const QVariant& CDocumentRepresentationController::GetRepresentationModel() const
{
	return m_representationModel;
}


void CDocumentRepresentationController::SetRepresentationModel(const QVariant& representationModel)
{
	if (m_representationModel != representationModel){
		m_representationModel = representationModel;
		Q_EMIT representationModelChanged(m_representationModel);
	}
}


QObject* CDocumentRepresentationController::GetView() const
{
	return m_view.data();
}


void CDocumentRepresentationController::SetView(QObject* view)
{
	if (m_view.data() != view){
		m_view = view;
		Q_EMIT viewChanged(view);
	}
}


// public slots

void CDocumentRepresentationController::updateRepresentationFromDocument()
{
	Q_EMIT startUpdateRepresentation(m_documentId, m_representationModel);
	qWarning() << "CDocumentRepresentationController::updateRepresentationFromDocument()"
			   << "should be implemented in a subclass (documentId=" << m_documentId << ")";
	Q_EMIT representationUpdated(m_documentId, m_representationModel);
}


void CDocumentRepresentationController::updateDocumentFromRepresentation()
{
	Q_EMIT startUpdateDocument(m_documentId);
	qWarning() << "CDocumentRepresentationController::updateDocumentFromRepresentation()"
			   << "should be implemented in a subclass (documentId=" << m_documentId << ")";
	Q_EMIT documentUpdated(m_documentId);
}


// private slots

void CDocumentRepresentationController::OnStartUpdateRepresentation(const QString& /*documentId*/, const QVariant& /*representation*/)
{
	m_isRepresentationUpdatePending = true;
}


void CDocumentRepresentationController::OnRepresentationUpdated(const QString& /*documentId*/, const QVariant& /*representation*/)
{
	if (!m_isRepresentationUpdatePending){
		qCritical() << "representationUpdated() called for document"
					<< m_documentId
					<< "before startUpdateRepresentation(). Possible logic error.";
	}
	else{
		m_isRepresentationUpdatePending = false;
	}
}


void CDocumentRepresentationController::OnUpdateRepresentationFailed(const QString& /*documentId*/, const QString& /*message*/)
{
	m_isRepresentationUpdatePending = false;
}


void CDocumentRepresentationController::OnStartUpdateDocument(const QString& /*documentId*/)
{
	m_isDocumentUpdatePending = true;
}


void CDocumentRepresentationController::OnDocumentUpdated(const QString& /*documentId*/)
{
	if (!m_isDocumentUpdatePending){
		qCritical() << "documentUpdated() called for document"
					<< m_documentId
					<< "before startUpdateDocument(). Possible logic error.";
	}
	else{
		m_isDocumentUpdatePending = false;
	}
}


void CDocumentRepresentationController::OnUpdateDocumentFailed(const QString& /*documentId*/, const QString& /*message*/)
{
	m_isDocumentUpdatePending = false;
}


} // namespace imtqml
