#pragma once


// ACF includes
#include <i3d/CVector3d.h>

// ImtCore includes
#include <imt3d/IObject3d.h>


namespace imt3d
{


/**
	Definition of graphical point object in the 3D-space.
*/
class CPosition3d: virtual public IObject3d
{
public:
	static QByteArray GetTypeName();

	CPosition3d();
	CPosition3d(const i3d::CVector3d& center);

	const i3d::CVector3d& GetPosition() const;
	void SetPosition(const i3d::CVector3d& position);
	void Translate(const i3d::CVector3d& vector);
	CPosition3d GetTranslated(const i3d::CVector3d& vector) const;

	// reimplemented (imt3d::IObject3d)
	virtual bool IsEmpty() const override;
	virtual i3d::CVector3d GetCenter() const override;
	virtual void MoveCenterTo(const i3d::CVector3d& position) override;
	virtual CCuboid GetBoundingCuboid() const override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive);

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	i3d::CVector3d m_position;
};


// inline methods

inline const i3d::CVector3d& CPosition3d::GetPosition() const
{
	return m_position;
}


inline void CPosition3d::Translate(const i3d::CVector3d& vector)
{
	m_position += vector;
}


inline CPosition3d CPosition3d::GetTranslated(const i3d::CVector3d& vector) const
{
	CPosition3d translatedPosition = *this;

	translatedPosition.Translate(vector);

	return translatedPosition;
}


} // namespace imt3d


