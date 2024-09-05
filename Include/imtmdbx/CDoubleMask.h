#pragma once

// ImtCore includes
#include <imtmdbx/CMask.h>


namespace imtmdbx
{


class CDoubleMask: public CMask
{
public:
	CDoubleMask(const QString& name,
				mdbx::txn_managed& txn,
				quint32 externKey = 0,
				mdbx::key_mode keyMode = mdbx::key_mode::ordinal,
				mdbx::value_mode valueMode = mdbx::value_mode::single,
				bool hasIndex = false);

protected:
	virtual bool GetNearestOffset(quint64& offset, quint64 startOffset = 0) override;
	virtual bool GetNextItemOffset(quint64& offset, quint64 startOffset = -1) override;
	virtual bool GetPreviosItemOffset(quint64& offset, quint64 startOffset = 0xffffffff) override;

public:
	virtual bool GetItem(quint64 offset, quint64& item) override;
	virtual bool SetItem(quint64 offset, quint64 item) override;
	bool SetExternalKey(quint64 externalKey);

private:
	quint64 m_externKey;

};

}//namespace imtmdbx
