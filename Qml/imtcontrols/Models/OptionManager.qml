import QtQuick 2.12

/*!
	\qmltype OptionManager
	\inqmlmodule Custom
	\brief Base class for loading model data.
	
	The OptionManager is an abstract base class that defines an interface for
	loading data. It provides signals to indicate successful or failed data retrieval.
	Subclasses should override the \l requestDataModel() method to implement specific
	data-fetching logic.
*/
QtObject {
	id: dataProvider
	
	/*!
		\qmlproperty var OptionManager::dataModel
		\brief Holds the model data used in QML components.
		
		This property stores the data retrieved by the provider. It should be 
		updated by subclasses when new data is loaded.
	*/
	property var optionModel

	/*!
		\qmlsignal OptionManager::requestStarted(var params)
		\brief Emitted when data is successfully loaded.

		The \a params argument contains any parameters passed when requesting the data.
	*/
	signal requestStarted(var params)
	
	/*!
		\qmlsignal OptionManager::optionModelReady(var optionModel)
		\brief Emitted when data is successfully loaded.
		
		The \a optionModel argument contains model when new data is loaded.
	*/
	signal optionModelReady(var optionModel)
	
	/*!
		\qmlsignal OptionManager::optionModelLoadFailed(string errorMessage)
		\brief Emitted when data loading fails.
		
		The \a errorMessage argument provides details about the failure.
	*/
	signal optionModelLoadFailed(string errorMessage)
	
	/*!
		\qmlmethod void OptionManager::insertOption(id, name, description, index)
		\brief Loads model data.

		This method should be overridden in subclasses to fetch data from a static
		source or a remote server.

		\a id of option.
		\a name of option.
		\a description of option.
		\a index of option.

		The base implementation does not perform an actual data retrieval and
		should not be used directly.
	*/
	function insertOption(id, name, description, index){

	}

	/*!
		\qmlmethod void OptionManager::appendOption(id, name, description)
		\brief Loads model data.

		This method should be overridden in subclasses to fetch data from a static
		source or a remote server.

		\a id of option.
		\a name of option.
		\a description of option.

		The base implementation does not perform an actual data retrieval and
		should not be used directly.
	*/
	function appendOption(id, name, description){

	}


	/*!
		\qmlmethod void removeOption(index)
		\brief Loads model data.

		This method should be overridden in subclasses to fetch data from a static
		source or a remote server.

		\a index of removed option.
		\a name of option.
		\a description of option.

		The base implementation does not perform an actual data retrieval and
		should not be used directly.
	*/
	function removeOption(index){

	}

	/*!
		\qmlmethod void getOptionCount()
		\brief Loads model data.

		This method should be overridden in subclasses to fetch data from a static
		source or a remote server.

		The base implementation does not perform an actual data retrieval and
		should not be used directly.
	*/
	function getOptionCount(){
		return 0
	}

	/*!
		\qmlmethod void OptionManager::requestDataModel(var params)
		\brief Loads model data.

		This method should be overridden in subclasses to fetch data from a static
		source or a remote server.

		\a params contains any parameters required for the data request.

		The base implementation does not perform an actual data retrieval and
		should not be used directly.
	*/
	function requestDataModel(params){
		console.warn("requestDataModel() should be implemented in a subclass")
		requestStarted(params)
		optionModelReady(params)
	}
}

