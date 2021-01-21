#include <imtloggui/CGroupViewComp.h>

// Qt includes
#include <QtGui/QColor>


namespace imtloggui
{


// public methods

// reimplemented (imtloggui::IGroupVisualSettingsProvider)

QColor CGroupViewComp::GetBackgroundColor() const
{
	if (m_colorAttrPtr.IsValid()){
		return QColor(*m_colorAttrPtr);
	}

	return QColor(Qt::transparent);
}


int CGroupViewComp::GetHeight() const
{
	if (m_colorAttrPtr.IsValid()){
		return *m_heightAttrPtr;
	}

	return 200;
}


} // namespace imtloggui


