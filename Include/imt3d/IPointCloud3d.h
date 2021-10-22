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


// ImtCore includes
#include <imt3d/IPointsBasedObject.h>


namespace imt3d
{


/**
	Common interface describing a cloud of 3D-points.
*/
class IPointCloud3d: virtual public IPointsBasedObject
{
public:
	/**
		Create point cloud with specified point format.
	*/
	virtual bool CreateCloud(PointFormat pointFormat) = 0;

	/**
		Create point cloud with specified point format using external data buffer.
	*/
	virtual bool CreateCloud(
				PointFormat pointFormat,
				size_t pointsCount,
				const void* dataPtr,
				const istd::CIndex2d* gridSizePtr = nullptr) = 0;

	/**
		Append points to existing data in the point cloud.
		The points should be in the format specified by \c CreateCloud.
	*/
	virtual bool InsertPoints(
				size_t pointsCount,
				const void* dataPtr) = 0;
};


} // namespace imt3d


