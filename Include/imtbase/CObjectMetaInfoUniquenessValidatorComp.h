#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtBase includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectMetaInfoUniquenessValidator.h>


namespace imtbase
{

class CObjectMetaInfoUniquenessValidatorComp:
			public icomp::CComponentBase,
			virtual public imtbase::IObjectMetaInfoUniquenessValidator
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectMetaInfoUniquenessValidatorComp);
		I_REGISTER_INTERFACE(IObjectMetaInfoUniquenessValidator);
		I_ASSIGN_MULTI_0(m_collectionCompPtr, "Collections", "Collections for reading metainfo data values of objects", false);
		I_ASSIGN_MULTI_0(m_metaInfoTypeAttrPtr, "MetaInfoTypes", "Metainfo types for corresponding collections", false);
	I_END_COMPONENT;

	// reimplemented (imtbase::IMetaInfoUniquenessChecker)
	virtual int IsUniqueValue(const imtbase::IObjectCollection* collectionPtr, const QByteArray& objectId, const QVariant& value) const override;

private:
	int GetCollectionCount() const;

private:
	I_MULTIREF(IObjectCollection, m_collectionCompPtr);
	I_MULTIATTR(int, m_metaInfoTypeAttrPtr);
};


} // namespace imtbase


