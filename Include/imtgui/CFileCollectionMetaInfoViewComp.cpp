#include <imtgui/CFileCollectionMetaInfoViewComp.h>


// ACF includes
#include <iwidgets/iwidgets.h>


namespace imtgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CFileCollectionMetaInfoViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	iwidgets::ClearLayout(InfoWidget->layout());

	QPixmap pixmap;

	QGridLayout* layoutPtr = dynamic_cast<QGridLayout*>(InfoWidget->layout());
	if (layoutPtr != nullptr){
		idoc::IDocumentMetaInfo* metaInfoPtr= GetObservedObject();
		Q_ASSERT(metaInfoPtr != nullptr);

		idoc::IDocumentMetaInfo::MetaInfoTypes types = metaInfoPtr->GetMetaInfoTypes();

		for (int type : types) {
			QString name = metaInfoPtr->GetMetaInfoName(type);
			QVariant value = metaInfoPtr->GetMetaInfo(type);

			QLabel* labelNamePtr = new QLabel(name);
			QLabel* labelValuePtr = nullptr;

			switch (type) {
			case imtbase::IFileObjectCollection::MIT_CHECKSUM:
				labelNamePtr->setText("Checksum");
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
				pixmap.convertFromImage(value.value<QImage>());
				PreviewPixmap->setPixmap(pixmap.scaledToWidth(200));
				PreviewFrame->show();
				break;
			default:
				labelValuePtr = new QLabel("UNKNOWN MIT_ID");
			}

			labelNamePtr->setStyleSheet("font-size: 12px; font: bold; color: #335777");
			labelValuePtr->setStyleSheet("font-size: 9px; color: gray");
			layoutPtr->addWidget(labelNamePtr, layoutPtr->rowCount(), 0, 1, 1);
			layoutPtr->addWidget(labelValuePtr, layoutPtr->rowCount(), 0, 1, 1);

			QSpacerItem* delimeter = new QSpacerItem(10, 5);
			layoutPtr->addItem(delimeter, layoutPtr->rowCount(), 0, 1, 1);
		}

		layoutPtr->setColumnMinimumWidth(0, 10);
		layoutPtr->setColumnStretch(0, 1);
		layoutPtr->setColumnStretch(1, 100);
		layoutPtr->setColumnStretch(1, 100);
		layoutPtr->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding),layoutPtr->rowCount(), 0);
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CFileCollectionMetaInfoViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (InfoWidget->layout() == nullptr){
		InfoWidget->setLayout(new QGridLayout());
	}

	PreviewFrame->hide();
}


void CFileCollectionMetaInfoViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	if (IsModelAttached()){
		UpdateGui(istd::IChangeable::GetAnyChange());
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CFileCollectionMetaInfoViewComp::OnGuiModelDetached()
{
	iwidgets::ClearLayout(InfoWidget->layout());

	PreviewFrame->hide();

	BaseClass::OnGuiModelDetached();
}


} // namespace imtgui


