// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


namespace imtdesign
{

class IDesignTokenProcessor
{
public:
	/**
		\brief Processing the file
		\return result of processing
		\retval 0 - success \retval !0 - fail
	 */
	virtual int Exec() = 0;
	virtual QByteArray GetHelpString()const = 0;

};


} // namespace imtdesign
