#include <imtauthgui/CContactInfoEditorQmlComp.h>


namespace imtauthgui
{


// protected methods

// reimplemented (imtqml::CQuickObjectCompBase)

void CContactInfoEditorQmlComp::OnItemCreated(QQuickItem& item)
{
	if (m_dataControllerCompPtr.IsValid()){
		CTreeItemModel *treeItemModel = m_dataControllerCompPtr->GetTreeItemModel("");

		item.setProperty("contactInfoModel", QVariant::fromValue(treeItemModel));
	}
}


} // namespace imtauthgui


