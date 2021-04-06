#pragma once


// ACF includes
#include <iprm/IParamsSet.h>


namespace imtbase
{


/**
	Implementation of the multiple selection.
*/
class CParamsSetJoiner: virtual public iprm::IParamsSet
{
public:
	CParamsSetJoiner(iprm::IParamsSet* paramsSet1, iprm::IParamsSet* paramsSet2);
	CParamsSetJoiner(const iprm::IParamsSet* paramsSet1, const iprm::IParamsSet* paramsSet2);

	// reimplemented (iprm::IParamsSet)
	virtual Ids GetParamIds(bool editableOnly = false) const override;
	virtual const iser::ISerializable * GetParameter(const QByteArray& id) const override;
	virtual iser::ISerializable * GetEditableParameter(const QByteArray& id) override;

	// reimplement (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	iprm::IParamsSet* m_paramsSet1;
	iprm::IParamsSet* m_paramsSet2;
};


} // namespace imtbase


