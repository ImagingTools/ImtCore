#pragma once


// Qt includes
#include <QtGui/QVector3D>

// Acf includes
#include <istd/IPolymorphic.h>


namespace imt3dview
{


/**
	Basic interface for pointing to target
*/
class ITargetPointer: virtual public istd::IPolymorphic
{
public:
	/**
		Creates Target on vectorTo point and trajectory from vectorFrom point
	*/
	virtual void SetTargetPointer(QVector3D vectorFrom, QVector3D vectorTo) = 0;
};


} // namespace imt3dview


