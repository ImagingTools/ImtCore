// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtPositioning/QGeoCoordinate>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtgeo
{


/**
	\brief Struct representing a map tile coordinate.

	Represents tiles defined by standard Slippy map tile names or coordinate metrics:
	X-axis coordinate index, Y-axis coordinate index, and zoom level Z.
 */
struct Tile
{
	qint64 x = 0;	///< Tile x coordinate (longitude slippy index).
	qint64 y = 0;	///< Tile y coordinate (latitude slippy index).
	int z = 0;		///< Zoom level of the tile.

	/**
		\brief Structural equality check operator.
		\param other The other coordinate details to compare to.
		\return True if X, Y, and Z fields match, false otherwise.
	*/
	bool operator==(const Tile& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}

	/**
		\brief Structural inequality check operator.
		\param other The other coordinate details to compare to.
		\return True if X, Y, or Z fields differ, false otherwise.
	*/
	bool operator!=(const Tile& other) const
	{
		return !(*this == other);
	}
};


class CTileProvider: virtual public istd::IPolymorphic
{

protected:
	qint64 long2tilex(double lon, int z) const;
	qint64 lat2tiley(double lat, int z) const;

	double tilex2long(qint64 x, int z) const;
	double tiley2lat(qint64 y, int z) const;

	/**
		\brief Computes the specific tile information details for a geographical coordinate at a zoom level.
		\param coord Geographical coordinate.
		\param z Zoom level.
		\return The computed Tile structure.
	*/
	Tile getTile(const QGeoCoordinate& coord, int z) const;

	qint64 getTileNumber(const QGeoCoordinate& coord, int z) const;

	/**
		\brief Computes a list of slide-bounded map tiles matching the rectangular frame box.
		\param coordLeftTop North-west box bounding coordinate.
		\param coordRightBottom South-east box bounding coordinate.
		\param z Zoom level.
		\return List containing the matching Tile structures.
	*/
	QList<Tile> getTileSet(
				const QGeoCoordinate& coordLeftTop,
				const QGeoCoordinate& coordRightBottom,
				int z) const;
};


} // namespace imtgeo

