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


#include <imtauth/CRoleMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CRoleMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* /*dataPtr*/,
			const QByteArray& /*typeId*/,
			idoc::MetaInfoPtr& /*metaInfoPtr*/) const
{
	return true;
}


// public methods of embedded class MetaInfo

QString CRoleMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth


