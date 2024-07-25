#pragma once


// ACF includes
#include <istd/CChangeGroup.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtbase/CIdentifiable.h>


namespace imtbase
{


template<class Base>
class TIdentifiableWrap: public Base, public CIdentifiable
{
public:
	typedef CIdentifiable BaseClass;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
};


// public methods

// reimplemented (iser::ISerializable)

template<class Base>
bool TIdentifiableWrap<Base>::Serialize(iser::IArchive& archive)
{
	bool retVal = retVal && Base::Serialize(archive);
	retVal = retVal && BaseClass::Serialize(archive);

	return retVal;
}


// reimplemented (IChangeable)

template<class Base>
int TIdentifiableWrap<Base>::GetSupportedOperations() const
{
	return Base::GetSupportedOperations();
}


template<class Base>
bool TIdentifiableWrap<Base>::CopyFrom(const istd::IChangeable& object,  CompatibilityMode mode)
{
	istd::CChangeGroup changeGroup(this);

	bool retVal = Base::CopyFrom(object, mode);
	retVal = retVal && BaseClass::CopyFrom(object, mode);

	return retVal;
}


template<class Base>
bool TIdentifiableWrap<Base>::IsEqual(const istd::IChangeable& object) const
{
	bool retVal = Base::IsEqual(object);
	retVal = retVal && BaseClass::IsEqual(object);

	return retVal;
}


template<class Base>
istd::IChangeable* TIdentifiableWrap<Base>::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<TIdentifiableWrap<Base>> clonePtr(new TIdentifiableWrap<Base>());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


template<class Base>
bool TIdentifiableWrap<Base>::ResetData(CompatibilityMode mode)
{
	istd::CChangeGroup changeGroup(this);

	bool retVal = Base::ResetData(mode);
	retVal = retVal && BaseClass::ResetData(mode);

	return retVal;
}


} // namespace imtbase


