#pragma once


// Qt includes
#include <QtWidgets/QDialog>

// ImtCore includes
#include <imtbase/IRevisionController.h>
#include <GeneratedFiles/imtrepogui/ui_CFileObjectCollectionRevisionDialog.h>


namespace imtrepogui
{


class CFileObjectCollectionRevisionDialog:
			public QDialog,
			public Ui::CFileObjectCollectionRevisionDialog
{
	Q_OBJECT
public:
	explicit CFileObjectCollectionRevisionDialog(QWidget *parent = nullptr);

	void SetParams(
				const imtbase::IRevisionController::RevisionInfoList& revisions,
				int currentRevision,
				const imtbase::IRevisionController* revisionControllerPtr,
				const QByteArray& objectId,
				const QString& fileName,
				const QString& filter);
	int GetSelectedRevision();

private Q_SLOTS:
	void OnCustomContextMenuRequested(const QPoint &point);
	void OnExport();

private:
	const imtbase::IRevisionController* m_revisionControllerPtr;
	QByteArray m_objectId;
	QString m_fileName;
	QString m_filter;
};


} // namespace imtrepogui


