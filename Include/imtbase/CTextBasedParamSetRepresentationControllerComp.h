#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/ITextBasedRepresentationController.h>
#include <imtbase/IParamRepresentationController.h>


namespace imtbase
{


class CTextBasedParamSetRepresentationControllerComp:
			public icomp::CComponentBase,
			virtual public ITextBasedRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTextBasedParamSetRepresentationControllerComp);
		I_REGISTER_INTERFACE(ITextBasedRepresentationController);
		I_ASSIGN_MULTI_0(m_paramIdAttrPtr, "ParamIds", "Param ids in the ParamSet", false)
		I_ASSIGN_MULTI_0(m_paramNameAttrPtr, "ParamNames", "Displayed names for params", false)
		I_ASSIGN_MULTI_0(m_paramRepresentationControllerCompPtr, "ParamRepresentationControllers", "Param representation controllers", false)
	I_END_COMPONENT;

	// reimplemented (ITextBasedRepresentationController)
	virtual bool CreateTextRepresentation(const istd::IChangeable& object, QString& textRepresentation) const override;

private:
	I_MULTIATTR(QByteArray, m_paramIdAttrPtr);
	I_MULTIATTR(QByteArray, m_paramNameAttrPtr);
	I_MULTIREF(IParamRepresentationController, m_paramRepresentationControllerCompPtr);
};


} // namespace imtbase


