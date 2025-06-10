/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtdev
{


/**
	Information about current device connection state
*/
class IDeviceConnectionState: virtual public istd::IChangeable
{
public:

	enum ChangeFlags
	{
		CF_CONNECTION_STATE_CHANGED = istd::IChangeable::ChangeFlags::CF_NO_UNDO + 7171
	};

	/**
		Check connection state for device
		\return \c true if the device is connected, otherwise \c false.
	*/
	virtual bool IsDeviceConnected(const QByteArray& deviceId) = 0;
};


} // namespace imtdev


