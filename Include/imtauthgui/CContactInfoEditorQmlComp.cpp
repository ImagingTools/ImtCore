#include <imtauthgui/CContactInfoEditorQmlComp.h>


namespace imtauthgui
{


// protected methods

// reimplemented (imtqml::CQuickObjectCompBase)

void CContactInfoEditorQmlComp::OnItemCreated()
{
	Q_ASSERT(m_quickItemPtr != nullptr);

	if (m_dataProviderCompPtr.IsValid()){
		QList<QByteArray> query;
		imtrest::QueryParams params;
		imtbase::CTreeItemModel *itemModelPtr = m_dataProviderCompPtr->GetTreeItemModel(query, params);

		m_quickItemPtr->setProperty("contactInfoModel", QVariant::fromValue(itemModelPtr));
	}
}


} // namespace imtauthgui


