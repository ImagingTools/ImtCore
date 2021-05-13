#include <imtrepogui/CFileObjectCollectionRevisionDialog.h>


// Qt includes
#include <QtWidgets/QMenu>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>


namespace imtrepogui
{


CFileObjectCollectionRevisionDialog::CFileObjectCollectionRevisionDialog(QWidget *parent)
	:QDialog(parent, Qt::WindowTitleHint)
{
	setupUi(this);
	Table->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(Table, &QTableWidget::customContextMenuRequested, this, &CFileObjectCollectionRevisionDialog::OnCustomContextMenuRequested);
}


void CFileObjectCollectionRevisionDialog::SetParams(
			const imtbase::IRevisionController::RevisionInfoList& revisions,
			int currentRevision,
			const imtbase::IObjectCollection* collectionPtr, 
			const imtbase::IRevisionController* revisionControllerPtr,
			const QByteArray& objectId,
			const QString& fileName,
			const QString& filter)
{
	ButtonBox->button(QDialogButtonBox::Ok)->setText(tr("Restore"));

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
		}

		if (!revisions[i].isRevisionAvailable){
			m_unavailableRevisions.insert(revisions[i].revision);
			Table->item(i, 0)->setForeground(Qt::red);
			Table->item(i, 1)->setForeground(Qt::red);
			Table->item(i, 2)->setForeground(Qt::red);
			Table->item(i, 3)->setForeground(Qt::red);
			Table->item(i, 0)->setToolTip(tr("Revision package not found"));
			Table->item(i, 1)->setToolTip(tr("Revision package not found"));
			Table->item(i, 2)->setToolTip(tr("Revision package not found"));
			Table->item(i, 3)->setToolTip(tr("Revision package not found"));
		}

		Table->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	}

	m_collectionPtr = collectionPtr;
	m_revisionControllerPtr = revisionControllerPtr;
	m_objectId = objectId;
	m_fileName = fileName;
	m_filter = filter;
}


int CFileObjectCollectionRevisionDialog::GetSelectedRevision()
{
	QList<QTableWidgetItem*> selectedItems = Table->selectedItems();
	if (!selectedItems.isEmpty()){
		return Table->item(selectedItems[0]->row(), 0)->text().toInt();
	}

	return -1;
}


// private slots

void CFileObjectCollectionRevisionDialog::OnCustomContextMenuRequested(const QPoint &point)
{
	QList<QTableWidgetItem*> selectedItems = Table->selectedItems();
	if (selectedItems.isEmpty()){
		return;
	}

	if (m_unavailableRevisions.contains(Table->item(selectedItems[0]->row(), 0)->text().toInt())){
		return;
	}

	QAction* actionRestore;
	QMenu menu(Table);

	actionRestore = menu.addAction(QIcon(":/Icons/Export"), tr("Export"));
	connect(actionRestore, &QAction::triggered, this, &CFileObjectCollectionRevisionDialog::OnExport);

	menu.exec(Table->viewport()->mapToGlobal(point));
}


void CFileObjectCollectionRevisionDialog::OnExport()
{
	if (m_revisionControllerPtr != nullptr){
		int revision = -1;

		QList<QTableWidgetItem*> selectedItems = Table->selectedItems();
		if (!selectedItems.isEmpty()){
			revision = Table->item(selectedItems[0]->row(), 0)->text().toInt();
		}

		if (revision != -1){
			QString filePath = QFileDialog::getSaveFileName(
						this,
						tr("Export File"),
						QString("%1 (revision %2)").arg(m_fileName).arg(revision),
						m_filter);

			if (!filePath.isEmpty()){
				if (!m_revisionControllerPtr->ExportObject(*m_collectionPtr, m_objectId, revision, filePath)){
					QMessageBox::critical(this, tr("Collection"), tr("Document could not be exported"));
				}
			}
		}
	}
}


} // namespace imtrepogui


