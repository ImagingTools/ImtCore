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
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	bool Create(PointFormat pointFormat, int pointsCount, void* dataPtr, bool releaseFlag);
	bool Append(int pointsCount, void* dataPtr);
	int GetDataSize() const;
	void CreateInternalBuffer();
	bool AllocateData(int size, quint8*& buffer);
	void FreeData();
	void EnsureCenterCalculated() const;
	void EnsureCuboidCalculated() const;

	template <typename PointType> PointType*
	TGetPointData(int pointIndex, bool validOnly = false) const;
	template <typename PointType>
	void AllocateObjectBuffer();
	template <typename PointType> void TFreeData();
	template <typename PointType> bool TIsPointValid(const PointType& pointData) const;
	template <typename PointType> void TEnsureCenterCalculated() const;
	template <typename PointType> void TEnsureCuboidCalculated() const;
	template<typename PointType> void TMoveCenterTo(const i3d::CVector3d& position);
	template<typename PointType> void GetBoundingRanges(istd::CRange& xRange, istd::CRange& yRange, istd::CRange& zRange) const;

	// reimplemented (istd::IChangeable)
	void OnEndChanges(const ChangeSet& changes) override;

protected:
	template <typename PointType>
	static bool AllocateInternal(int size, quint8*& buffer);
	static int GetBufferSize(PointFormat pointFormat, int pointsCount);

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


