// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/CSimpleAttributeManagerViewComp.h>


// Qt includes
#include <QtWidgets/QLabel>

// ACF includes
#include <iattr/TAttribute.h>
#include <iwidgets/iwidgets.h>


namespace imtgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CSimpleAttributeManagerViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	iwidgets::ClearLayout(Infos->layout());

	iattr::IAttributesProvider* objectPtr = GetObservedObject();
	iattr::IAttributesProvider::AttributeIds ids = objectPtr->GetAttributeIds();
	for (const QByteArray& id : ids){
		iser::IObject* attrObjectPtr = objectPtr->GetAttribute(id);

		iattr::CIdAttribute* idAttrPtr = dynamic_cast<iattr::CIdAttribute*>(attrObjectPtr);
		iattr::CStringAttribute* strAttrPtr = dynamic_cast<iattr::CStringAttribute*>(attrObjectPtr);
		iattr::CIntegerAttribute* intAttrPtr = dynamic_cast<iattr::CIntegerAttribute*>(attrObjectPtr);
		iattr::CRealAttribute* realAttrPtr = dynamic_cast<iattr::CRealAttribute*>(attrObjectPtr);
		iattr::CBooleanAttribute* boolAttrPtr = dynamic_cast<iattr::CBooleanAttribute*>(attrObjectPtr);

		QString info = id + ":";

		if (idAttrPtr != nullptr){
			info += " " + idAttrPtr->GetValue();
		}
		else if (strAttrPtr != nullptr){
			info += " " + strAttrPtr->GetValue();
		}
		else if (intAttrPtr != nullptr){
			info += " " + QString::number(intAttrPtr->GetValue());
		}
		else if (realAttrPtr != nullptr){
			info += " " + QString::number(realAttrPtr->GetValue());
		}
		else if (boolAttrPtr != nullptr){
			info += " " + QVariant(boolAttrPtr->GetValue()).toString();
		}
		else {
			info += " INVALID VALUE";
		}

		QLabel* labelPtr = new QLabel(info, Infos);
		Infos->layout()->addWidget(labelPtr);
	}
}


} // namespace imtgui
