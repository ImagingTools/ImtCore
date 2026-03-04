// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/CButtonBasedSelectionGuiComp.h>


// ACF includes
#include <istd/CChangeGroup.h>
#include <iwidgets/iwidgets.h>


namespace imtgui
{


// public methods

CButtonBasedSelectionGuiComp::CButtonBasedSelectionGuiComp()
	: m_optionListObserver(*this),
	  m_iconObserver(*this)
{
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CButtonBasedSelectionGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	QLayout* layoutPtr = Frame->layout();
	Q_ASSERT(layoutPtr != nullptr);

	const iprm::ISelectionParam* selectionPtr = GetObservedObject();
	Q_ASSERT(selectionPtr != nullptr);

	int index = selectionPtr->GetSelectedOptionIndex();
	QLayoutItem* itemPtr = layoutPtr->itemAt(index);
	if (itemPtr != nullptr){
		QToolButton* buttonPtr = dynamic_cast<QToolButton*>(itemPtr->widget());
		if (buttonPtr != nullptr){
			buttonPtr->setChecked(true);
		}
	}
	else {
		QList<QToolButton*> buttonList = Frame->findChildren<QToolButton*>();
		for (QToolButton* buttonPtr : buttonList){
			buttonPtr->setChecked(false);
		}
	}
}


void CButtonBasedSelectionGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	iprm::ISelectionParam* selectionParamsPtr = GetObservedObject();
	if (selectionParamsPtr != nullptr){
		const iprm::IOptionsList* constraintsPtr = selectionParamsPtr->GetSelectionConstraints();
		if (constraintsPtr != nullptr){
			m_optionListObserver.RegisterObject(constraintsPtr, &CButtonBasedSelectionGuiComp::OnOptionListUpdate);
		}
	}

	iqtgui::IIconProvider* iconProviderPtr = m_iconProviderCompPtr.GetPtr();
	if (iconProviderPtr != nullptr){
		m_iconObserver.RegisterObject(iconProviderPtr, &CButtonBasedSelectionGuiComp::OnIconsUpdate);
	}
}


void CButtonBasedSelectionGuiComp::OnGuiModelDetached()
{
	m_iconObserver.UnregisterAllObjects();
	m_optionListObserver.UnregisterAllObjects();

	iwidgets::ClearLayout(Frame->layout());

	BaseClass::OnGuiModelDetached();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CButtonBasedSelectionGuiComp::OnGuiDesignChanged()
{
	BaseClass::OnGuiDesignChanged();
}


// private slots

void CButtonBasedSelectionGuiComp::OnButtonClicked(bool checked)
{
	QLayout* layoutPtr = Frame->layout();
	Q_ASSERT(layoutPtr != nullptr);

	QToolButton* buttonPtr = dynamic_cast<QToolButton*>(sender());
	Q_ASSERT(buttonPtr != nullptr);

	if (!checked){
		buttonPtr->setChecked(true);
	}

	iprm::ISelectionParam* selectionPtr = GetObservedObject();
	Q_ASSERT(selectionPtr != nullptr);
	if (selectionPtr != nullptr){
		selectionPtr->SetSelectedOptionIndex(buttonPtr->property("Index").toInt());
	}
}


// private methods

void CButtonBasedSelectionGuiComp::OnOptionListUpdate(
	const istd::IChangeable::ChangeSet& /*changeSet*/, const iprm::IOptionsList* objectPtr)
{
	if (IsGuiCreated() && !IsUpdateBlocked() && IsModelAttached()){
		QLayout* layoutPtr = Frame->layout();
		if (layoutPtr != nullptr){
			iwidgets::ClearLayout(layoutPtr);
			delete layoutPtr;
		}

		switch (*m_orientationAttrPtr){
		case O_VERTICAL:
			layoutPtr = new QVBoxLayout(Frame);
			break;
		case O_HORIZONTAL:
		default:
			layoutPtr = new QHBoxLayout(Frame);
			break;
		}

		Frame->setLayout(layoutPtr);
		layoutPtr->setSpacing(*m_spacingAttrPtr > 0 ? *m_spacingAttrPtr : 0);
		layoutPtr->setContentsMargins(QMargins(0, 0, 0, 0));

		iqtgui::IIconProvider* iconProvider =
			m_iconProviderCompPtr.IsValid() ? m_iconProviderCompPtr.GetPtr() : nullptr;

		int count = objectPtr->GetOptionsCount();
		if (count == 1 && *m_hideTheOnlyButtonAttrPtr){
			return;
		}

		for (int i = 0; i < count; i++){
			QToolButton* buttonPtr = new QToolButton(Frame);
			buttonPtr->setText(objectPtr->GetOptionName(i));
			buttonPtr->setEnabled(objectPtr->IsOptionEnabled(i));
			buttonPtr->setIconSize(
				*m_iconSizeAttrPtr > 0 ? QSize(*m_iconSizeAttrPtr, *m_iconSizeAttrPtr) : QSize(16, 16));
			buttonPtr->setCheckable(true);
			buttonPtr->setAutoRaise(true);
			buttonPtr->setAutoExclusive(true);
			buttonPtr->setProperty("Index", i);

			if (i == 0){
				buttonPtr->setProperty("First", true);
			}

			if (i == count - 1){
				buttonPtr->setProperty("Last", true);
			}

			if (*m_buttonStyleAttrPtr >= 0 && *m_buttonStyleAttrPtr <= Qt::ToolButtonFollowStyle){
				buttonPtr->setToolButtonStyle((Qt::ToolButtonStyle)(*m_buttonStyleAttrPtr));
			}

			if (iconProvider != nullptr){
				if (i < iconProvider->GetIconCount()){
					buttonPtr->setIcon(iconProvider->GetIcon(i));
				}
			}

			connect(buttonPtr, &QToolButton::clicked, this, &CButtonBasedSelectionGuiComp::OnButtonClicked);

			layoutPtr->addWidget(buttonPtr);

			GetQtWidget()->style()->polish(buttonPtr);
		}

		UpdateGui(istd::IChangeable::ChangeSet());
	}
}


void CButtonBasedSelectionGuiComp::OnIconsUpdate(
	const istd::IChangeable::ChangeSet& /*changeSet*/, const iqtgui::IIconProvider* objectPtr)
{
	QList<QToolButton*> buttonList = Frame->findChildren<QToolButton*>();
	for (int i = 0; i < buttonList.count(); i++){
		QToolButton* buttonPtr = buttonList[i];
		int index = buttonPtr->property("Index").toInt();
		QIcon icon = index < objectPtr->GetIconCount() ? objectPtr->GetIcon(index) : QIcon();
		buttonPtr->setIcon(icon);
	}
}


} // namespace imtgui
