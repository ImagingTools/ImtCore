#pragma once


// ACF includes
#include <imtbase/IObjectCollection.h>


namespace imthype
{


class ITaskSettings: virtual public istd::IChangeable
{
public:
	virtual QString GetTaskName() const = 0;
	virtual void SetTaskName(const QString& taskName) = 0;
	virtual QString GetTaskDescription() const = 0;
	virtual void SetTaskDescription(const QString& taskDescription) = 0;
	virtual bool GetTaskEnabled() const = 0;
	virtual void SetTaskEnabled(bool isEnabled) = 0;
	virtual QByteArray GetUserTaskId() const = 0;
	virtual void SetUserTaskId(const QByteArray& userTaskId) = 0;
	virtual QByteArray GetTaskInputId() const = 0;
	virtual void SetTaskInputId(const QByteArray& inputId) = 0;
	virtual const imtbase::IObjectCollection* GetTaskInputs() const = 0;
};


} // namespace imthype


