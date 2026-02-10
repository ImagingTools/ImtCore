// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrest
{


/**
 * \brief The IBinaryDataProvider class provides file from disc, resources or memory
 */
class IBinaryDataProvider:  virtual public istd::IPolymorphic
{
public:
	/*!
		\brief extracts data from providing file and puts it in data param
		\param data [OUTPUT] - data of providing file
		\param dataId [INPUT] - id of providing file
		\param readFromPosition - sets the current position to start reading
		\param readMaxLength - sets maximum size of data to be read \note if it is negative - reads as much data as possible
		\return result of extracting data
	 */

	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const = 0;

};


} // namespace imtrest


