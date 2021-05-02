#include <imtauthgui/CContactInfoEditorQmlComp.h>


namespace imtauthgui
{


// protected methods

// reimplemented (imtqml::CQuickObjectCompBase)

void CContactInfoEditorQmlComp::OnItemCreated()
{
	Q_ASSERT(m_quickItemPtr != nullptr);

	if (m_dataControllerCompPtr.IsValid()){
		CTreeItemModel *treeItemModel = m_dataControllerCompPtr->GetTreeItemModel("");

		m_quickItemPtr->setProperty("contactInfoModel", QVariant::fromValue(treeItemModel));
	}
}


} // namespace imtauthgui


