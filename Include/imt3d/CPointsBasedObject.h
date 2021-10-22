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

// ACF includes
#include <istd/TRange.h>

// ImtCore includes
#include <imt3d/IPointsBasedObject.h>


namespace imt3d
{


/**
	Common implementation of a 3D-points data.
*/
class CPointsBasedObject: virtual public IPointsBasedObject
{
public:
	CPointsBasedObject();

	// reimplemented (imt3d::IPointsBasedObject)
	PointFormat GetPointFormat() const override;
	int GetPointsCount() const override;
	const void* GetPointData(size_t pointIndex) const override;
	void* GetPointData(size_t pointIndex) override;
	virtual void* GetData() override;
	virtual const void* GetData() const override;
	virtual int GetPointBytesSize() const override;


	// reimplemented (imt3d::IObject3d)
	bool IsEmpty() const override;
	i3d::CVector3d GetCenter() const override;
	void MoveCenterTo(const i3d::CVector3d& position) override;
	CCuboid GetBoundingCuboid() const override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	bool Create(PointFormat pointFormat);
	bool Create(PointFormat pointFormat, size_t pointsCount, const void* dataPtr);
	bool Append(size_t pointsCount, const void* dataPtr);
	int GetDataSize() const;
	void EnsureCenterCalculated() const;
	void EnsureCuboidCalculated() const;

	template <typename PointType> const PointType*
		TGetPointData(size_t pointIndex, bool validOnly = false) const;
	template <typename PointType> PointType*
		TGetPointData(size_t pointIndex, bool validOnly = false);
	template <typename PointType> bool TIsPointValid(const PointType& pointData) const;
	template <typename PointType> void TEnsureCenterCalculated() const;
	template <typename PointType> void TEnsureCuboidCalculated() const;
	template <typename PointType> void TMoveCenterTo(const i3d::CVector3d& position);
	template <typename PointType> void GetBoundingRanges(istd::CRange& xRange, istd::CRange& yRange, istd::CRange& zRange) const;

	// reimplemented (istd::IChangeable)
	void OnEndChanges(const ChangeSet& changes) override;

protected:
	static int GetPointBytesSize(PointFormat pointFormat);
	static int GetBufferSize(PointFormat pointFormat, size_t pointsCount);

protected:
	std::vector<uint8_t> m_data;
	PointFormat m_pointFormat;
	size_t m_pointsCount;

	mutable i3d::CVector3d m_cloudCenter;
	mutable CCuboid m_boundingCuboid;
	mutable bool m_isCenterCalculationValid;
	mutable bool m_isCuboidCalculationValid;
};


} // namespace imt3d


