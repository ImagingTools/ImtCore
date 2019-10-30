#pragma once


#if defined _MSC_VER || defined __MINGW32__ || defined __MINGW64__
	#define PLUGIN_FUNCTION_EXPORT __declspec(dllexport)
#elif defined __GNUC__
	#define PLUGIN_FUNCTION_EXPORT __attribute__ ((visibility("default")))
#else
	#define PLUGIN_FUNCTION_EXPORT
#endif


#ifndef QT_NO_DEBUG
	#define CREATE_PLUGIN_INSTANCE_FUNCTION(suffix) CreatePluginInstanceDebug#suffix
	#define CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) "CreatePluginInstanceDebug_"##suffix
	#define DESTROY_PLUGIN_INSTANCE_FUNCTION(suffix) DestroyPluginInstanceDebug#suffix
	#define DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) "DestroyPluginInstanceDebug_"##suffix
#else // !QT_NO_DEBUG
	#define CREATE_PLUGIN_INSTANCE_FUNCTION(suffix) CreatePluginInstance#suffix
	#define CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) "CreatePluginInstance_"##suffix
	#define DESTROY_PLUGIN_INSTANCE_FUNCTION(suffix) DestroyPluginInstance#suffix
	#define DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) "DestroyPluginInstance_"##suffix
#endif // !QT_NO_DEBUG


#define DECLARE_PLUGIN_INTERFACE(PluginInterface)\
class PluginInterface;\
typedef PluginInterface* (*CreatePluginInstanceFunction)();\
typedef void (*DestroyPluginInstanceFunction)(PluginInterface*);\
extern "C" PLUGIN_FUNCTION_EXPORT PluginInterface* CREATE_PLUGIN_INSTANCE_FUNCTION();\
extern "C" PLUGIN_FUNCTION_EXPORT void DESTROY_PLUGIN_INSTANCE_FUNCTION(PluginInterface* pluginInstancePtr);


