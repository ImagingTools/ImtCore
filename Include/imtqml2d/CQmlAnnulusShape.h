// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <i2d/CAnnulus.h>
#include <imod/TSingleModelObserverBase.h>

/// \todo REMOVWE IT!
#include "dummy.h"


namespace imtqml2d
{


class CQmlAnnulusShape: public sdl::imtsdl2d::imt2d::CAnnulus, public imod::TSingleModelObserverBase<i2d::CAnnulus>
{
	Q_OBJECT

public:
	typedef imod::CSingleModelObserverBase BaseClass;
	typedef CAnnulus BaseClass2;

	[[nodiscard]] bool IsValid() const;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (sdl::imtsdl2d::imt2d::CAnnulus)
	virtual void SetCenter(sdl::imtsdl2d::imt2d::CPoint2d newCenter) override;
	virtual void SetInnerRadius(double newInnerRadius) override;
	virtual void SetOuterRadius(double newOuterRadius) override;
};


} // namespace imtqml2d
