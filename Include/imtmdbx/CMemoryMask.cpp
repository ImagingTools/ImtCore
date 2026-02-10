// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/CMemoryMask.h>

// Qt includes
#include <QtCore/QDebug>


namespace imtmdbx
{


CMemoryMask::CMemoryMask()
{

}


bool CMemoryMask::GetUnit(quint64 position)
{
    quint64 offset = position / 64;
    quint64 bitPosition = position % 64;

    quint64 item;

    if (!m_cache.contains(offset)){
        item = 0;
    }
    else {
        item = m_cache.value(offset);
    }

    return (item >> bitPosition) & (quint64)1;
}


bool CMemoryMask::SetUnit(quint64 position, bool unit)
{
    quint64 offset = position / 64;
    quint64 bitPosition = position % 64;

    quint64 item;

    if (!m_cache.contains(offset)){
        item = 0;
    }
    else {
        item = m_cache.value(offset);
    }

    if(unit){
        item = item | ((quint64)1 << bitPosition);
    }
    else {
        item = item & ~((quint64)1 << bitPosition);
    }

    //item = (item & ~((quint64)1 << bitPosition)) | ((quint64)unit << bitPosition);

    SetItem(offset, item);

    return true;
}


bool CMemoryMask::GetItem(quint64 offset, quint64& item)
{
    if (!m_cache.contains(offset)){
        item = 0;
    }
    else {
        item = m_cache.value(offset);
    }

    return true;
}


bool CMemoryMask::SetItem(quint64 offset, quint64 item)
{
    m_cache.insert(offset, item);

    return true;
}


bool CMemoryMask::GetNearestOffset(quint64& offset, quint64 startOffset)
{
    if(m_cache.isEmpty()){
        return false;
    }

    QList<quint64> mapKeys = m_cache.keys();

    quint64 biggestKey = mapKeys.at(mapKeys.length() -1);
    if(startOffset > biggestKey){
        return false;
    }

    for(int i = 0; i < mapKeys.length(); i++){
        quint64 key = mapKeys.at(i);
        if(key >= startOffset){
            offset = key;
            return true;
        }
    }

    return false;
}


bool CMemoryMask::GetNextItemOffset(quint64& offset, quint64 startOffset)
{
    if(m_cache.isEmpty()){
        return false;
    }

    QList<quint64> mapKeys = m_cache.keys();

    quint64 biggestKey = mapKeys.at(mapKeys.length() -1);
    if(startOffset > biggestKey){
        return false;
    }

    for(int i = 0; i < mapKeys.length(); i++){
        quint64 key = mapKeys.at(i);
        if(key > startOffset){
            offset = key;
            return true;
        }
    }
    return false;
}


bool CMemoryMask::GetPreviosItemOffset(quint64& offset, quint64 startOffset)
{
    if(m_cache.isEmpty()){
        return false;
    }

    QList<quint64> mapKeys = m_cache.keys();

    quint64 smallestKey = mapKeys.at(0);
    if(startOffset < smallestKey){
        return false;
    }

    for(int i = mapKeys.length() -1; i >= 0; i--){
        quint64 key = mapKeys.at(i);
        if(key < startOffset){
            offset = key;
            return true;
        }
    }
    return false;
}


}//namespace imtmdbx
