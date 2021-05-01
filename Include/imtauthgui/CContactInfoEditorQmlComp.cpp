#include <imtauthgui/CContactInfoEditorQmlComp.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>


namespace imtauthgui
{


// protected methods

// reimplemented (imtqml::CQuickObjectComp)

bool CContactInfoEditorQmlComp::CreateItem(QQmlEngine *enginePtr)
{
	enginePtr->addImportPath("qrc:/qml");

	bool retVal = BaseClass::CreateItem(enginePtr);
	if (retVal){
		QQuickItem *item = BaseClass::GetItem();
		if (item && m_dataControllerCompPtr.IsValid()){
			CTreeItemModel *treeItemModel = m_dataControllerCompPtr->GetTreeItemModel("");
			item->setProperty("contactInfoModel", QVariant::fromValue(treeItemModel));
		}
	}

	return retVal;
}


} // namespace imtauthgui


