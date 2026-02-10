// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtDev includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtgeo/IPosition.h>


namespace imtgeo
{
/**
	Class for describing the coordinates of an object with id.
*/
class CPosition: virtual public IPosition
{
public:
	CPosition();
	~CPosition();

	// reimplemented (imtgeo::IPosition)
	virtual double GetLatitude() const override;
	virtual void SetLatitude(const double& lat) override;
	virtual double GetLongitude() const override;
	virtual void SetLongitude(const double& lon) override;
	virtual int GetZoomLevel() const override;
	virtual void SetZoomLevel(const double& zoom) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	double m_latitude;
	double m_longitude;
	double m_zoomLevel;
};


typedef imtbase::TIdentifiableWrap<CPosition> CPositionIdentifiable;


} // namespace imtgeo

