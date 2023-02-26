#pragma once


// ACF includes
#include <iser/IObject.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtbase/IIdentifiable.h>

namespace imtbase
{


/**
	Interface for getting information about a identifier.
*/
template<class Base>
class TIdentifiableWrap: public Base, virtual public IIdentifiable
{
public:
	bool SetObjectUuid(const QByteArray& identifier);

	// reimplemented (IIdentifiable)
	virtual QByteArray GetObjectUuid() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual bool IsIdentifierAccepted(const QByteArray& identifier) const;
	virtual bool IsMutable() const;

private:
	QByteArray m_identifier;
};


// public methods

template<class Base>
bool TIdentifiableWrap<Base>::SetObjectUuid(const QByteArray& identifier)
{
	Q_ASSERT(!identifier.isEmpty());
	if(identifier.isEmpty()){
		return false;
	}

	if(!IsIdentifierAccepted(identifier)){
		return false;
	}

	if(!IsMutable()){
		return false;
	}

	if(m_identifier != identifier){
		istd::CChangeNotifier changeNotifier(this);
		m_identifier = identifier;
	}

	return true;
}


// reimplemented (IIdentifiable)

template<class Base>
QByteArray TIdentifiableWrap<Base>:: GetObjectUuid() const
{
	return m_identifier;
}


// reimplemented (iser::ISerializable)

template<class Base>
bool TIdentifiableWrap<Base>::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 identifiableVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, identifiableVersion)){
		identifiableVersion = 0;
	}
	if (identifiableVersion >= 5902){
		static iser::CArchiveTag identifierTag("Uuid", "Unique identifier of the object", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(identifierTag);
		retVal = retVal && archive.Process(m_identifier);
		retVal = retVal && archive.EndTag(identifierTag);
	}
	retVal = retVal && Base::Serialize(archive);

	return retVal;
}


// reimplemented (IChangeable)

template<class Base>
int TIdentifiableWrap<Base>::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}

template<class Base>
bool TIdentifiableWrap<Base>::CopyFrom(const istd::IChangeable& object,  CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const TIdentifiableWrap<Base>* sourcePtr = dynamic_cast<const TIdentifiableWrap<Base>*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_identifier = sourcePtr->m_identifier;

		bool retVal = Base::CopyFrom(object);

		return retVal;
	}

	return false;
}


template<class Base>
bool TIdentifiableWrap<Base>::IsEqual(const istd::IChangeable& object) const
{
	const TIdentifiableWrap<Base>* sourcePtr = dynamic_cast<const TIdentifiableWrap<Base>*>(&object);
	if (sourcePtr != nullptr){
		return (m_identifier == sourcePtr->m_identifier);
	}

	return false;
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
bool TIdentifiableWrap<Base>::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_identifier.clear();
	Base::ResetData();

	return true;
}


// protected methods

template<class Base>
bool TIdentifiableWrap<Base>::IsIdentifierAccepted(const QByteArray& identifier) const
{
	if(identifier.isEmpty()){
		return false;
	}

	return true;
}


template<class Base>
bool TIdentifiableWrap<Base>::IsMutable() const
{
	return true;
}


typedef TIdentifiableWrap<IIdentifiable> CIdentifiable;


} // namespace imtbase


