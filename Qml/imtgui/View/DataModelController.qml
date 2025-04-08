import QtQuick 2.12
import Acf 1.0

DataModelProvider {
	/*!
		\qmlsignal dataModelUpdated(var params)
		\brief Emitted when the data model is successfully updated.
		
		The \a params argument contains any parameters that were used 
		during the update process.
	*/
	signal dataModelUpdated(var params)
	
	/*!
		\qmlsignal dataModelUpdateFailed(string errorMessage)
		\brief Emitted when the data model update fails.
		
		The \a errorMessage argument provides details about the failure.
	*/
	signal dataModelUpdateFailed(string errorMessage)
	
	/*!
		\qmlmethod void updateDataModel(var params)
		\brief Updates the model data.
		
		This method should be overridden in subclasses to perform the actual 
		data update, either locally or via a network request.
		
		\a params contains any parameters required for the update.
		
		The base implementation does not perform an actual update and should 
		not be used directly.
	*/
	function updateDataModel(params) {
		console.warn("updateDataModel() should be implemented in a subclass");
		
		// Subclasses should call dataModelUpdated(params) when the update succeeds.
		dataModelUpdated(params);
	}
}

