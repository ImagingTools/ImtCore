// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CCluster.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtgeo
{


// public methods

CCluster::CCluster()
	: m_zoom(0.0)
{
}


// reimplemented (imtgeo::ICluster)

QByteArrayList CCluster::GetChildIds() const
{
	return m_childrenIds;
}


void CCluster::SetChildIds(const QByteArrayList& list)
{
	if(m_childrenIds != list){
		m_childrenIds = list;

		istd::CChangeNotifier notifier(this);

	}
}


double CCluster::GetZoom() const
{
	return m_zoom;
}


void CCluster::SetZoom(double zoom)
{
	if(zoom != m_zoom){
		m_zoom = zoom;

		istd::CChangeNotifier notifier(this);
	}
}


bool CCluster::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_childrenIds, QByteArrayLiteral("ObjectIds"), QByteArrayLiteral("ObjectId"));

	static iser::CArchiveTag zoomLevelTag("ZoomLevel", "Map zoom level", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(zoomLevelTag);
	retVal = retVal && archive.Process(m_zoom);
	retVal = retVal && archive.EndTag(zoomLevelTag);

	return retVal;
}


} // namespace imtgeo

