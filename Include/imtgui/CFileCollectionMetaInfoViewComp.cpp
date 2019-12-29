#include <imtgui/CFileCollectionMetaInfoViewComp.h>


namespace imtgui
{


// protected methods

CFileCollectionMetaInfoViewComp::CFileCollectionMetaInfoViewComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFileCollectionMetaInfoViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	QGridLayout* layout = dynamic_cast<QGridLayout *>(InfoWidget->layout());
	if (layout){
		while (layout->count()){
			QLayoutItem* item = layout->takeAt(0);
			delete item->widget();
			delete item;
		}
	}

	if (!IsModelAttached()){
		return;
	}

	QPixmap pixmap = QPixmap(":/Icons/StateOk");
	layout = dynamic_cast<QGridLayout*>(InfoWidget->layout());
	if (layout){
		idoc::IDocumentMetaInfo* metaInfo = GetObservedObject();
		idoc::IDocumentMetaInfo::MetaInfoTypes types = metaInfo->GetMetaInfoTypes();

		for (int type : types) {
			QString name = metaInfo->GetMetaInfoName(type);
			QVariant value = metaInfo->GetMetaInfo(type);

			QLabel* labelNamePtr = new QLabel(name);
			QLabel* labelValuePtr;

			switch (type) {
			case imtbase::IFileObjectCollection::MIT_CHECKSUM:
				labelNamePtr->setText("CRC");
				labelValuePtr = new QLabel((QString("%1").arg(value.toLongLong(), 8, 16, QChar('0'))).toUpper());
				break;
			case imtbase::IFileObjectCollection::MIT_INSERTION_USER:
			case imtbase::IFileObjectCollection::MIT_LAST_OPERATION_USER:
			case idoc::IDocumentMetaInfo::MIT_TITLE:
			case idoc::IDocumentMetaInfo::MIT_AUTHOR:
			case idoc::IDocumentMetaInfo::MIT_CREATOR:
			case idoc::IDocumentMetaInfo::MIT_DESCRIPTION:
				labelValuePtr = new QLabel(value.toString());
				break;
			case imtbase::IFileObjectCollection::MetaInfoType::MIT_INSERTION_TIME:
			case imtbase::IFileObjectCollection::MetaInfoType::MIT_LAST_OPERATION_TIME:
			case idoc::IDocumentMetaInfo::MIT_CREATION_TIME:
			case idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME:
				labelValuePtr = new QLabel(value.toDateTime().toString(Qt::DefaultLocaleShortDate));
				break;
			case imtbase::IFileObjectCollection::MetaInfoType::MIT_PREVIEW_THUMBNAIL:
				pixmap = value.value<QPixmap>();
				break;
			default:
				labelValuePtr = new QLabel("UNKNOWN MIT_ID");
			}

			labelNamePtr->setStyleSheet("font-size: 12px; font: bold; color: black");
			labelValuePtr->setStyleSheet("font-size: 9px; font: bold; color: gray");
			layout->addWidget(labelNamePtr, layout->rowCount(), 0, 1, 3);
			layout->addWidget(labelValuePtr, layout->rowCount(), 1, 1, 2);
			QSpacerItem* delimeter = new QSpacerItem(10, 5);
			layout->addItem(delimeter, layout->rowCount(), 0, 1, 3);
		}

		PreviewPixmap->setPixmap(pixmap.scaledToWidth(200));

		layout->setColumnMinimumWidth(0, 10);
		layout->setColumnStretch(0, 1);
		layout->setColumnStretch(1, 100);
		layout->setColumnStretch(1, 100);
		layout->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding),layout->rowCount(), 0);
	}
}

// reimplemented (iqtgui::CGuiComponentBase)

void CFileCollectionMetaInfoViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (!InfoWidget->layout()){
		InfoWidget->setLayout(new QGridLayout());
	}

	PreviewPixmap->setPixmap(QPixmap(":/Icons/StateOk").scaledToWidth(200));
}


void CFileCollectionMetaInfoViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateGui(istd::IChangeable::GetAnyChange());
}

// reimplemented (iqtgui::TGuiObserverWrap)

void CFileCollectionMetaInfoViewComp::OnGuiModelDetached()
{
	QGridLayout* layout = dynamic_cast<QGridLayout *>(InfoWidget->layout());
	if (layout){
		while (layout->count()){
			QLayoutItem* item = layout->takeAt(0);
			delete item->widget();
			delete item;
		}
	}

	PreviewPixmap->setPixmap(QPixmap(":/Icons/StateOk").scaledToWidth(200));
}

} // namespace imtgui


