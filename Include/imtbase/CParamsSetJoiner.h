// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IParamsInfoProvider.h>


namespace imtbase
{


/**
	Implementation of the multiple selection.
*/
class CParamsSetJoiner: virtual public iprm::IParamsSet
{
public:
	CParamsSetJoiner();
	CParamsSetJoiner(iprm::IParamsSet* paramsSet1, iprm::IParamsSet* paramsSet2);
	CParamsSetJoiner(const iprm::IParamsSet* paramsSet1, const iprm::IParamsSet* paramsSet2);

	void SetParams(iprm::IParamsSet* paramsSet1, iprm::IParamsSet* paramsSet2);
	void SetParams(const iprm::IParamsSet* paramsSet1, const iprm::IParamsSet* paramsSet2);

	// reimplemented (iprm::IParamsSet)
	virtual Ids GetParamIds(bool editableOnly = false) const override;
	virtual const iser::ISerializable* GetParameter(const QByteArray& id) const override;
	virtual iser::ISerializable* GetEditableParameter(const QByteArray& id) override;
	virtual const iprm::IParamsInfoProvider* GetParamsInfoProvider() const override;

	// reimplement (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	class ParamsInfoProviderJoiner: virtual public iprm::IParamsInfoProvider
	{
	public:
		ParamsInfoProviderJoiner(const CParamsSetJoiner* joiner);

		// reimplemented (iprm::IParamsInfoProvider)
		virtual std::unique_ptr<ParamInfo> GetParamInfo(const QByteArray& paramId) const override;

	private:
		const CParamsSetJoiner* m_joiner;
	};

	iprm::IParamsSet* m_paramsSet1;
	iprm::IParamsSet* m_paramsSet2;
	mutable ParamsInfoProviderJoiner m_infoProvider;
};


} // namespace imtbase


