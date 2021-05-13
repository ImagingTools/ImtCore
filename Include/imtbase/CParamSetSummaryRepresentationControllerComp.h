#pragma once


// Qt includes
#include <QtCore/QVector>
#include <QtCore/QUuid>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/ISummaryRepresentationController.h>


namespace imtbase
{


class CParamSetSummaryRepresentationControllerComp:
			public icomp::CComponentBase,
			virtual public ISummaryRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CParamSetSummaryRepresentationControllerComp);
		I_REGISTER_INTERFACE(ISummaryRepresentationController);
		I_ASSIGN_MULTI_0(m_paramIdAttrPtr, "ParamIds", "Param ids in the ParamSet", false)
		I_ASSIGN_MULTI_0(m_paramNameAttrPtr, "ParamNames", "Displayed names for params", false)
		I_ASSIGN_MULTI_0(m_paramRepresentationControllerCompPtr, "ParamRepresentationControllers", "Param representation controllers", false)
	I_END_COMPONENT;

	// reimplemented (ITextBasedRepresentationController)
	virtual bool CreateSummaryRepresentation(const istd::IChangeable& object, QString& textRepresentation) const override;

private:
	I_MULTIATTR(QByteArray, m_paramIdAttrPtr);
	I_MULTIATTR(QByteArray, m_paramNameAttrPtr);
	I_MULTIREF(ISummaryRepresentationController, m_paramRepresentationControllerCompPtr);
};


} // namespace imtbase


