#include <imtgui/CPaginationGuiComp.h>


// Qt includes
#include <QtWidgets/QLayout>

// ACF includes
#include <iprm/IOptionsList.h>
#include <iwidgets/iwidgets.h>


namespace imtgui
{


PaginationGuiItem::PaginationGuiItem(QWidget* parent)
	:QLabel(parent)
{
}


PaginationGuiItem::PaginationGuiItem(const QString& text, QWidget* parent)
	:QLabel(text, parent)
{
}


// reimplemented (QWidget)

void PaginationGuiItem::mousePressEvent(QMouseEvent* ev)
{
	Q_EMIT Clicked();
}


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CPaginationGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	QWidget* widgetPtr = GetQtWidget();
	if (widgetPtr != nullptr){
		QHBoxLayout* layoutPtr = new QHBoxLayout();

		layoutPtr->setContentsMargins(QMargins(0, 0, 0, 0));
		layoutPtr->setSpacing(6);

		widgetPtr->setLayout(layoutPtr);
	}
}


// reimplemented (icomp::CComponentBase)

void CPaginationGuiComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
}


void CPaginationGuiComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CPaginationGuiComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	RefreshWidget();
}


// reimplemented (imod::IObserver)

bool CPaginationGuiComp::OnModelDetached(imod::IModel* modelPtr)
{
	bool retVal = BaseClass2::OnModelDetached(modelPtr);

	RefreshWidget();

	return retVal;
}


// protected methods

void CPaginationGuiComp::RefreshWidget()
{
	if (IsGuiCreated()){
		QWidget* widgetPtr = GetQtWidget();
		if (widgetPtr != nullptr){
			QVBoxLayout* layoutPtr = dynamic_cast<QVBoxLayout*>(widgetPtr->layout());
			if (layoutPtr != nullptr){
				iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
				if (pageSelectionPtr != nullptr){
					const iprm::IOptionsList* pageListPtr = pageSelectionPtr->GetSelectionConstraints();
					if (pageListPtr != nullptr){
						// TODO: Refresh code

						return;
					}
				}

				iwidgets::ClearLayout(layoutPtr);
			}
		}
	}
}


// private slots

void CPaginationGuiComp::OnFirstClicked()
{
	iprm::ISelectionParam* selectionPtr = GetObservedObject();
	if (selectionPtr != nullptr){
		selectionPtr->SetSelectedOptionIndex(0);
	}
}


void CPaginationGuiComp::OnPrevClicked()
{
	iprm::ISelectionParam* selectionPtr = GetObservedObject();
	if (selectionPtr != nullptr){
		int currentSelection = selectionPtr->GetSelectedOptionIndex();
		if (currentSelection > 0){
			selectionPtr->SetSelectedOptionIndex(currentSelection - 1);
		}
	}
}


void CPaginationGuiComp::OnPageClicked()
{
	iprm::ISelectionParam* selectionPtr = GetObservedObject();
	if (selectionPtr != nullptr){
		PaginationGuiItem* itemPtr = dynamic_cast<PaginationGuiItem*>(sender());
		if (itemPtr != nullptr){
			QString text = itemPtr->text();
			bool isOk;
			int page = text.toInt(&isOk);
			if (isOk){
				selectionPtr->SetSelectedOptionIndex(page);
			}
		}
	}
}


void CPaginationGuiComp::OnNextClicked()
{
	iprm::ISelectionParam* selectionPtr = GetObservedObject();
	if (selectionPtr != nullptr){
		int currentSelection = selectionPtr->GetSelectedOptionIndex();
		selectionPtr->SetSelectedOptionIndex(currentSelection + 1);
	}
}


void CPaginationGuiComp::OnLastClicked()
{
	iprm::ISelectionParam* selectionPtr = GetObservedObject();
	if (selectionPtr != nullptr){
		const iprm::IOptionsList* optionsPtr = selectionPtr->GetSelectionConstraints();
		if (optionsPtr != nullptr){
			selectionPtr->SetSelectedOptionIndex(optionsPtr->GetOptionsCount() - 1);
		}
	}
}


} // namespace imtgui


