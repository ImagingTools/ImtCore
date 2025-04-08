import QtQuick 2.12
import Acf 1.0

/*!
	\qmltype DataProvider
	\inqmlmodule Custom
	\brief Base class for loading model data.
	
	The DataProvider is an abstract base class that defines an interface for 
	loading data. It provides signals to indicate successful or failed data retrieval.
	Subclasses should override the \l loadDataModel() method to implement specific 
	data-fetching logic.
*/
QtObject {
	id: dataProvider
	
	/*!
		\qmlproperty var DataProvider::dataModel
		\brief Holds the model data used in QML components.
		
		This property stores the data retrieved by the provider. It should be 
		updated by subclasses when new data is loaded.
	*/
	property var dataModel
	
	/*!
		\qmlsignal DataProvider::dataModelReady(var params)
		\brief Emitted when data is successfully loaded.
		
		The \a params argument contains any parameters passed when requesting the data.
	*/
	signal dataModelReady(var params)
	
	/*!
		\qmlsignal DataProvider::dataModelLoadFailed(string errorMessage)
		\brief Emitted when data loading fails.
		
		The \a errorMessage argument provides details about the failure.
	*/
	signal dataModelLoadFailed(string errorMessage)
	
	/*!
		\qmlmethod void DataProvider::loadDataModel(var params)
		\brief Loads model data.
		
		This method should be overridden in subclasses to fetch data from a static 
		source or a remote server.
		
		\a params contains any parameters required for the data request.
		
		The base implementation does not perform an actual data retrieval and 
		should not be used directly.
	*/
	function loadDataModel(params){
		console.warn("loadDataModel() should be implemented in a subclass")
		
		dataModelReady(params)
	}
}

