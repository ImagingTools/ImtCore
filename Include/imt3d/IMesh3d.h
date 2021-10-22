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


// STL includes
#include <vector>

// ImtCore includes
#include <imt3d/IObject3d.h>
#include <imt3d/IPointsBasedObject.h>


namespace imt3d
{


/**
	Common interface describing a mesh on 3D-points.
*/
class IMesh3d: virtual public IPointsBasedObject
{
public:
	typedef std::vector<uint32_t> Face;
	typedef std::vector<Face> Indices;

	/**
		Create mesh with specified point format.
	*/
	virtual bool CreateMesh(PointFormat pointFormat) = 0;

	/**
		Create mesh with specified point format using external data buffer.
	*/
	virtual bool CreateMesh(PointFormat pointFormat, size_t pointsCount, const void* pointsDataPtr, const Indices& indices) = 0;

	virtual bool InsertData(size_t pointsCount, const void* pointsDataPtr, const Indices& indices) = 0;

	/**
		Get mesh indices.
	*/
	virtual const Indices& GetIndices() const = 0;
};


} // namespace imt3d


