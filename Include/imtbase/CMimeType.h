// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IMimeType.h>


namespace imtbase
{


class CMimeType : virtual public IMimeType
{
public:
	// reimplemented (IMimeType)
	virtual const QString& GetType() const override;
	virtual bool SetType(const QString& type) override;
	virtual QStringList GetTree() const override;
	virtual bool SetTree(const QStringList& tree) override;
	virtual const QString& GetSubType() const override;
	virtual bool SetSubType(const QString& subType) override;
	virtual const QString& GetSuffix() const override;
	virtual bool SetSuffix(const QString& suffix) override;
	virtual const Parameters& GetParameters() const override;
	virtual bool SetParameters(const Parameters& parameters) override;
	virtual QString ToString() override;
	virtual bool FromString(const QString& string) override;

protected:
	bool isTypeValid(const QString& type);
	bool isTreeValid(const QStringList& tree);
	bool isSubTypeValid(const QString& subType);
	bool isSuffixValid(const QString& suffix);
	bool isParametersValid(const Parameters& parameters);

protected:
	QString m_type;
	QStringList m_tree;
	QString m_subType;
	QString m_suffix;
	Parameters m_parameters;
};


} // namespace imtbase


