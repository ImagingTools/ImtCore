// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/TLocalizableWrap.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <icomp/CComponentBase.h>
#include <imod/TModelWrap.h>
#include <iprm/CSelectionParam.h>

// ImtCore includes
#include <imtstyle/CImtStyle.h>
#include <imtstyle/IColorPaletteProvider.h>
#include <imtstyle/IFontProvider.h>


namespace imtstyle
{


class CDesignManagerComp:
			public ibase::TRuntimeStatusHanderCompWrap<icomp::CComponentBase>,
			public ibase::TLocalizableWrap<iprm::CSelectionParam>
{
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<icomp::CComponentBase> BaseClass;
	typedef ibase::TLocalizableWrap<iprm::CSelectionParam> BaseClass2;

	I_BEGIN_COMPONENT(CDesignManagerComp);
		I_REGISTER_INTERFACE(iprm::ISelectionParam);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_SUBELEMENT(DesignList);
		I_REGISTER_SUBELEMENT_INTERFACE(DesignList, iprm::IOptionsList, ExtractDesignList);
		I_REGISTER_SUBELEMENT_INTERFACE(DesignList, imod::IModel, ExtractDesignList);
		I_REGISTER_SUBELEMENT_INTERFACE(DesignList, istd::IChangeable, ExtractDesignList);
		I_ASSIGN(m_paletteProviderCompPtr, "PaletteProvider", "Palette provider", false, "PaletteProvider");
		I_ASSIGN_TO(m_fontProviderCompPtr, m_paletteProviderCompPtr, false);
		I_ASSIGN(m_defaultThemeIndexAttrPtr, "DefaultThemeIndex", "Index of the default theme", true, -1);
	I_END_COMPONENT;

	CDesignManagerComp();
	~CDesignManagerComp();

	// reimplemented (iprm::ISelectionParam)
	virtual bool SetSelectedOptionIndex(int index) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemStarting() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;

private:
	bool ApplyDesignScheme(const QByteArray& themeId);

	// static template methods for subelement access
	template <class InterfaceType>
	static InterfaceType* ExtractDesignList(CDesignManagerComp& component)
	{
		return &component.m_designs;
	}

private:
	class DesignList: virtual public iprm::IOptionsList
	{
	public:
		DesignList();

		void SetParent(CDesignManagerComp* parentPtr);

		void UpdateDesignList();

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const override;
		virtual int GetOptionsCount() const override;
		virtual QString GetOptionName(int index) const override;
		virtual QString GetOptionDescription(int index) const override;
		virtual QByteArray GetOptionId(int index) const override;
		virtual bool IsOptionEnabled(int index) const override;

	private:
		struct DesignInfo
		{
			QByteArray id;
			QString name;
			QString description;
		};

	private:
		QList<DesignInfo> m_designList;
		CDesignManagerComp* m_parentPtr;
	};

private:
	I_REF(iprm::ISelectionParam, m_slaveCompPtr);
	I_REF(imtstyle::IColorPaletteProvider, m_paletteProviderCompPtr);
	I_REF(imtstyle::IFontProvider, m_fontProviderCompPtr);
	I_ATTR(int, m_defaultThemeIndexAttrPtr);

	imod::TModelWrap<DesignList> m_designs;
};


} // namespace imtstyle


