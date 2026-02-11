// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


// forward declareation
namespace istd
{
class IChangeable;
}

namespace imtgql
{
class CGqlRequest;
}


namespace imtservergql
{

/**
	\brief The TCreateUpdateCollectionWrap class was created to simplify usage of \c CObjectCollectionControllerCompBase
	\note Programmers SHOULD NOT use this class. They SHOULD create a correct implementation of Base's methods
	\authors SZ, VS

	\param _CreateRepresentation - a representation, used to create an object
	\param _UpdateRequest - a request, MUST be able to extract representtion to update object's data, with type \c _CreateRepresentation
	\param _Base - a base class, that contains create/update methods
	\param _OID - an object ID, that should be created with \c _Base i.e.: by call \c _Base::CreateObject() method
	\warning \c _OID MUST be defined as static variable (CXX known limitation)

	\example of usage

	\code //C++
		static char s_MyClassCOTId[] = "";
		class MyClass: public TCreateUpdateCollectionWrap<MyRepresentation, MyUpdateRequest, BaseCollectionController, s_MyClassCOTId>
		{
			//
		protected:
			[[nodiscard]]virtual MyRepresentation GetRepresentationFromRequest(const MyUpdateRequest& request)const
			{
				return request->GetRepresentation();
			};
			[[nodiscard]]virtual bool SetObjectDataFromRepresentation(istd::IChangeable& object, const MyRepresentation& representation, QString& errorString) const
			{
				MyModelData* modelData = (MyModelData*)(object);
				if (modelData != nullptr){
					modelData->SetName(representation.GetName());

					return true;
				}

				return false;
			};
		}

	\endcode
*/

template<class _CreateRepresentation, class _UpdateRequest, class _Base, const char* _OID>
class TCreateUpdateCollectionWrap: public _Base
{
public:
	typedef _Base BaseClass;

protected:
	// abstract methods
	[[nodiscard]]virtual _CreateRepresentation GetRepresentationFromRequest(const _UpdateRequest& request)const = 0;
	[[nodiscard]]virtual bool SetObjectDataFromRepresentation(istd::IChangeable& object, const _CreateRepresentation& representation, QString& errorMessage) const = 0;

	// reimplemented (_Base)
	istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const _CreateRepresentation& representation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;

	bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const _UpdateRequest& updateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
};


// protected methods

// reimplemented (_Base)
template<class _CreateRepresentation, class _UpdateRequest, class _Base, const char* _OID>
istd::IChangeableUniquePtr TCreateUpdateCollectionWrap<_CreateRepresentation, _UpdateRequest, _Base, _OID>::CreateObjectFromRepresentation(
	const _CreateRepresentation& representation,
	QByteArray& /*newObjectId*/,
	QString& errorMessage) const
{
	std::unique_ptr<istd::IChangeable> objetPtr(_Base::CreateObject(QByteArray(_OID)));
	if (objetPtr){
		const bool isUpdated = SetObjectDataFromRepresentation(*objetPtr, representation, errorMessage);
		if (isUpdated){
			return objetPtr.release();
		}
	}

	return nullptr;
}


template<class _CreateRepresentation, class _UpdateRequest, class _Base, const char* _OID>
bool TCreateUpdateCollectionWrap<_CreateRepresentation, _UpdateRequest, _Base, _OID>::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			const _UpdateRequest& updateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	_CreateRepresentation repreesentation = GetRepresentationFromRequest(updateRequest);

	return SetObjectDataFromRepresentation(object, repreesentation, errorMessage);
}


} // namespac imtservergql
