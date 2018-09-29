#pragma once


// Qt includes
#include <QtCore/QLinkedList>

// ACF includes
#include <imath/imath.h>
#include <i3d/CVector3d.h>

/**
	Contains the 3D primitives.
	This package is system independent.
	This package use following ACF packages:
	\li istd
	\li iser
	\li imath

	\ingroup 3D
*/
namespace imt3d
{


typedef i3d::CVector3d Point3d;
typedef QLinkedList<Point3d> CloudPoints;


} // namespace imt3d

