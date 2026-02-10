// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgeo/IPosition.h>


namespace imtgeo
{
/**
    Interface for describing a cluster of objects on the map.
*/

class ICluster:  virtual public IPosition
{
public:

    /**
        Get id list of objects in cluster.
    */
    virtual QByteArrayList GetChildrenIds() const = 0;

    /**
        Set id list of objects in cluster.
    */
    virtual void SetChildrenIds(QByteArrayList &list) = 0;

    /**
        Get zoom level of cluster.
    */
    virtual double GetZoom() const = 0;

    /**
        Set zoom level of cluster.
    */
    virtual void SetZoom(double zoom) = 0;

};


} // namespace imtgeo


