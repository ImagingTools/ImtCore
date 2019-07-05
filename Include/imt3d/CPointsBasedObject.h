#pragma once


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
	~CPointsBasedObject();

	// reimplemented (imt3d::IPointsBasedObject)
	PointFormat GetPointFormat() const override;
	int GetPointsCount() const override;
	const void* GetPointData(int pointIndex) const override;
	void* GetPointData(int pointIndex) override;

	// reimplemented (imt3d::IObject3d)
	bool IsEmpty() const override;
	i3d::CVector3d GetCenter() const override;
	void MoveCenterTo(const i3d::CVector3d& position) override;
	CCuboid GetBoundingCuboid() const override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	bool CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode mode) override;
	bool ResetData(istd::IChangeable::CompatibilityMode mode = istd::IChangeable::CM_WITHOUT_REFS) override;

protected:
	bool Create(PointFormat pointFormat, int pointsCount, void* dataPtr, bool releaseFlag);
	template <typename PointType> PointType* TGetPointData(int pointIndex, bool validOnly = false) const;
	int GetDataSize() const;
	void AllocateData();
	template <typename PointType> void TAllocateData();
	void FreeData();
	template <typename PointType> void TFreeData();
	template <typename PointType> bool TIsPointValid(const PointType& pointData) const;
	void EnsureCenterCalculated() const;
	template <typename PointType> void TEnsureCenterCalculated() const;
	void EnsureCuboidCalculated() const;
	template <typename PointType> void TEnsureCuboidCalculated() const;
	template<typename PointType> void TMoveCenterTo(const i3d::CVector3d& position);
	template<typename PointType> void GetBoundingRanges(istd::CRange& xRange, istd::CRange& yRange, istd::CRange& zRange) const;

	// reimplemented (istd::IChangeable)
	void OnEndChanges(const ChangeSet& changes) override;

protected:
	quint8* m_dataPtr;
	bool m_dataOwner;
	PointFormat m_pointFormat;
	int m_pointsCount;

	mutable i3d::CVector3d m_cloudCenter;
	mutable CCuboid m_boundingCuboid;
	mutable bool m_isCenterCalculationValid;
	mutable bool m_isCuboidCalculationValid;
};


} // namespace imt3d


