#pragma once

// ACF includes


// ACF-Solutions includes
#include <iinsp/TSupplierCompWrap.h>

// imtcore includes
#include <imt3d\IPointCloud3d.h>
#include <imt3d\IObject3dProvider.h>
#include <imt3d\CMesh.h>


namespace imt3dalgo
{


	/**
		Supplier of tetrahedral mesh object from 3d point cloud
	*/
	class CMeshSupplierComp :
		public iinsp::TSupplierCompWrap< imt3d::CMesh>,
		virtual public imt3d::IObject3dProvider
	{
	public:
		typedef iinsp::TSupplierCompWrap<imt3d::CMesh> BaseClass;

		I_BEGIN_COMPONENT(CMeshSupplierComp);
		I_REGISTER_INTERFACE(imt3d::IObject3dProvider);
		I_ASSIGN(m_pointCloudProviderCompPtr, "PointCloudProvider", "Provide input 3D point cloud", true, "PointCloudProvider");
		I_END_COMPONENT;

		CMeshSupplierComp();

		// reimplemented (imt3d::IObject3dProvider)
		virtual const imt3d::IObject3d* GetObject3d() const;

	protected:
		
		// reimplemented (iinsp::TSupplierCompWrap)
		virtual int ProduceObject(ProductType& result) const;

		// reimplemented (icomp::CComponentBase)
		virtual void OnComponentCreated();

	private:
		I_REF(imt3d::IObject3dProvider, m_pointCloudProviderCompPtr);

	};


} // namespace imt3dalgo


