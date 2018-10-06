#pragma once


// ACF includes
#include <iinsp/TSupplierCompWrap.h>

// ImtCore includes
#include <imt3d/IPointCloud3d.h>
#include <imt3d/IObject3dProvider.h>
#include <imt3d/CMesh3d.h>


namespace imt3dalgo
{


/**
	Supplier segmenting object of interest on the scene. Sets it as output if it's head, otherwise output is NULL.
	Input & output - 3D mesh objects


	Algorithm:
	- Use range & (if available) other data thresholding to extract biggest connected object in front of the camera
	- Use object topology (other features) to decide wheater it is head
*/
class CHeadExtractorSupplierComp :
			public iinsp::TSupplierCompWrap< imt3d::CMesh3d>,
			virtual public imt3d::IObject3dProvider
{
public:
	typedef iinsp::TSupplierCompWrap<imt3d::CMesh3d> BaseClass;

	I_BEGIN_COMPONENT(CHeadExtractorSupplierComp);
	I_REGISTER_INTERFACE(imt3d::IObject3dProvider);
	I_ASSIGN(m_sceneMeshProviderCompPtr, "SceneMeshProvider", "Provide 3D mesh of input scene", true, "SceneMeshProvider");
	I_END_COMPONENT;

	CHeadExtractorSupplierComp();

	// reimplemented (imt3d::IObject3dProvider)
	virtual const imt3d::IObject3d* GetObject3d() const;

protected:
	// reimplemented (iinsp::TSupplierCompWrap)
	virtual int ProduceObject(ProductType& result) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	I_REF(imt3d::IObject3dProvider, m_sceneMeshProviderCompPtr);

};


} // namespace imt3dalgo


