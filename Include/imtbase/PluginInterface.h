#pragma once


#if defined _MSC_VER || defined __MINGW32__ || defined __MINGW64__
	#define IMT_PLUGIN_FUNCTION_EXPORT __declspec(dllexport)
#elif defined __GNUC__
	#define IMT_PLUGIN_FUNCTION_EXPORT __attribute__ ((visibility("default")))
#else
	#define IMT_PLUGIN_FUNCTION_EXPORT
#endif

#define IMT_MAKE_STRING(value) #value

#ifndef QT_NO_DEBUG
	#define IMT_CREATE_PLUGIN_INSTANCE_FUNCTION(suffix) CreatePluginInstanceDebug_##suffix
	#define IMT_CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) "CreatePluginInstanceDebug_"##IMT_MAKE_STRING(suffix)
#define IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION(suffix) DestroyPluginInstanceDebug_##suffix
	#define IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) "DestroyPluginInstanceDebug_"##IMT_MAKE_STRING(suffix)
#else // !QT_NO_DEBUG
	#define IMT_CREATE_PLUGIN_INSTANCE_FUNCTION(suffix) CreatePluginInstance_##suffix
	#define IMT_CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) "CreatePluginInstance_"##IMT_MAKE_STRING(suffix)
	#define IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION(suffix) DestroyPluginInstance_##suffix
	#define IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) "DestroyPluginInstance_"##IMT_MAKE_STRING(suffix)
#endif // !QT_NO_DEBUG


#define IMT_DECLARE_PLUGIN_INTERFACE(PluginName, PluginInterface)\
typedef PluginInterface* (*CreatePluginInstanceFunction)();\
typedef void (*DestroyPluginInstanceFunction)(PluginInterface*);\
extern "C" IMT_PLUGIN_FUNCTION_EXPORT PluginInterface* IMT_CREATE_PLUGIN_INSTANCE_FUNCTION(PluginName);\
extern "C" IMT_PLUGIN_FUNCTION_EXPORT void IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION(PluginName)(PluginInterface* pluginInstancePtr);\


#define IMT_REGISTER_PLUGIN(PluginInterface, PluginImplementation, PluginName)\
	PluginInterface* IMT_CREATE_PLUGIN_INSTANCE_FUNCTION(PluginName)()\
	{\
		return new PluginImplementation(#PluginName);\
	}\
	void IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION(PluginName)(PluginInterface* pluginInstancePtr)\
	{\
		delete pluginInstancePtr;\
	}\


