// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <i2d/CRectangle.h>
#include <imod/TSingleModelObserverBase.h>

/// \todo REMOVWE IT!
#include "dummy.h"


namespace imtqml2d
{

class CQmlRectangleShape: public sdl::imtsdl2d::imt2d::CRectangle2d, public imod::TSingleModelObserverBase<i2d::CRectangle>
{
	Q_OBJECT

public:
	typedef imod::CSingleModelObserverBase BaseClass;
	typedef sdl::imtsdl2d::imt2d::CRectangle2d BaseClass2;

	[[nodiscard]] bool IsValid() const;

public:
	virtual void SetTopLeft(sdl::imtsdl2d::imt2d::CPoint2d newTopLeft) override;
	virtual void SetBottomRight(sdl::imtsdl2d::imt2d::CPoint2d newBottomRight) override;

protected:
	// reimplemented (imod::TSingleModelObserverBase)
	virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask) override;
	virtual bool OnModelDetached(imod::IModel* modelPtr) override;
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
};


} // namespace imtqml2d
