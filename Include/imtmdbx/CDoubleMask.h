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
				mdbx::key_mode keyMode = mdbx::key_mode::ordinal,
				mdbx::value_mode valueMode = mdbx::value_mode::single,
				bool hasIndex = false,
				quint32 externKey = 0);

	virtual bool GetUnit(quint64 position) override;
	virtual bool SetUnit(quint64 position, bool unit) override;


protected:
	virtual bool GetItem(quint64 offset, quint64& item) override;
	virtual bool SetItem(quint64 offset, quint64 item) override;
	virtual bool GetNextItemOffset(quint64& offset, qint64 startOffset = -1) override;
	virtual bool GetPreviosItemOffset(quint64& offset, quint64 startOffset = 0xffffffff) override;

private:
	quint64 m_externKey;

};

}//namespace imtmdbx
