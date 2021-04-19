#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TModelWrap.h>
#include <iprm/CSelectionParam.h>

// ImtCore includes
#include <imtwidgets/CImtStyle.h>


namespace imtgui
{


class CDesignManagerCompBase:
			public icomp::CComponentBase,
			public iprm::CSelectionParam
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef iprm::CSelectionParam BaseClass2;

	I_BEGIN_BASE_COMPONENT(CDesignManagerCompBase);
		I_REGISTER_INTERFACE(iprm::ISelectionParam);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_slaveCompPtr, "SlaveDesignManager", "Slave design manager", false, "");
	I_END_COMPONENT;

protected:
	typedef int(*ResourceFunctionPtr)();

	virtual void RegisterResourcesFunctions() = 0;

	void SetDesignResourcesFunctions(
				imtwidgets::CImtStyle::DesignSchema designSchema,
				ResourceFunctionPtr initResources,
				ResourceFunctionPtr cleanupResources);

	// reimplemented (iprm::ISelectionParam)
	virtual bool SetSelectedOptionIndex(int index) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	class Constraints: virtual public iprm::IOptionsList
	{
	public:
		void Update();

		// reimplemented (iprm::IOptionsList)
		virtual int GetOptionsFlags() const;
		virtual int GetOptionsCount() const;
		virtual QString GetOptionName(int index) const;
		virtual QString GetOptionDescription(int index) const;
		virtual QByteArray GetOptionId(int index) const;
		virtual bool IsOptionEnabled(int index) const;

	private:
		struct Option
		{
			QByteArray id;
			QString name;
			QString description;
		};

	private:
		QList<Option> m_options;
	};

private:
	I_REF(iprm::ISelectionParam, m_slaveCompPtr);

	QMap<imtwidgets::CImtStyle::DesignSchema, ResourceFunctionPtr> m_initResources;
	QMap<imtwidgets::CImtStyle::DesignSchema, ResourceFunctionPtr> m_cleanupResources;

	imod::TModelWrap<Constraints> m_constraints;
};


} // namespace imtgui


