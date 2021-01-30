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
	virtual bool CreateMesh(PointFormat pointFormat, int pointsCount, const void* pointsDataPtr, const Indices& indices) = 0;

	virtual bool InsertData(int pointsCount, const void* pointsDataPtr, const Indices& indices) = 0;

	/**
		Get mesh indices.
	*/
	virtual const Indices& GetIndices() const = 0;
};


} // namespace imt3d


