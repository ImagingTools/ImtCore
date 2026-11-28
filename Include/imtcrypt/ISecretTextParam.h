// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iprm/ITextParam.h>


namespace imtcrypt
{


/**
	Text parameter holding a secret, e.g. an API key.
	Its value is never shown or sent to a client; representations only tell whether a value is set.
	\see imtserverapp::CSecretTextParamRepresentationControllerComp
*/
class ISecretTextParam: virtual public iprm::ITextParam
{
};


} // namespace imtcrypt


