// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/ISummaryRepresentationController.h>


namespace imtbase
{


class CSelectionParamSummaryRepresentationControllerComp:
			public icomp::CComponentBase,
			public imtbase::ISummaryRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSelectionParamSummaryRepresentationControllerComp)
		I_REGISTER_INTERFACE(ISummaryRepresentationController);
	I_END_COMPONENT;

protected:
	// reimplemented (ISummaryRepresentationController)
	virtual bool CreateSummaryRepresentation(const istd::IChangeable& param, QString& textRepresentation) const override;
};


} // namespace imtbase


