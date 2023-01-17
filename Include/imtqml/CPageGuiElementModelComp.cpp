#include <imtqml/CPageGuiElementModelComp.h>


namespace imtqml
{


// public methods

// reimplemented (imtqml::IPageElementContainer)

QString CPageGuiElementModelComp::GetStartSourceItem() const
{
	return *m_startSourceItemAttrPtr;
}


QString CPageGuiElementModelComp::GetPageQmlItemFilePath() const
{
	return *m_pageSourceItemAttrPtr;
}


} // namespace imtqml


