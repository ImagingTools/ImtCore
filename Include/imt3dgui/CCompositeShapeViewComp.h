#pragma once


// ImtCore includes
#include <imt3dgui/IShape3d.h>
#include <imt3dgui/CView3dProviderComp.h>


namespace imt3dgui
{


class CCompositeShapeViewComp: public CView3dProviderComp
{
public:
	typedef CView3dProviderComp BaseClass;

	I_BEGIN_COMPONENT(CCompositeShapeViewComp);
		I_ASSIGN_MULTI_0(m_shapesCompPtr, "Shapes", "List of shapes to be painted in the 3D-view", true);
	I_END_COMPONENT

protected:
	// reimplemented (iqtui::CGuiComponentBase)
	void OnGuiCreated() override;
	void OnGuiDestroyed() override;

private:
	I_MULTIREF(IShape3d, m_shapesCompPtr);
};


} // namespace imt3dgui


