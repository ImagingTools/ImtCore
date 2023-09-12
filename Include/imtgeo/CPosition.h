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
		Get latitude address.
	*/
    virtual double GetLatitude() const override;

	/**
		Set latitude address.
	*/
    virtual void SetLatitude(double lat) override;

	/**
		Get longitude address.
	*/
    virtual double GetLongitude() const override;

	/**
		Set longitude address.
	*/
    virtual void SetLongitude(double lon) override;

    // reimplemented (iser::ISerializable)
    virtual bool Serialize(iser::IArchive& archive) override;


private:

    QGeoCoordinate m_coordinate = QGeoCoordinate();

};

typedef imtbase::TIdentifiableWrap<CPosition> CPositionIdentifiable;


} // namespace imtgeo


