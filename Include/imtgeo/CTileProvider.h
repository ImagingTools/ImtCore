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
	/**
		\brief Converts a longitude value to the corresponding X tile coordinate index at a specific zoom level.
		\param lon Longitude in degrees.
		\param z Zoom level.
		\return The computed X tile coordinate index as a 64-bit integer.
	*/
	qint64 long2tilex(double lon, int z) const;
	/**
		\brief Converts a latitude value to the corresponding Y tile coordinate index at a specific zoom level.

		The conversion accounts for the Mercator projection used in Slippy maps, which causes the Y tile index to increase as latitude decreases.
		\param lat Latitude in degrees.
		\param z Zoom level.
		\return The computed Y tile coordinate index as a 64-bit integer.
	*/
	qint64 lat2tiley(double lat, int z) const;
	/**
		\brief Converts an X tile coordinate index back to the corresponding longitude value at a specific zoom level.
		\param x X tile coordinate index.
		\param z Zoom level.
		\return The computed longitude in degrees.
	*/
	double tilex2long(qint64 x, int z) const;
	/**
		\brief Converts a Y tile coordinate index back to the corresponding latitude value at a specific zoom level.

		The conversion accounts for the Mercator projection used in Slippy maps, which causes the Y tile index to increase as latitude decreases.
		\param y Y tile coordinate index.
		\param z Zoom level.
		\return The computed latitude in degrees.
	*/
	double tiley2lat(qint64 y, int z) const;
	/**
		\brief Computes the specific tile information details for a geographical coordinate at a zoom level.
		\param coord Geographical coordinate.
		\param z Zoom level.
		\return The computed Tile structure.
	*/
	Tile getTile(const QGeoCoordinate& coord, int z) const;
	/**
		\brief Computes the tile number for a geographical coordinate at a zoom level.

		The tile number is a unique identifier for a tile at a specific zoom level, calculated based on the X and Y tile coordinates.
		\param coord Geographical coordinate.
		\param z Zoom level.
		\return The computed tile number as a 64-bit integer.
	*/
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

