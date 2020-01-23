#include <imtgui/CFileCollectionMetaInfoViewComp.h>


// Qt includes
#include <QtWidgets/QGraphicsEffect>

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
		for (int type : types){
			QString name = metaInfoPtr->GetMetaInfoName(type);
			if (name.isEmpty()){
				continue;
			}

			QString textValue;
			QVariant value = metaInfoPtr->GetMetaInfo(type);
			switch (value.type()){
			case QVariant::String:
				textValue = value.toString();
				break;
			case QVariant::DateTime:
				textValue = value.toDateTime().toString(Qt::DefaultLocaleShortDate);
				break;
			case QVariant::ByteArray:
				textValue = value.toByteArray();
				break;
			}

			QLabel* labelNamePtr = new QLabel(name, GetWidget());
			labelNamePtr->setStyleSheet("font-size: 12px; font: bold; color: #88b8e3");

			QLabel* labelValuePtr = new QLabel(textValue, GetWidget());
			labelValuePtr->setWordWrap(true);

			switch (type){
			case idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM:
				labelValuePtr->setText((QString("%1").arg(value.toLongLong(), 8, 16, QChar('0'))).toUpper());
				break;

			case imtbase::IFileObjectCollection::MIT_PREVIEW_THUMBNAIL:
				pixmap.convertFromImage(value.value<QImage>());
				PreviewPixmap->setPixmap(pixmap.scaledToWidth(250));
				PreviewFrame->show();
				break;
			}

			if (type != imtbase::IFileObjectCollection::MIT_PREVIEW_THUMBNAIL){
				layoutPtr->addWidget(labelNamePtr, layoutPtr->rowCount(), 0, 1, 1);
				if (labelValuePtr != nullptr){
					labelValuePtr->setStyleSheet("font-size: 9px; color: gray");
					layoutPtr->addWidget(labelValuePtr, layoutPtr->rowCount(), 0, 1, 1);
				}
			}

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

	QGraphicsDropShadowEffect* shadowPtr = new QGraphicsDropShadowEffect;
	shadowPtr->setXOffset(0);
	shadowPtr->setYOffset(0);
	shadowPtr->setBlurRadius(12);
	shadowPtr->setColor(QColor("#555555"));

	PreviewFrame->setGraphicsEffect(shadowPtr);

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


