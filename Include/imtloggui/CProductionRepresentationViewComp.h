// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/CRepresentationViewCompBase.h>
#include <imtloggui/CProductionSpeedItem.h>


namespace imtloggui
{


class CProductionRepresentationViewComp: public CRepresentationViewCompBase
{
public:
	typedef CRepresentationViewCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductionRepresentationViewComp)
	I_END_COMPONENT

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
};


} // namespace imtloggui


