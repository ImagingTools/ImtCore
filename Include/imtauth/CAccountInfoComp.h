#pragma once


// ACF includes
#include <iauth/IRightsProvider.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtauth/CAccountInfo.h>


namespace imtauth
{


/**
	Component implementation of IAccountInfo interface.
	\ingroup LicenseManagement
*/
class CAccountInfoComp:
			public icomp::CComponentBase,
			public CAccountInfo
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CAccountInfo BaseClass2;

	I_BEGIN_COMPONENT(CAccountInfoComp)
		I_REGISTER_INTERFACE(IAccountInfo);
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_contactCollectionCompPtr, "ContactCollection", "Collection of the contacts", true, "ContactCollection");
		I_ASSIGN_TO(m_contactCollectionModelCompPtr, m_contactCollectionCompPtr, true);
	I_END_COMPONENT

	CAccountInfoComp();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void OnContactCollectionUpdate(
				const istd::IChangeable::ChangeSet& changeSet,
				const imtbase::IObjectCollection* objectCollectionPtr);

private:
	I_REF(imtbase::IObjectCollection, m_contactCollectionCompPtr);
	I_REF(imod::IModel, m_contactCollectionModelCompPtr);

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CAccountInfoComp> m_contactCollectionUpdateBinder;
};


} // namespace imtauth


