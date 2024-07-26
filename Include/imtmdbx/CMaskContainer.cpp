#include <imtmdbx/CMaskContainer.h>


namespace imtmdbx
{


static int BitsSetTable256[256];
static bool isInit = false;

int BitCount(quint64 n)
{

	return (BitsSetTable256[n & 0xff] +
			BitsSetTable256[(n >> 8) & 0xff] +
			BitsSetTable256[(n >> 16) & 0xff] +
			BitsSetTable256[(n >> 24) & 0xff] +
			BitsSetTable256[(n >> 32) & 0xff] +
			BitsSetTable256[(n >> 40) & 0xff] +
			BitsSetTable256[(n >> 48) & 0xff] +
			BitsSetTable256[n >> 56]);

}

void Initialize()
{

	// To initially generate the
	// table algorithmically
	BitsSetTable256[0] = 0;
	for (int i = 0; i < 256; i++)
	{
		BitsSetTable256[i] = (i & 1) +
				BitsSetTable256[i / 2];
	}
}


CMaskContainer::CMaskContainer(OperationType operationType):
	m_operationType(operationType)
{
	if (!isInit){
		Initialize();
	}
}


void CMaskContainer::AddMask(IMask *mask, bool isInversion)
{
	if(!isInversion){
		m_maskList.append(mask);
	}
	else {
		m_maskListInv.append(mask);
	}
}


quint64 CMaskContainer::GetUnitCount()
{
	std::cout << "GetUnitCount";
	qint64 activeOffset = -1;
	quint64 unitCount = 0;

	int whileCount = 0;
	while(1){
		std::cout << whileCount;
		qint64 maxOffset = activeOffset;
		quint64 activeItem = 0xffffffffffffffff;
		for(int i = 0; i < m_maskList.length(); i++){
			quint64 offset = 0;
			if(!m_maskList.at(i)->GetNextItemOffset(offset, activeOffset)){
				return unitCount;
			}
			qint64 offset_ = offset;
			if(offset_ > maxOffset){
				maxOffset = offset;
			}
		}
		for(int i = 0; i < m_maskListInv.length(); i++){
			quint64 offset = 0;
			if(!m_maskListInv.at(i)->GetNextItemOffset(offset, activeOffset)){
				return unitCount;
			}
			qint64 offset_ = offset;
			if(offset_ > maxOffset){
				maxOffset = offset;
			}
		}

		activeOffset = maxOffset;

		for(int i = 0; i < m_maskList.length(); i++){
			quint64 item;
			if(activeOffset >= 0){
				if (!m_maskList.at(i)->GetItem(activeOffset, item)){
					item = 0;
				}
			}
			else {
				item = 0;
			}

			if(m_operationType == imtmdbx::CMaskContainer::OT_AND){
				activeItem = activeItem & item;
			}
			else if(m_operationType == imtmdbx::CMaskContainer::OT_OR){
				activeItem = activeItem | item;
			}


		}
		for(int i = 0; i < m_maskListInv.length(); i++){

			quint64 item;
			if(activeOffset >= 0){
				if (!m_maskListInv.at(i)->GetItem(activeOffset, item)){
					item = 0;
				}
			}
			else {
				item = 0;
			}


			if(m_operationType == imtmdbx::CMaskContainer::OT_AND){
				activeItem = activeItem & ~item;//Inversion
			}
			else if(m_operationType == imtmdbx::CMaskContainer::OT_OR){
				activeItem = activeItem | ~item;//Inversion
			}
		}

		unitCount += BitCount(activeItem);

		whileCount++;
		if(whileCount > 1000){
			break;
		}
	}

	return unitCount;
}


QList<quint64> CMaskContainer::GetUnitPositions(quint64 offset, int limit)
{
	QList<quint64> list;
	qint64 activeOffset = -1;
	quint64 unitCount = 0;

	while(1){
		qint64 maxOffset = activeOffset;
		quint64 activeItem = 0xffffffffffffffff;
		for(int i = 0; i < m_maskList.length(); i++){
			quint64 offset = 0;
			if(!m_maskList.at(i)->GetNextItemOffset(offset, activeOffset)){
				return list;
			}
			qint64 offset_ = offset;
			if(offset_ > maxOffset){
				maxOffset = offset;
			}
		}
		for(int i = 0; i < m_maskListInv.length(); i++){
			quint64 offset = 0;
			if(!m_maskListInv.at(i)->GetNextItemOffset(offset, activeOffset)){
				return list;
			}
			qint64 offset_ = offset;
			if(offset_ > maxOffset){
				maxOffset = offset;
			}
		}
		activeOffset = maxOffset;
		for(int i = 0; i < m_maskList.length(); i++){

			quint64 item;
			if(activeOffset >= 0){
				if (!m_maskList.at(i)->GetItem(activeOffset, item)){
					item = 0;
				}
			}
			else {
				item = 0;
			}
			if(m_operationType == imtmdbx::CMaskContainer::OT_AND){
				activeItem = activeItem & item;
			}
			else if(m_operationType == imtmdbx::CMaskContainer::OT_OR){
				activeItem = activeItem | item;
			}
		}
		for(int i = 0; i < m_maskListInv.length(); i++){
			quint64 item;
			if(activeOffset >= 0){
				if (!m_maskListInv.at(i)->GetItem(activeOffset, item)){
					item = 0;
				}
			}
			else {
				item = 0;
			}
			if(m_operationType == imtmdbx::CMaskContainer::OT_AND){
				activeItem = activeItem & ~item;//Inversion
			}
			else if(m_operationType == imtmdbx::CMaskContainer::OT_OR){
				activeItem = activeItem | ~item;//Inversion
			}
		}

		for(int i = 0; i < 64; i++){
			bool ok = (activeItem >> i) & (quint64)1;//check activeItem
			if(ok){
				unitCount++;
				if (unitCount > offset){
					int position = activeOffset * 64 + i;
					list.append(position);
					if(list.length() >= limit){
						return list;
					}
				}
			}
		}
	}

    return list;
}



}//namespace imtmdbx
