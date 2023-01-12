#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IRepresentationController.h>


namespace imtbase
{


class CObjectRepresentationControllerCompBase:
			public icomp::CComponentBase,
			virtual public IRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectRepresentationControllerCompBase)
		I_REGISTER_INTERFACE(IRepresentationController);
		I_ASSIGN(m_paramIdAttrPtr, "ObjectId", "Object-ID", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual QByteArray GetModelId() const override;

protected:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
};


} // namespace imtbase


