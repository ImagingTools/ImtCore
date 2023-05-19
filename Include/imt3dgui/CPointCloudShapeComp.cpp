#include <imt3dgui/CPointCloudShapeComp.h>


namespace imt3dgui
{

void CPointCloudShapeComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_isInfoBoxEnabledAttrPtr.IsValid()) {
		SetInfoBoxEnabled(*m_isInfoBoxEnabledAttrPtr);
	}
}


} // namespace imt3dgui


