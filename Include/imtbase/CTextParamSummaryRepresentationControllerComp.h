// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/ISummaryRepresentationController.h>


namespace imtbase
{


class CTextParamSummaryRepresentationControllerComp:
			public icomp::CComponentBase,
			virtual public ISummaryRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTextParamSummaryRepresentationControllerComp);
		I_REGISTER_INTERFACE(ISummaryRepresentationController);
	I_END_COMPONENT;

	// reimplemented (ISummaryRepresentationController)
	virtual bool CreateSummaryRepresentation(const istd::IChangeable& param, QString& textRepresentation) const override;
};


} // namespace imtbase


