#include <imt3d/CPointCloud3d.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/TRange.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imt3d/IObject3d.h>
#include <i3d/CVector3d.h>


namespace imt3d
{


// public methods

// reimplemented (IPointCloud3d)

bool CPointCloud3d::CreateCloud(PointFormat pointFormat, int pointsCount, const istd::CIndex2d* gridSizePtr)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = Create(pointFormat, pointsCount, nullptr, true);

	if (retVal && gridSizePtr){
		m_gridSize = *gridSizePtr;
	}

	return retVal;
}


bool CPointCloud3d::CreateCloud(PointFormat pointFormat,
			int pointsCount,
			void* dataPtr,
			bool releaseFlag,
			const istd::CIndex2d* gridSizePtr)
{
	if (!dataPtr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	bool retVal = Create(pointFormat, pointsCount, dataPtr, releaseFlag);

	if (retVal && gridSizePtr){
		m_gridSize = *gridSizePtr;
	}

	return retVal;
}


// reimplemented (imt3d::IGridInfo)

istd::CIndex2d CPointCloud3d::GetGridSize() const
{
	return m_gridSize;
}


void CPointCloud3d::SetGridSize(const istd::CIndex2d& gridSize)
{
	if (m_gridSize != gridSize){
		istd::CChangeNotifier changeNotifier(this);

		m_gridSize = gridSize;
	}
}


istd::CIndex2d CPointCloud3d::GetGridPosition(int index) const
{
	if (!m_gridSize.IsSizeEmpty() && m_gridSize.IsValid()){
		int y = index / m_gridSize.GetX();
		int x = index % m_gridSize.GetX();

		return istd::CIndex2d(x, y);
	}

	return istd::CIndex2d::GetInvalid();
}


int CPointCloud3d::GetCloudPosition(const istd::CIndex2d& index) const
{
	return m_gridSize.GetX() * index.GetY() + index.GetX();
}


// reimplemented (iser::ISerializable)

bool CPointCloud3d::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag gridSizeXTag("GridSizeX", "Size of points grid throw x", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag gridSizeYTag("GridSizeY", "Size of points grid throw y", iser::CArchiveTag::TT_LEAF);

	bool retVal = CPointsBasedObject::Serialize(archive);

	if (retVal){
		int gridSizeX = m_gridSize.GetX();
		int gridSizeY = m_gridSize.GetY();

		retVal = retVal && archive.BeginTag(gridSizeXTag);
		retVal = retVal && archive.Process(gridSizeX);
		retVal = retVal && archive.EndTag(gridSizeXTag);

		retVal = retVal && archive.BeginTag(gridSizeYTag);
		retVal = retVal && archive.Process(gridSizeY);
		retVal = retVal && archive.EndTag(gridSizeYTag);

		if (!archive.IsStoring()){
			m_gridSize.SetX(gridSizeX);
			m_gridSize.SetY(gridSizeY);
		}
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CPointCloud3d::GetSupportedOperations() const
{
	return SO_CLONE | SO_COMPARE | SO_COPY | SO_RESET;
}


bool CPointCloud3d::CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode mode)
{
	bool retVal = CPointsBasedObject::CopyFrom(object, mode);

	if (retVal){
		const CPointCloud3d* objectPtr = dynamic_cast<const CPointCloud3d*>(&object);
		if (objectPtr != nullptr){
			istd::CChangeNotifier changeNotifier(this);

			m_gridSize = objectPtr->m_gridSize;
		}
	}

	return retVal;
}


bool CPointCloud3d::IsEqual(const IChangeable & object) const
{
	const CPointCloud3d* sourcePtr = dynamic_cast<const CPointCloud3d*>(&object);
	if (sourcePtr != nullptr){

		if (BaseClass::GetSupportedOperations() & SO_COMPARE){
			if (!BaseClass::IsEqual(object)){
				return false;
			}
		}

		return (m_gridSize == sourcePtr->m_gridSize);
	}

	return false;
}


istd::IChangeable* CPointCloud3d::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CPointCloud3d> clonePtr(new CPointCloud3d());

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CPointCloud3d::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = CPointsBasedObject::ResetData(mode);

	if (retVal){
		m_gridSize.Reset();
	}

	return retVal;
}


} // namespace imt3d


