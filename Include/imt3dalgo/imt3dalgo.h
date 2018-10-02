#pragma once


// Qt includes
#include <QtCore/QLinkedList>

// ACF includes
#include <imath/imath.h>
#include <i3d/CVector3d.h>


/**
	Contains 3D point cloud and mesh processing algorithms.
	This package is system independent.
	This package uses following ACF packages:
	\li istd
	\li iser
	\li imath

	\ingroup 3D
*/
namespace imt3dalgo
{


typedef QLinkedList<i3d::CVector3d> CloudPoints;


} // namespace imt3dalgo

