// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual PointFormat GetPointFormat() const override;
	virtual int GetPointsCount() const override;
	virtual const void* GetPointData(int pointIndex) const override;
	virtual void* GetPointData(int pointIndex) override;
	virtual void* GetData() override;
	virtual const void* GetData() const override;
	virtual int GetPointBytesSize() const override;


	// reimplemented (imt3d::IObject3d)
	virtual bool IsEmpty() const override;
	virtual i3d::CVector3d GetCenter() const override;
	virtual void MoveCenterTo(const i3d::CVector3d& position) override;
	virtual CCuboid GetBoundingCuboid() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	bool Create(PointFormat pointFormat);
	bool Create(PointFormat pointFormat, int pointsCount, const void* dataPtr);
	bool Append(int pointsCount, const void* dataPtr);
	int GetDataSize() const;
	void EnsureCenterCalculated() const;
	void EnsureCuboidCalculated() const;

	template <typename PointType> const PointType*
		TGetPointData(int pointIndex, bool validOnly = false) const;
	template <typename PointType> PointType*
		TGetPointData(int pointIndex, bool validOnly = false);
	template <typename PointType> bool TIsPointValid(const PointType& pointData) const;
	template <typename PointType> void TEnsureCenterCalculated() const;
	template <typename PointType> void TEnsureCuboidCalculated() const;
	template <typename PointType> void TMoveCenterTo(const i3d::CVector3d& position);
	template <typename PointType> void GetBoundingRanges(istd::CRange& xRange, istd::CRange& yRange, istd::CRange& zRange) const;

	// reimplemented (istd::IChangeable)
	virtual void OnEndChanges(const ChangeSet& changes) override;

protected:
	static int GetPointBytesSize(PointFormat pointFormat);
	static int GetBufferSize(PointFormat pointFormat, int pointsCount);

protected:
	std::vector<uint8_t> m_data;
	PointFormat m_pointFormat;
	int m_pointsCount;

	mutable i3d::CVector3d m_cloudCenter;
	mutable CCuboid m_boundingCuboid;
	mutable bool m_isCenterCalculationValid;
	mutable bool m_isCuboidCalculationValid;
};


} // namespace imt3d


