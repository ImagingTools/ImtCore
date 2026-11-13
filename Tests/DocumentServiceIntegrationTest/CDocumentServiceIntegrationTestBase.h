// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QObject>

// ACF includes
#include <imod/IObserver.h>

// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtdoc/IDocumentServiceEventHandler.h>


/**
	Event handler that records all events of the document services for verification.
*/
class CRecordingEventHandler: public imtdoc::IDocumentServiceEventHandler
{
public:
	struct RecordedEvent
	{
		QByteArray type;
		QByteArray userId;
		QByteArray documentId;
		QByteArray documentTypeId;
		QString documentName;
		QByteArray documentSourceRef;
		bool isDirty = false;
	};

	QList<RecordedEvent> GetEvents() const;
	void ClearEvents();
	int CountEventsOfType(const QByteArray& type) const;
	bool HasEvent(const QByteArray& type, const QByteArray& userId) const;

	// reimplemented (imtdoc::IDocumentServiceEventHandler)
	virtual bool ProcessEvent(imtdoc::CEventBase* eventPtr) override;

private:
	mutable QMutex m_mutex;
	QList<RecordedEvent> m_events;
};


/**
	Observer for testing RegisterDocumentObserver/UnregisterDocumentObserver.
*/
class CTestObserver: public imod::IObserver
{
public:
	virtual ~CTestObserver();

	// reimplemented (imod::IObserver)
	virtual bool IsModelAttached(const imod::IModel* modelPtr) const override;
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;
	virtual void BeforeUpdate(imod::IModel* modelPtr) override;
	virtual void AfterUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	QList<imod::IModel*> m_models;
};


/**
	Common helpers of the document service integration tests.
*/
class CDocumentServiceIntegrationTestBase: public QObject
{
protected:
	/**
		Creates a new document and waits until its data is loaded.
		Returns the document ID.
	*/
	QByteArray CreateDocumentAndWaitForLoad(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& typeId,
				const QString& name = QString());

	/**
		Waits until the document is not loading anymore.
	*/
	bool WaitForDocumentLoaded(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId);

	/**
		Starts the task and returns its final status (immediate error or task result).
	*/
	imtdoc::IDocumentService::OperationStatus ExecuteTask(
				imtdoc::IDocumentService& service,
				imtdoc::IDocumentService::TaskType taskType,
				const imtdoc::IDocumentService::TaskParams& params,
				QByteArray* documentIdPtr = nullptr);

	imtdoc::IDocumentService::OperationStatus CloseDocument(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId);

	imtdoc::IDocumentService::OperationStatus SaveDocument(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId,
				const QString& documentName = QString());

	imtdoc::IDocumentService::DocumentListItem FindDocumentInfo(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId) const;

	QString GetDocumentText(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId) const;

	/**
		Sets the text of the document using \c SetDocumentData.
	*/
	imtdoc::IDocumentService::OperationStatus SetDocumentText(
				imtdoc::IDocumentService& service,
				const QByteArray& userId,
				const QByteArray& documentId,
				const QString& text);

protected:
	CRecordingEventHandler m_eventHandler;
};


