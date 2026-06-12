// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QSharedPointer>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtgeo/CPosition.h>
#include <imtgeo/CCluster.h>


namespace imtgeo
{


/**
 \class CClusterCreator
 \brief Class responsible for grouping spatial objects and creating map clusters.
 */
class CClusterCreator: virtual public istd::IPolymorphic
{
protected:
	/**
	 \brief Converts a geographical coordinate to Mercator coordinate projection.
	 \param coord The geographical coordinate.
	 \return A pair of latitude and longitude coordinates projected in Mercator space.
	 */
	QPair<double, double> coordToMercator(const QGeoCoordinate& coord) const;
	/**
	 \brief Calculates the Cartesian distance between two geographical coordinates in projected Mercator space.
	 \param coord1 First geographical coordinate.
	 \param coord2 Second geographical coordinate.
	 \return The distance between the elements.
	 */
	double getDistanceBetweenObjects(const QGeoCoordinate& coord1, const QGeoCoordinate& coord2) const;

	/**
	 \brief Computes the distance limit coefficient for a specific zoom level.
	 \param zoomLevel The map zoom level.
	 \return Distance limit coefficient as a double.
	 */
	double getDistanceLimitCoeff(double zoomLevel) const;
	/**
	 \brief Groups geographical items within a JSON array model.
	 \param model The source JSON array of objects.
	 \param zoomLevel The map zoom level.
	 \param limitInPixels Pixel limit for clustering.
	 \return JSON array containing clustered and non-clustered map objects.
	 */
	QJsonArray createMapClusterModel(
				const QJsonArray& model,
				double zoomLevel,
				double limitInPixels = 50.0) const;
	/**
	 \brief Groups a list of identifiable position objects into CCluster structures.
	 \param objectList The list of spatial objects.
	 \param zoomLevel The map zoom level.
	 \param limitInPixels Pixel limit for clustering.
	 \return List of shared pointer cluster nodes.
	 */
	QList<ClusterSharedPtr> createMapClusters(
				const QList<CPositionIdentifiable*>& objectList,
				double zoomLevel,
				double limitInPixels = 50) const;
	/**
	 \brief Converts a list of position objects directly to simple single-child clusters without grouping.
	 \param objectList The list of spatial objects.
	 \param zoomLevel The map zoom level.
	 \return List of simple single-child cluster nodes.
	 */
	QList<ClusterSharedPtr> convertToMapClusters(const QList<CPositionIdentifiable*>& objectList, double zoomLevel) const;
};


} // namespace imtgeo

