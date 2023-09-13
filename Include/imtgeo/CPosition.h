#pragma once

// Qt includes
#include <QtPositioning/QGeoCoordinate>

// ImtDev includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtgeo/IPosition.h>

namespace imtgeo
{
/**
    Class for describing the coordinates of an object with id.
*/
class CPosition: virtual public imtgeo::IPosition
{
public:

    CPosition();

    ~CPosition();

    // reimplemented (imtgeo::IPosition)
	/**
        Get latitude of object.
	*/
    virtual double GetLatitude() const override;

	/**
        Set latitude of object.
	*/
    virtual void SetLatitude(double lat) override;

	/**
        Get longitude of object.
	*/
    virtual double GetLongitude() const override;

	/**
        Set longitude of object.
	*/
    virtual void SetLongitude(double lon) override;

    // reimplemented (iser::ISerializable)
    virtual bool Serialize(iser::IArchive& archive) override;


private:

    QGeoCoordinate m_coordinate = QGeoCoordinate(0.0,0.0);

};

typedef imtbase::TIdentifiableWrap<CPosition> CPositionIdentifiable;


} // namespace imtgeo


