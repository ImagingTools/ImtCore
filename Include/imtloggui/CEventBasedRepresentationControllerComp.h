// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtloggui/CRepresentationControllerCompBase.h>


namespace imtloggui
{


class CEventBasedRepresentationControllerComp: public CRepresentationControllerCompBase
{
public:
	typedef CRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CEventBasedRepresentationControllerComp)
	I_END_COMPONENT

protected:
	// reimplemented (CRepresentationControllerCompBase)
	virtual void BuildRepresentation(
				istd::IChangeable& representation,
				imtlog::IEventProvider::EventContainerPtr containerPtr,
				const imtbase::CTimeRange& timeRange) const override;
};


} // namespace imtloggui


