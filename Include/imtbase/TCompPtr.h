#pragma once


// STD includes
#include <memory>

// ACF includes
#include <icomp/CComponentBase.h>
#include <icomp/CInterfaceManipBase.h>


namespace imtbase
{


/**
	\ingroup Main
*/
template <class InterfaceInterfaceType>
class TCompPtr: protected CInterfaceManipBase
{
public:
	InterfaceTypedef CInterfaceManipBase BaseClass;

	TCompPtr();
	
	explicit TCompPtr(icomp::IComponent* pointer);

	/**
		Check, whether the object is in valid state.
	*/
	bool IsValid() const;

	/**
		Get access to pointed object.
	*/
	const InterfaceType* GetPtr() const;

	/**
		Get access to pointed object.
	*/
	InterfaceType* GetPtr();

	/**
		Set this pointer to NULL.
	*/
	void Reset();

	template <class CastedInterfaceType>
	CastedInterfaceType Cast(const QByteArray& subComponentId = QByteArray()) const
	{
		CastedInterfaceType* castedPtr = BaseClass::ExtractInterface<CastedInterfaceType>(m_ptr.data(), subComponentId);

		return castedPtr;
	}

	InterfaceType& operator*() const;
	InterfaceType* operator->() const;
	bool operator==(const TCompPtr& other) const;

	void SetPtr(icomp::IComponent* pointer);

private:
	QSharedPointer<icomp::IComponent> m_ptr;
	InterfaceType* m_interfacePtr;
};


template <class InterfaceType>
inline TCompPtr<InterfaceType>::TCompPtr()
	:m_interfacePtr(NULL)
{
}


template <class InterfaceType>
inline TCompPtr<InterfaceType>::TCompPtr(icomp::IComponent* pointer)
{
	SetPtr(pointer);
}



// public methods

template<class InterfaceType>
inline bool TCompPtr<InterfaceType>::IsValid() const
{
	return m_impl.data() != NULL;
}


template<class InterfaceType>
inline const InterfaceType* TCompPtr<InterfaceType>::GetPtr() const
{
	return m_interfacePtr;
}


template<class InterfaceType>
inline InterfaceType* TCompPtr<InterfaceType>::GetPtr()
{
	return m_interfacePtr;
}


template<class InterfaceType>
inline void TCompPtr<InterfaceType>::Reset()
{
	m_impl.reset();

	m_interfacePtr = nullptr;
}



template<class InterfaceType>
inline InterfaceType & TCompPtr<InterfaceType>::operator*() const
{
	Q_ASSERT(m_interfacePtr != nullptr);

	return *m_interfacePtr;
}


template<class InterfaceType>
inline InterfaceType * TCompPtr<InterfaceType>::operator->() const
{
	return m_interfacePtr;
}


template <class InterfaceType>
inline bool TCompPtr<InterfaceType>::operator==(const TCompPtr<InterfaceType>& other) const
{
	return m_ptr == other.m_ptr;
}


template <class InterfaceType>
inline void TCompPtr<InterfaceType>::SetPtr(icomp::IComponent* pointer)
{
	InterfaceType* castedPtr = BaseClass::ExtractInterface<InterfaceType>(pointer);
	if (castedPtr != NULL){
		m_ptr.reset(pointer);

		m_interfacePtr = castedPtr;
	}
}


} // namespace imtbase


