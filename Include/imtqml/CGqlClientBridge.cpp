// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlClientBridge.h>


namespace imtqml
{


CGqlClientBridge* CGqlClientBridge::s_instancePtr = nullptr;


// public methods

CGqlClientBridge::CGqlClientBridge(QObject* parent)
	:QObject(parent),
	BaseClass()
{
}


CGqlClientBridge::~CGqlClientBridge() = default;


CGqlClientBridge* CGqlClientBridge::Instance()
{
	return s_instancePtr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CGqlClientBridge::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	s_instancePtr = this;
}


void CGqlClientBridge::OnComponentDestroyed()
{
	if (s_instancePtr == this){
		s_instancePtr = nullptr;
	}
	BaseClass::OnComponentDestroyed();
}


} // namespace imtqml
