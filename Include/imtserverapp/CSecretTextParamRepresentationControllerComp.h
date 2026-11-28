// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/CPasswordParamRepresentationControllerComp.h>


namespace imtserverapp
{


/**
	Representation of a secret text parameter (imtcrypt::ISecretTextParam) as a password field.

	The secret never leaves the server: a set value is represented by a placeholder. When the
	placeholder comes back unchanged, the stored value is kept; an empty text removes it; any
	other text replaces it.
*/
class CSecretTextParamRepresentationControllerComp: public CPasswordParamRepresentationControllerComp
{
public:
	typedef CPasswordParamRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CSecretTextParamRepresentationControllerComp)
		I_REGISTER_INTERFACE(IJsonRepresentationController);
	I_END_COMPONENT;

	/**
		Text that stands for a stored secret.
	*/
	static const QString s_placeholder;

protected:
	// reimplemented (imtserverapp::CTextParamRepresentationControllerComp)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::V1_0::imtbase::CTextParam& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::V1_0::imtbase::CTextParam& sdlRepresentation) const override;
};


} // namespace imtserverapp


