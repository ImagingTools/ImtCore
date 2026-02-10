// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtserverapp/TJsonRepresentationControllerWrap.h>


namespace imtserverapp
{


template<class SdlClass>
class TJsonRepresentationControllerCompWrap:
			public ilog::CLoggerComponentBase,
			public TJsonRepresentationControllerWrap<SdlClass>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef TJsonRepresentationControllerWrap<SdlClass> BaseClass2;
	
	I_BEGIN_BASE_COMPONENT(TJsonRepresentationControllerCompWrap);
		I_REGISTER_INTERFACE(IJsonRepresentationController);
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "Model-ID", false, "");
		I_ASSIGN(m_nameAttrPtr, "Name", "Name", false, "");
		I_ASSIGN(m_descriptionAttrPtr, "Description", "Description", false, "");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_ATTR(QByteArray, m_modelIdAttrPtr);
	I_TEXTATTR(m_nameAttrPtr);
	I_TEXTATTR(m_descriptionAttrPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


// reimplemented (icomp::CComponentBase)
template<class SdlClass>
void TJsonRepresentationControllerCompWrap<SdlClass>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_modelIdAttrPtr.IsValid()){
		this->m_modelId = *m_modelIdAttrPtr;
	}

	if (m_nameAttrPtr.IsValid()){
		this->m_name = *m_nameAttrPtr;
	}

	if (m_descriptionAttrPtr.IsValid()){
		this->m_description = *m_descriptionAttrPtr;
	}

	if (m_translationManagerCompPtr.IsValid()){
		this->m_translationManagerPtr = m_translationManagerCompPtr.GetPtr();
	}
}


} // namespace imtserverapp


