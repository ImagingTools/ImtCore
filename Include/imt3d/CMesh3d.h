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


// Qt includes
#include <QtCore/QFile>

// ImtCore includes
#include <imt3d/IMesh3d.h>
#include <imt3d/CPointsBasedObject.h>


namespace imt3d
{


/**
	Triangle mesh implementation.
*/
class CMesh3d: virtual public IMesh3d, public CPointsBasedObject
{
public:
	typedef CPointsBasedObject BaseClass;

	bool SaveToStlFile(const QString& filePath) const;
	bool LoadFromStlFile(const QString& filePath, bool ensureNormalExists);

	// reimplemented (imt3d::IMesh3d)
	virtual bool CreateMesh(PointFormat pointFormat) override;
	virtual bool CreateMesh(PointFormat pointFormat, size_t pointsCount, const void* pointsDataPtr, const Indices& indices) override;
	virtual bool InsertData(size_t pointsCount, const void* pointsDataPtr, const Indices& indices) override;
	virtual const Indices& GetIndices() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	template <typename PointType> bool SaveToStlFile(const QString& filePath) const;
	template <typename DataType> bool WriteTypedValue(const DataType& data, QFile& file) const;
	template <typename PointType> bool WritePointData(int pointIndex, int pointComponentOffset, QFile& file) const;
	bool ReadPointData(QFile& file, float* pointBufPtr) const;
	void EnsureNormalExists(
				IPointsBasedObject::PointXyzwNormal32& point1,
				IPointsBasedObject::PointXyzwNormal32& point2,
				IPointsBasedObject::PointXyzwNormal32& point3) const;

private:
	Indices m_indices;
	static const int s_stlHeaderSize = 80;
};


} // namespace imt3d


