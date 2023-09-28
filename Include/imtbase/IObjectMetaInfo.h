/********************************************************************************
**
**	Copyright (C) 2007-2017 Witold Gantzke & Kirill Lepskiy
**
**	This file is part of the ACF Toolkit.
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
** 	See http://www.ilena.org or write info@imagingtools.de for further
** 	information about the ACF.
**
********************************************************************************/

#pragma once


// Qt includes
#include <QtCore/QVariant>

// ACF includes
#include <istd/TSmartPtr.h>
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for object meta-information.
*/
class IObjectMetaInfo: virtual public istd::IChangeable
{
public:

	/**
		Data model change notification flags.
	*/
	enum ChangeFlags
	{
		/**
			Meta information has been changed.
		*/
		CF_OBJECT_METAINFO = 0xd83067d
	};

	
	/**
		Get object's meta info.
	*/
	virtual QVariant GetMetaInfo(QByteArray metaInfoId) const = 0;

	/**
		Set new object's meta information for a given meta type.
	*/
	virtual bool SetMetaInfo(QByteArray metaInfoId, const QVariant& metaInfo) = 0;

	/**
		Check if meta information can be changed.
	*/
	virtual bool IsMetaInfoWritable(QByteArray metaInfoId) const = 0;
};


typedef istd::TSmartPtr<IObjectMetaInfo> ObjectMetaInfoPtr;


} // namespace imtbase

