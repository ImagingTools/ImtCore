#pragma once


// ACF includes
#include <i2d/CPolyline.h>
#include <icomp/CComponentBase.h>
#include <imod/TModelWrap.h>
#include <istd/TSmartPtr.h>
#include <iview/IShape.h>
#include <iview/IShapeFactory.h>

// ImtCore includes
#include <imtgui/IViewExtender.h>


namespace imtgui
{


class CImageContoursExtender: public icomp::CComponentBase, virtual public IViewExtender
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CImageContoursExtender);
		I_REGISTER_INTERFACE(IViewExtender);
		I_ASSIGN(m_shapeFactoryCompPtr, "ShapeFactory", "Shape factory", true, "SimpleShapeFactory");
	I_END_COMPONENT;

	// reimplemented (imtgui::IViewExtender)
	virtual bool AddItems(imod::IObserver* observerPtr, const istd::IChangeable* objectPtr) override;
	virtual bool RemoveItems(imod::IObserver* observerPtr) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentDestroyed() override;

private:
	I_REF(iview::IShapeFactory, m_shapeFactoryCompPtr);

	typedef imod::TModelWrap<i2d::CPolyline> ShapeModel;
	typedef istd::TSmartPtr<ShapeModel> ShapeModelPtr;
	QMap<imod::IObserver*, QList<ShapeModelPtr>> m_shapeModels;
	QMap<imod::IObserver*, QList<iview::IShape*>> m_shapes;
};


} // namespace imtgui


