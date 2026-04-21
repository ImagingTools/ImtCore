// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtqml/IDataModelBridge.h>


namespace imtqml
{


/**
	\brief Base \c icomp component implementation of
	\c IDataModelBridge intended to be inherited by per-model
	bridges.

	\details
	\c CDataModelBridgeBase wires the boilerplate that every
	per-model bridge needs:

	- it is a regular \c icomp::CComponentBase that registers the
	  \c IDataModelBridge interface, so it can be plugged into
	  \c CDataModelBridgeDemultiplexer through the \c ModelDelegates
	  multi-ref slot;
	- it owns a \c QString \c ModelId attribute identifying the
	  single \c modelId served by the concrete subclass;
	- it implements \c IsSupported by comparing the requested
	  \c modelId against the \c ModelId attribute, so the
	  demultiplexer can route requests to the right delegate
	  without the subclass having to override \c IsSupported.

	Concrete subclasses provide the actual fetch / store logic by
	overriding \c GetModel and \c SetModel (still abstract on
	\c IDataModelBridge); they may use \c GetModelId() to read the
	configured identifier when constructing transport-specific
	requests.

	\note Bridge is intentionally NOT exposed to QML — clients
	(\c CDataModelController) resolve it through icomp.
*/
class CDataModelBridgeBase:
			public icomp::CComponentBase,
			virtual public IDataModelBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDataModelBridgeBase);
		I_REGISTER_INTERFACE(IDataModelBridge);
		I_ASSIGN(m_modelIdAttrPtr, "ModelId",
				"Identifier of the data model served by this bridge", false, "");
	I_END_COMPONENT;

	CDataModelBridgeBase();
	~CDataModelBridgeBase() override;

	/**
		\brief Returns the \c ModelId attribute value configured for
		this bridge, or an empty string if the attribute was not
		set.
	*/
	QString GetModelId() const;

	// reimplemented (IDataModelBridge)
	virtual bool IsSupported(const QString& modelId) const override;

private:
	I_ATTR(QString, m_modelIdAttrPtr);
};


} // namespace imtqml
