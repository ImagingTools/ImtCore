#include <imtreport/CFont.h>


// ACF includes
#include "istd/TDelPtr.h"
#include "istd/CChangeNotifier.h"
#include "iser/IArchive.h"
#include "iser/CArchiveTag.h"


namespace imtreport
{


// public methods

CFont::CFont()
	:m_size(3.0),
	m_fontFlags(FontFlags::FF_NORMAL)
{
}


CFont::CFont(const QString& name, double size, int fontFlags)
	:m_name(name),
	m_size(size),
	m_fontFlags(fontFlags)
{
}


QString CFont::GetName() const
{
	return m_name;
}


void CFont::SetName(const QString& name)
{
	if (m_name != name)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


double CFont::GetSize() const
{
	return m_size;
}


void CFont::SetSize(double size)
{
	if (m_size != size)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_size = size;
	}
}


int CFont::GetFontFlags() const
{
	return m_fontFlags;
}


void CFont::SetFontFlags(int fontFlags)
{
	if (m_fontFlags != fontFlags)
	{
		istd::CChangeNotifier changeNotifier(this);

		m_fontFlags = fontFlags;
	}
}


// reimplemented (iser::ISerializable)

bool CFont::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? NULL : this);

	static iser::CArchiveTag nameTag("Name", "Font name", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag sizeTag("Size", "Font size", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sizeTag);
	retVal = retVal && archive.Process(m_size);
	retVal = retVal && archive.EndTag(sizeTag);

	static iser::CArchiveTag fontFlagsTag("FontFlags", "Font flags", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fontFlagsTag);
	retVal = retVal && archive.Process(m_fontFlags);
	retVal = retVal && archive.EndTag(fontFlagsTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CFont::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE;
}


bool CFont::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CFont* fontPtr = dynamic_cast<const CFont*>(&object);

	if (fontPtr != NULL)
	{
		istd::CChangeNotifier notifier(this);

		m_name = fontPtr->m_name;
		m_size = fontPtr->m_size;
		m_fontFlags = fontPtr->m_fontFlags;

		return true;
	}

	return false;
}


bool CFont::IsEqual(const IChangeable& object) const
{
	const CFont* fontPtr = dynamic_cast<const CFont*>(&object);
	if (fontPtr != NULL){
		return m_name == fontPtr->m_name &&
			m_size == fontPtr->m_size &&
			m_fontFlags == fontPtr->m_fontFlags;
	}

	return false;
}


istd::IChangeable* CFont::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CFont> clonePtr(new CFont());

	if (clonePtr->CopyFrom(*this, mode))
	{
		return clonePtr.PopPtr();
	}

	return NULL;
}


} // namespace imtreport


