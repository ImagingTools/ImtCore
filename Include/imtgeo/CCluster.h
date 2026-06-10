// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtPositioning/QGeoCoordinate>


// ImtCore includes
#include <imtgeo/ICluster.h>
#include <imtgeo/CPosition.h>


namespace imtgeo
{


class CCluster: public CPositionIdentifiable, virtual public ICluster
{
public:
	using BaseClass = CPositionIdentifiable;

	CCluster();

	// reimplemented (imtgeo::ICluster)
	virtual QByteArrayList GetChildIds() const override;
	virtual void SetChildIds(const QByteArrayList& list) override;

	virtual double GetZoom() const override;
	virtual void SetZoom(double zoom) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	QByteArrayList m_childrenIds;
	double m_zoom;
};


} // namespace imtgeo

