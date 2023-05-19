#pragma once


// ImtCore includes
#include <imt3dgui/CPointCloudShape.h>

//ACF includes
#include <ibase/TMakeModelObserverCompWrap.h>


namespace imt3dgui
{


/**
	Common point cloud implementation in 3D-space.
*/
class CPointCloudShapeComp: public ibase::TMakeModelObserverCompWrap<
	imt3dgui::CPointCloudShape,
	imt3dgui::IShape3d,
	imt3dview::IScene3dItem>
{
public:
	typedef ibase::TMakeModelObserverCompWrap<
		imt3dgui::CPointCloudShape,
		imt3dgui::IShape3d,
		imt3dview::IScene3dItem> BaseClass;

	I_BEGIN_COMPONENT(CPointCloudShapeComp);
		I_ASSIGN(m_isInfoBoxEnabledAttrPtr, "ShowInfoBox", "Show info box if enabled", true, false);
	I_END_COMPONENT

protected:

	virtual void OnComponentCreated() override;

private:
	I_ATTR(bool, m_isInfoBoxEnabledAttrPtr);
};


} // namespace imt3dgui
