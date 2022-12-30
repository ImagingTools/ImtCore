#include <imtgui/CVisualStatusComp.h>


// Qt includes
#include <QtCore/QFile>


namespace imtgui
{


// public methods

// reimplemented (iqtgui::IVisualStatus)

QIcon CVisualStatusComp::GetStatusIcon() const
{
	QIcon icon(*m_statusIconAttrPtr);

	if (icon.isNull()){
		return QIcon();
	}

	return icon;
}


QString CVisualStatusComp::GetStatusText() const
{
	return *m_statusTextAttrPtr;
}


} // namespace imtgui


