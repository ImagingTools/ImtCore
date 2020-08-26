#pragma once


// Qt includes
#include <QtWidgets/QDialog>

// ImtCore includes
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

	void SetRevisionList(const QVector<int>& revisions);
	int GetSelectedRevision();
};


} // namespace imtrepogui


