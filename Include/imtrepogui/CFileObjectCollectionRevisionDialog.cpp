#include <imtrepogui/CFileObjectCollectionRevisionDialog.h>


namespace imtrepogui
{


CFileObjectCollectionRevisionDialog::CFileObjectCollectionRevisionDialog(QWidget *parent)
	:QDialog(parent, Qt::WindowTitleHint)
{
	setupUi(this);
}


void CFileObjectCollectionRevisionDialog::SetRevisionList(const imtbase::IRevisionController::RevisionInfoList& revisions, int currentRevision)
{
	for (int i = 0; i < revisions.count(); i++){
		Table->insertRow(i);

		Table->setItem(i, 0, new QTableWidgetItem(QString::number(revisions[i].revision)));
		Table->setItem(i, 1, new QTableWidgetItem(revisions[i].timestamp.toString("dd.MM.yyyy hh:mm:ss")));
		Table->setItem(i, 2, new QTableWidgetItem(revisions[i].user));
		Table->setItem(i, 3, new QTableWidgetItem(revisions[i].comment));

		if (revisions[i].revision == currentRevision){
			QFont font = Table->item(i, 0)->font();
			font.setBold(true);
			Table->item(i, 0)->setFont(font);
			Table->item(i, 1)->setFont(font);
			Table->item(i, 2)->setFont(font);
			Table->item(i, 3)->setFont(font);
			Table->item(i, 0)->setFlags(Qt::NoItemFlags);
			Table->item(i, 1)->setFlags(Qt::NoItemFlags);
			Table->item(i, 2)->setFlags(Qt::NoItemFlags);
			Table->item(i, 3)->setFlags(Qt::NoItemFlags);
		}

		if (!revisions[i].isArchivePresent){
			Table->item(i, 0)->setTextColor(Qt::red);
			Table->item(i, 1)->setTextColor(Qt::red);
			Table->item(i, 2)->setTextColor(Qt::red);
			Table->item(i, 3)->setTextColor(Qt::red);
			Table->item(i, 0)->setToolTip(tr("Revision package not found"));
			Table->item(i, 1)->setToolTip(tr("Revision package not found"));
			Table->item(i, 2)->setToolTip(tr("Revision package not found"));
			Table->item(i, 3)->setToolTip(tr("Revision package not found"));
		}

		Table->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
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


