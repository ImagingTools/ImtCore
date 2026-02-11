// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <i2d/CLine2d.h>
#include <imod/TSingleModelObserverBase.h>

/// \todo REMOVWE IT!
#include "dummy.h"


namespace imtqml2d
{


class CQmlLineShape: public sdl::imtsdl2d::imt2d::CLine2d, public imod::TSingleModelObserverBase<i2d::CLine2d>
{
	Q_OBJECT

public:
	typedef imod::CSingleModelObserverBase BaseClass;
	typedef CLine2d BaseClass2;

	[[nodiscard]] bool IsValid() const;

	// reimplemented (imod::IObserver)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented CLine2d
	virtual void SetPoint1(sdl::imtsdl2d::imt2d::CPoint2d newPoint1) override;
	virtual void SetPoint2(sdl::imtsdl2d::imt2d::CPoint2d newPoint2) override;
};


} // namespace imtqml2d
