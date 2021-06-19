#pragma once


// Qt includes
#include <QtWidgets/QDialog>

// ImtCore includes
#include <imtbase/IRevisionController.h>
#include <GeneratedFiles/imtgui/ui_CObjectCollectionRevisionDialog.h>


namespace imtgui
{


class CObjectCollectionRevisionDialog:
			public QDialog,
			public Ui::CObjectCollectionRevisionDialog
{
	Q_OBJECT
public:
	explicit CObjectCollectionRevisionDialog(QWidget *parent = nullptr);

	void SetParams(
				const imtbase::IRevisionController::RevisionInfoList& revisions,
				int currentRevision,
				const imtbase::IObjectCollection* collectionPtr,
				const imtbase::IRevisionController* revisionControllerPtr,
				const QByteArray& objectId,
				const QString& fileName,
				const QString& filter);
	int GetSelectedRevision();

private Q_SLOTS:
	void OnCustomContextMenuRequested(const QPoint &point);
	void OnExport();

private:
	const imtbase::IObjectCollection* m_collectionPtr;
	const imtbase::IRevisionController* m_revisionControllerPtr;
	QByteArray m_objectId;
	QString m_fileName;
	QString m_filter;
	QSet<int> m_unavailableRevisions;
};


} // namespace imtgui


