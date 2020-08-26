#include <imtrepogui/CFileObjectCollectionRevisionDialog.h>


namespace imtrepogui
{


CFileObjectCollectionRevisionDialog::CFileObjectCollectionRevisionDialog(QWidget *parent)
	:QDialog(parent, Qt::WindowTitleHint)
{
	setupUi(this);
	setWindowTitle(tr("Select revision"));
	Revisions->setSelectionMode(QAbstractItemView::SingleSelection);
	Revisions->setSelectionBehavior(QAbstractItemView::SelectRows);
}


void CFileObjectCollectionRevisionDialog::SetRevisionList(const QVector<int>& revisions)
{
	for (int i = 0; i < revisions.count(); i++){
		Revisions->insertRow(i);

		QTableWidgetItem* itemPtr = new QTableWidgetItem(QString::number(revisions[i]));
		Revisions->setItem(i, 0, itemPtr);
	}
}


int CFileObjectCollectionRevisionDialog::GetSelectedRevision()
{
	QList<QTableWidgetItem*> selectedItems = Revisions->selectedItems();
	if (!selectedItems.isEmpty()){
		return Revisions->item(selectedItems[0]->row(), 0)->text().toInt();
	}

	return -1;
}


} // namespace imtrepogui


