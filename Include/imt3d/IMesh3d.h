#pragma once


// Qt includes
#include <QtCore/QLinkedList>
#include <QVector>

// ACF includes
#include <i3d/CVector3d.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/IObject3d.h>


namespace imt3d
{


/**
	Common interface for describing the mesh on 3D-points.
*/
class IMesh3d: virtual public IObject3d
{
public:
	/**
		Data model change notification flags
	*/
	enum ChangeFlags
	{
		CF_VERTICES,
		CF_CUBOID
	};

	typedef imath::TVector<3, float> FloatVector3d;
	typedef imath::TVector<3, int> IntVector3d;
	typedef FloatVector3d Vertex;
	typedef FloatVector3d Normal;

	/**
		Edge defined through coordinates of its points
	*/
	typedef QPair<FloatVector3d, FloatVector3d> Edge;

	/**
		Edge defined trough indices of its points
	*/
	typedef QPair<int, int> IndexEdge;

	/**
		Contains indices of the corresp Vertices
	*/
	typedef IntVector3d Triangle;


	typedef QVector<Vertex> MeshVertices;
	typedef QVector<Normal> MeshNormals;
	typedef QVector<Edge> MeshEdges;
	typedef QVector<IndexEdge> MeshIndexEdges;
	typedef QVector<Triangle> MeshTriangles;
	
	/**
		Mesh object pointer
	*/
	typedef istd::TSmartPtr<IMesh3d> MeshPtr;

	typedef istd::TSmartPtr<MeshEdges> MeshEdgesPtr;
	typedef istd::TSmartPtr<MeshIndexEdges> MeshIndexEdgesPtr;

	//virtual void CreateMesh(/*const MeshTriangles& triangles*/) = 0;
	
	/**
		Get list of vertices
	*/
	virtual const MeshVertices& GetVertices() const = 0;

	/**
		Get the list of edges.
	*/
	virtual const MeshEdgesPtr GetEdges() const = 0;

	/**
		Get the list of index edges.
	*/
	virtual const MeshIndexEdgesPtr GetIndexEdges() const = 0;

	/**
		Get list of triangles
	*/
	virtual const MeshTriangles& GetTriangles() const = 0;

	/**
		Get list of normals
	*/
	virtual const MeshNormals& GetNormals() const = 0;

};


} // namespace imt3d


