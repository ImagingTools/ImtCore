#include <imtauthgui/CContactInfoEditorQmlComp.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

// ACF includes
#include <istd/CChangeGroup.h>
#include <iser/CJsonStringWriteArchive.h>

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

//	QFile file("TextFieldCustom.qml");
//	if(file.open(QIODevice::ReadOnly)){
//		QByteArray ba = file.readAll();
//		qDebug() << ba;
//	}

	bool retVal = BaseClass::CreateItem(enginePtr);
	if (retVal){
		QQuickItem *item = BaseClass::GetItem();
		if (item && m_dataControllerCompPtr.IsValid()){
			CTreeItemModel *treeItemModel = m_dataControllerCompPtr->GetTreeItemModel("");
			item->setProperty("contactInfoModel", QVariant::fromValue(treeItemModel));
		}
	}

	return retVal;
//	return BaseClass::CreateItem(enginePtr,{{"contactInfoModel", QVariant::fromValue(&m_treeItemModel)}});
}


} // namespace imtauthgui


