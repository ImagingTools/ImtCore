#include <imtqml/CPageGuiElementModelComp.h>


namespace imtqml
{


// public methods

// reimplemented (imtqml::IPageElementContainer)

QString CPageGuiElementModelComp::GetStartSourceItem() const
{
    if (!m_startSourceItemAttrPtr.IsValid()){
        return QString();
    }

	return *m_startSourceItemAttrPtr;
}


QString CPageGuiElementModelComp::GetPageQmlItemFilePath() const
{
    if (!m_pageSourceItemAttrPtr.IsValid()){
        return QString();
    }

	return *m_pageSourceItemAttrPtr;
}


} // namespace imtqml


