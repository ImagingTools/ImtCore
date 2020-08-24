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
	#define IMT_CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) IMT_MAKE_STRING(CreatePluginInstanceDebug##suffix)
#define IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION(suffix) DestroyPluginInstanceDebug_##suffix
	#define IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) IMT_MAKE_STRING(DestroyPluginInstanceDebug_##suffix)
#else // !QT_NO_DEBUG
	#define IMT_CREATE_PLUGIN_INSTANCE_FUNCTION(suffix) CreatePluginInstance_##suffix
	#define IMT_CREATE_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) IMT_MAKE_STRING(CreatePluginInstance_##suffix)
	#define IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION(suffix) DestroyPluginInstance_##suffix
	#define IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION_NAME(suffix) IMT_MAKE_STRING(DestroyPluginInstance_##suffix)
#endif // !QT_NO_DEBUG


#define IMT_CREATE_PLUGIN_FUNCTION(PluginType) CreatePluginInstanceFunction##PluginType
#define IMT_DESTROY_PLUGIN_FUNCTION(PluginType) DestroyPluginInstanceFunction##PluginType

#define IMT_CREATE_PLUGIN_FUNCTION_PTR(PluginType, PluginInterface)\
PluginInterface* (*IMT_CREATE_PLUGIN_FUNCTION(PluginType))();

#define IMT_DESTROY_PLUGIN_FUNCTION_PTR(PluginType, PluginInterface)\
void (*IMT_DESTROY_PLUGIN_FUNCTION(PluginType))(PluginInterface*);\

#define IMT_DECLARE_PLUGIN_INTERFACE(PluginType, PluginInterface)\
typedef IMT_CREATE_PLUGIN_FUNCTION_PTR(PluginType, PluginInterface);\
typedef IMT_DESTROY_PLUGIN_FUNCTION_PTR(PluginType, PluginInterface);\
extern "C" IMT_PLUGIN_FUNCTION_EXPORT PluginInterface* IMT_CREATE_PLUGIN_INSTANCE_FUNCTION(PluginType)();\
extern "C" IMT_PLUGIN_FUNCTION_EXPORT void IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION(PluginType)(PluginInterface* pluginInstancePtr);\


#define IMT_REGISTER_PLUGIN(PluginInterface, PluginImplementation, PluginType, PluginName)\
	IMT_DECLARE_PLUGIN_INTERFACE(PluginType, PluginInterface);\
	PluginInterface* IMT_CREATE_PLUGIN_INSTANCE_FUNCTION(PluginType)()\
	{\
		return new PluginImplementation(#PluginName);\
	}\
	void IMT_DESTROY_PLUGIN_INSTANCE_FUNCTION(PluginType)(PluginInterface* pluginInstancePtr)\
	{\
		Q_ASSERT(pluginInstancePtr != nullptr);\
		if (pluginInstancePtr != nullptr){\
			delete pluginInstancePtr;\
		};\
	}\


