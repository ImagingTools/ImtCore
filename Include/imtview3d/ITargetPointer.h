#pragma once


// Qt includes
#include <QtGui/QVector3D>

// Acf includes
#include <istd/IPolymorphic.h>


namespace imtview3d
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


} // namespace imtview3d


