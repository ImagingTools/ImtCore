pragma Singleton
import QtQuick 2.12

/*!
	\qmltype eventsContainer
	\inqmlmodule Custom
	\since 1.0
	\brief A simple event dispatcher for QML.
	
	The `eventsContainer` object provides a basic event system that allows objects
	to subscribe to named events and receive notifications when those events are triggered.
	
	This system does not rely on passing function references, instead it uses object-method
	pairs. Each subscriber registers with an event key, a target object, and the name of the
	method to call when the event is emitted.
	
	Example usage:
	
	\qml
	QtObject{
		id: subscriber
		
		function onSomething(data){
			console.log("Got event:", data.message)
		}
		
		Component.onCompleted:{
			eventsContainer.subscribeEvent("hello", subscriber, "onSomething")
			eventsContainer.sendEvent("hello",{ message: "Hi!" })
		}
	}
	\endqml
*/
QtObject {
	id: eventsContainer
	
	/*!
		\qmlproperty var eventsContainer::events
		\brief Internal storage for registered events.
		
		This property holds the mapping of event keys to their associated list of handlers.
		Each handler is a dictionary with `object` and `method` keys.
	*/
	property var events // structure:{ eventKey: [{ object, method } ] }
	
	/*!
		\qmlmethod void eventsContainer::sendEvent(string key, var parameters)
		\brief Emits an event with the given key.
		
		Calls all registered handlers for the event. If \a parameters is specified,
		it will be passed as an argument to the handler method.
		
		\param key The name of the event to emit.
		\param parameters Optional argument passed to each subscribed handler.
	*/
	function sendEvent(key, parameters){
		if (key in eventsContainer.events){
			let handlers = eventsContainer.events[key].slice() // делаем копию
			
			for (let i = 0; i < handlers.length; ++i){
				let handler = handlers[i]
				if (!handler || !handler.object || !handler.method)
					continue
				
				if (typeof handler.object[handler.method] === "function"){
					if (parameters !== undefined){
						handler.object[handler.method](parameters)
					} else{
						handler.object[handler.method]()
					}
				}
			}
		}
	}
	
	Component.onCompleted: {
		eventsContainer.events = {}
	}
	
	/*!
		\qmlmethod void eventsContainer::subscribeEvent(string key, QtObject object, string method)
		\brief Subscribes a method of an object to a named event.
		
		If the same object-method pair is already registered under the same key, a warning is shown.
		
		\param key The event name to subscribe to.
		\param object The QML object that contains the handler method.
		\param method The name of the method to call when the event is emitted.
	*/
	function subscribeEvent(key, object, method){
		if (!object || typeof method !== "string"){
			console.warn("subscribeEvent: invalid arguments")
			return
		}
		
		if (!(key in eventsContainer.events)){
			eventsContainer.events[key] = []
		}
		
		let exists = false
		let slots = eventsContainer.events[key]
		for (let i = 0; i < slots.length; ++i){
			let h = slots[i]
			if (h.object === object && h.method === method){
				console.warn("subscribeEvent: key already subscribed. Key: ", key)
				return
			}
		}
		
		if (!exists){
			slots.push({ object: object, method: method })
		}
	}
	
	/*!
		\qmlmethod void eventsContainer::unSubscribeEvent(string key, QtObject object, string method)
		\brief Unsubscribes a specific method of an object from a named event.
		
		\param key The name of the event.
		\param object The QML object previously registered.
		\param method The method name that was registered.
	*/
	function unSubscribeEvent(key, object, method){
		if (!(key in eventsContainer.events)) return
		
		let slots = eventsContainer.events[key]
		for (let i = 0; i < slots.length; ++i){
			let h = slots[i]
			if (h.object === object && h.method === method){
				slots.splice(i, 1)
				return
			}
		}
	}
	
	/*!
		\qmlmethod void eventsContainer::unSubscribeAllFromObject(QtObject object)
		\brief Unsubscribes the given object from all events.
		
		This removes all handler entries associated with the given object.
	*/
	function unSubscribeAllFromObject(object){
		for (let key in eventsContainer.events){
			let slots = eventsContainer.events[key]
			for (let i = slots.length - 1; i >= 0; --i){
				if (slots[i].object === object){
					slots.splice(i, 1)
				}
			}
		}
	}
}
