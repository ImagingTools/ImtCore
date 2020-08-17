#include <imtloggui/CEventItemMetaInfoViewComp.h>


// Qt includes
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

// ACF includes
#include <iwidgets/iwidgets.h>


namespace imtloggui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CEventItemMetaInfoViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	iwidgets::ClearLayout(GetWidget()->layout());

	QVBoxLayout* layoutPtr = dynamic_cast<QVBoxLayout*>(GetWidget()->layout());
	if (layoutPtr != nullptr){
		idoc::IDocumentMetaInfo* metaInfoPtr = GetObservedObject();
		Q_ASSERT(metaInfoPtr != nullptr);

		QList<int> types = metaInfoPtr->GetMetaInfoTypes().toList();
		qSort(types);

		for (int type : types){
			QString name = metaInfoPtr->GetMetaInfoName(type);
			if (name.isEmpty()){
				continue;
			}

			QString value = metaInfoPtr->GetMetaInfo(type).toString();

			QLabel* labelNamePtr = new QLabel(name, GetWidget());
			labelNamePtr->setStyleSheet("font-size: 12px; font: bold; color: #88b8e3");

			QLabel* labelValuePtr = new QLabel(value, GetWidget());
			labelValuePtr->setWordWrap(true);

			layoutPtr->addWidget(labelNamePtr);
			if (labelValuePtr != nullptr){
				labelValuePtr->setStyleSheet("font-size: 9px; color: gray");
				layoutPtr->addWidget(labelValuePtr);
			}

			QSpacerItem* delimeter = new QSpacerItem(10, 5);
			layoutPtr->addItem(delimeter);
		}

		layoutPtr->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CEventItemMetaInfoViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (GetWidget()->layout() == nullptr){
		GetWidget()->setLayout(new QVBoxLayout());
	}
}


void CEventItemMetaInfoViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	if (IsModelAttached()){
		UpdateGui(istd::IChangeable::GetAnyChange());
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CEventItemMetaInfoViewComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


} // namespace imtloggui


