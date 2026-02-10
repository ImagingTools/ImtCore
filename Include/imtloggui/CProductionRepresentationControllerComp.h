// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtloggui/CRepresentationControllerCompBase.h>


namespace imtloggui
{


class CProductionRepresentationControllerComp: public CRepresentationControllerCompBase
{
public:
	typedef CRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductionRepresentationControllerComp)
		I_ASSIGN(m_granularityAttrPtr, "Granularity", "Statistics time granularity in seconds", true, 60);
	I_END_COMPONENT

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	// reimplemented (CRepresentationControllerCompBase)
	virtual void BuildRepresentation(
				istd::IChangeable& representation,
				imtlog::IEventProvider::EventContainerPtr containerPtr,
				const imtbase::CTimeRange& timeRange) const override;

private:
	I_ATTR(int, m_granularityAttrPtr);
};


} // namespace imtloggui


