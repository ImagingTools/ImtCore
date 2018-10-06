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
	Supplier segmenting forehead on the scene. Input & output - 3D mesh objects

	?? add IInformationProvider interface & compute all the necessary infos here?

	Algorithm:
	- something like PCA or blob detection on the field of surface normals
	(result is biggest almost-flat area of the object)
	- depending on the geometry of the result (not known yet..) - something like moment of inertia could give the POI
*/
class CForeheadExtractorSupplierComp :
			public iinsp::TSupplierCompWrap< imt3d::CMesh3d>,
			virtual public imt3d::IObject3dProvider
{
public:
	typedef iinsp::TSupplierCompWrap<imt3d::CMesh3d> BaseClass;

	I_BEGIN_COMPONENT(CForeheadExtractorSupplierComp);
		I_REGISTER_INTERFACE(imt3d::IObject3dProvider);
		I_ASSIGN(m_headMeshProviderCompPtr, "HeadMeshProvider", "Provide 3D mesh of head object", true, "HeadMeshProvider");
	I_END_COMPONENT;

	CForeheadExtractorSupplierComp();

	// reimplemented (imt3d::IObject3dProvider)
	virtual const imt3d::IObject3d* GetObject3d() const;

protected:
	// reimplemented (iinsp::TSupplierCompWrap)
	virtual int ProduceObject(ProductType& result) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated();

private:
	I_REF(imt3d::IObject3dProvider, m_headMeshProviderCompPtr);
};


} // namespace imt3dalgo


