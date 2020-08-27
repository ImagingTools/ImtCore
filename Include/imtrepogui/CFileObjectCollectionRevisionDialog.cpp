#include <imtrepogui/CFileObjectCollectionRevisionDialog.h>


namespace imtrepogui
{


CFileObjectCollectionRevisionDialog::CFileObjectCollectionRevisionDialog(QWidget *parent)
	:QDialog(parent, Qt::WindowTitleHint)
{
	setupUi(this);
}


void CFileObjectCollectionRevisionDialog::SetRevisionList(const imtrepo::IRevisionController::RevisionInfoList& revisions)
{
	for (int i = 0; i < revisions.count(); i++){
		Table->insertRow(i);

		Table->setItem(i, 0, new QTableWidgetItem(QString::number(revisions[i].revision)));
		Table->setItem(i, 1, new QTableWidgetItem(revisions[i].timestamp.toString()));
		Table->setItem(i, 2, new QTableWidgetItem(revisions[i].user));
		Table->setItem(i, 3, new QTableWidgetItem(revisions[i].comment));
	}
}


int CFileObjectCollectionRevisionDialog::GetSelectedRevision()
{
	QList<QTableWidgetItem*> selectedItems = Table->selectedItems();
	if (!selectedItems.isEmpty()){
		return Table->item(selectedItems[0]->row(), 0)->text().toInt();
	}

	return -1;
}


} // namespace imtrepogui


