#include <imtgui/CObjectCollectionMetaInfoViewComp.h>


// Qt includes
#include <QtWidgets/QGraphicsEffect>

// ACF includes
#include <iwidgets/iwidgets.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtgui
{


// protected methods

void CObjectCollectionMetaInfoViewComp::FillWidget(QGridLayout* layoutPtr)
{
	idoc::IDocumentMetaInfo* metaInfoPtr = GetObservedObject();
	Q_ASSERT(metaInfoPtr != nullptr);

	idoc::IDocumentMetaInfo::MetaInfoTypes types = metaInfoPtr->GetMetaInfoTypes();
	for (int type : types){
		if (m_ignoreIdsAttrPtr.FindValue(type) >= 0){
			continue;
		}

		QString name = metaInfoPtr->GetMetaInfoName(type);
		if (name.isEmpty()){
//			continue;
		}

		QString textValue;
		QVariant value = metaInfoPtr->GetMetaInfo(type);
		QLocale locale;
		switch (value.type()){
		case QVariant::String:
			textValue = value.toString();
			break;
		case QVariant::Date:
			textValue = locale.toString(value.toDate(),QLocale::ShortFormat);
			break;
		case QVariant::DateTime:
			textValue = locale.toString(value.toDateTime(),QLocale::ShortFormat);
			break;
		case QVariant::ByteArray:
			textValue = value.toByteArray();
			break;
		case QVariant::Int:
			textValue = QString::number(value.toInt());
			break;
		case QVariant::UInt:
			textValue = QString::number(value.toUInt());
			break;
		case QVariant::LongLong:
			textValue = QString::number(value.toLongLong());
			break;
		case QVariant::ULongLong:
			textValue = QString::number(value.toULongLong());
			break;
		case QVariant::Double:
			textValue = QString::number(value.toDouble());
		}

		switch (type){
		case idoc::IDocumentMetaInfo::MIT_CONTENT_CHECKSUM:
			textValue = QString("%1").arg(value.toLongLong(), 8, 16, QChar('0')).toUpper();
			break;

		case imtbase::IObjectCollection::MIT_PREVIEW_THUMBNAIL:
			QPixmap pixmap;
			pixmap.convertFromImage(value.value<QImage>());
			PreviewPixmap->setPixmap(pixmap.scaledToWidth(250));
			if (!pixmap.isNull()){
				PreviewFrame->setVisible(true);
			}
			break;
		}

		if (type != imtbase::IObjectCollection::MIT_PREVIEW_THUMBNAIL){
			QLabel* labelNamePtr = new QLabel(name, GetWidget());
			labelNamePtr->setProperty("MetaInfoType", true);
			layoutPtr->addWidget(labelNamePtr, layoutPtr->rowCount(), 0, 1, 1);

			QLabel* labelValuePtr = new QLabel(textValue, GetWidget());
			labelValuePtr->setProperty("MetaInfoValue", true);
			layoutPtr->addWidget(labelValuePtr, layoutPtr->rowCount(), 0, 1, 1);
		}

		QSpacerItem* delimeter = new QSpacerItem(10, 5);
		layoutPtr->addItem(delimeter, layoutPtr->rowCount(), 0, 1, 1);
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CObjectCollectionMetaInfoViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	iwidgets::ClearLayout(InfoWidget->layout());

	QPixmap pixmap;

	QGridLayout* layoutPtr = dynamic_cast<QGridLayout*>(InfoWidget->layout());
	if (layoutPtr != nullptr){
		FillWidget(layoutPtr);

		layoutPtr->setColumnMinimumWidth(0, 10);
		layoutPtr->setColumnStretch(0, 1);
		layoutPtr->setColumnStretch(1, 100);
		layoutPtr->setColumnStretch(1, 100);

		layoutPtr->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding), layoutPtr->rowCount(), 0);
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CObjectCollectionMetaInfoViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (InfoWidget->layout() == nullptr){
		InfoWidget->setLayout(new QGridLayout());
	}

	PreviewFrame->setVisible(false);

	QGraphicsDropShadowEffect* shadowPtr = new QGraphicsDropShadowEffect;
	shadowPtr->setXOffset(0);
	shadowPtr->setYOffset(0);
	shadowPtr->setBlurRadius(8);
	shadowPtr->setColor(QColor("#555555"));

	PreviewFrame->setGraphicsEffect(shadowPtr);
}


void CObjectCollectionMetaInfoViewComp::OnGuiDestroyed()
{
	iwidgets::ClearLayout(InfoWidget->layout());

	InfoWidget->setLayout(nullptr);
	
	BaseClass::OnGuiDestroyed();
}


void CObjectCollectionMetaInfoViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	if (IsModelAttached()){
		UpdateGui(istd::IChangeable::GetAnyChange());
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CObjectCollectionMetaInfoViewComp::OnGuiModelDetached()
{
	iwidgets::ClearLayout(InfoWidget->layout());

	PreviewFrame->setVisible(false);

	BaseClass::OnGuiModelDetached();
}


} // namespace imtgui


