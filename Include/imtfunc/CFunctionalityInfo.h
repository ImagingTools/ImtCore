#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QByteArrayList>

// ImtCore includes
#include <imtfunc/IFunctionalityInfo.h>


namespace imtfunc
{


/**
	@brief Implementation of IFunctionalityInfo
	
	Concrete implementation providing information about a controllable functionality.
	
	@see IFunctionalityInfo
	@ingroup imtfunc
*/
class CFunctionalityInfo: public IFunctionalityInfo
{
public:
	/**
		@brief Constructor
		
		@param id Unique functionality identifier
		@param name Human-readable name
		@param description Detailed description
		@param type Functionality type
		@param enabledByDefault Whether enabled by default
		@param required Whether functionality is required
		@param riskLevel Security risk level (0-3)
	*/
	CFunctionalityInfo(
		const QByteArray& id,
		const QString& name,
		const QString& description,
		FunctionalityType type,
		bool enabledByDefault = true,
		bool required = false,
		int riskLevel = 1);
	
	// IFunctionalityInfo interface
	QByteArray GetId() const override;
	QString GetName() const override;
	QString GetDescription() const override;
	FunctionalityType GetType() const override;
	bool IsEnabledByDefault() const override;
	bool IsRequired() const override;
	QByteArrayList GetDependencies() const override;
	int GetRiskLevel() const override;
	
	/**
		@brief Add a dependency
		
		Adds another functionality ID that this functionality depends on.
		
		@param dependencyId ID of required functionality
	*/
	void AddDependency(const QByteArray& dependencyId);
	
	/**
		@brief Set dependencies
		
		Sets the complete list of dependencies.
		
		@param dependencies List of required functionality IDs
	*/
	void SetDependencies(const QByteArrayList& dependencies);

private:
	QByteArray m_id;
	QString m_name;
	QString m_description;
	FunctionalityType m_type;
	bool m_enabledByDefault;
	bool m_required;
	int m_riskLevel;
	QByteArrayList m_dependencies;
};


} // namespace imtfunc


