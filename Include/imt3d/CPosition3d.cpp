#include <imt3d/CPosition3d.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CClassInfo.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imt3d
{


// public static methods


QByteArray CPosition3d::GetTypeName()
{
	return istd::CClassInfo::GetName<CPosition3d>();
}


// public methods

CPosition3d::CPosition3d()
	:m_position(0, 0, 0)
{
}


CPosition3d::CPosition3d(const i3d::CVector3d& position)
:	m_position(position)
{
}


void CPosition3d::SetPosition(const i3d::CVector3d& position)
{
	if (position != m_position){
		istd::CChangeNotifier changeNotifier(this);
		Q_UNUSED(changeNotifier);

		m_position = position;
	}
}


// reimplemented (imt3d::IObject3d)

bool CPosition3d::IsEmpty() const
{
	return false;
}


i3d::CVector3d CPosition3d::GetCenter() const
{
	return m_position;
}


void CPosition3d::MoveCenterTo(const i3d::CVector3d& position)
{
	SetPosition(position);
}


CCuboid CPosition3d::GetBoundingCuboid() const
{
	return CCuboid(
				m_position.GetX() - I_BIG_EPSILON,
				m_position.GetX() + I_BIG_EPSILON,
				m_position.GetY() - I_BIG_EPSILON,
				m_position.GetY() + I_BIG_EPSILON,
				m_position.GetZ() - I_BIG_EPSILON,
				m_position.GetZ() + I_BIG_EPSILON);
}


// reimplemented (iser::IObject)

QByteArray CPosition3d::GetFactoryId() const
{

	return GetTypeName();
}




// reimplemented (iser::ISerializable)

bool CPosition3d::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? NULL : this, &GetAllChanges());
	Q_UNUSED(changeNotifier);

	return m_position.Serialize(archive);
}


// reimplemented (istd::IChangeable)

int CPosition3d::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CPosition3d::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CPosition3d* positionPtr = dynamic_cast<const CPosition3d*>(&object);

	if (positionPtr != NULL){
		istd::CChangeNotifier changeNotifier(this);
		Q_UNUSED(changeNotifier);

		SetPosition(positionPtr->GetPosition());

		return true;
	}

	return false;
}


bool CPosition3d::IsEqual(const istd::IChangeable& object) const
{
	const CPosition3d* positionPtr = dynamic_cast<const CPosition3d*>(&object);
	if (positionPtr != NULL){
		return (m_position == positionPtr->m_position);
	}

	return false;
}


istd::IChangeable* CPosition3d::CloneMe(CompatibilityMode mode) const 
{
	istd::TDelPtr<CPosition3d> clonePtr(new CPosition3d);

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return NULL;
}


bool CPosition3d::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_position.Reset();

	return true;
}

} // namespace imt3d


