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
	bool CreateMesh(PointFormat pointFormat, int pointsCount, const Indices& indices) override;
	bool CreateMesh(PointFormat pointFormat, int pointsCount, void* pointsDataPtr, bool pointsDataReleaseFlag, const Indices& indices) override;
	const Indices& GetIndices() const override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

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


