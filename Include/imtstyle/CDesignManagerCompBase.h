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


class CDesignManagerCompBase:
			public ibase::TRuntimeStatusHanderCompWrap<icomp::CComponentBase>,
			public ibase::TLocalizableWrap<iprm::CSelectionParam>
{
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<icomp::CComponentBase> BaseClass;
	typedef ibase::TLocalizableWrap<iprm::CSelectionParam> BaseClass2;

	I_BEGIN_COMPONENT(CDesignManagerCompBase);
		I_REGISTER_INTERFACE(iprm::ISelectionParam);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_slaveCompPtr, "SlaveDesignManager", "Slave design manager", false, "SlaveDesignManager");
		I_ASSIGN(m_paletteProviderCompPtr, "PaletteProvider", "Palette provider", false, "PaletteProvider");
		I_ASSIGN_TO(m_fontProviderCompPtr, m_paletteProviderCompPtr, false);
	I_END_COMPONENT;

	CDesignManagerCompBase();
	~CDesignManagerCompBase();

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

private:
	class DesignList: virtual public iprm::IOptionsList
	{
	public:
		DesignList();

		void SetParent(CDesignManagerCompBase* parentPtr);

		void UpdateDesignList();

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const;
		virtual int GetOptionsCount() const;
		virtual QString GetOptionName(int index) const;
		virtual QString GetOptionDescription(int index) const;
		virtual QByteArray GetOptionId(int index) const;
		virtual bool IsOptionEnabled(int index) const;

	private:
		struct DesignInfo
		{
			QByteArray id;
			QString name;
			QString description;
		};

	private:
		QList<DesignInfo> m_designList;
		CDesignManagerCompBase* m_parentPtr;
	};

private:
	I_REF(iprm::ISelectionParam, m_slaveCompPtr);
	I_REF(imtstyle::IColorPaletteProvider, m_paletteProviderCompPtr);
	I_REF(imtstyle::IFontProvider, m_fontProviderCompPtr);

	imod::TModelWrap<DesignList> m_designs;
};


} // namespace imtstyle


