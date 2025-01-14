#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


class CUserRepresentationController: public istd::IPolymorphic
{
public:
	virtual bool FillUserInfoFromRepresentation(
				const sdl::imtauth::Users::CUserData::V1_0& representation,
				istd::IChangeable& userObject,
				imtbase::IObjectCollection& userCollection,
				QByteArray& userId,
				QString& errorMessage) const;
};


} // namespace imtauth


