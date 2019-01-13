#pragma once

#ifdef nicp_EXPORTS
#define NICP_API __declspec(dllexport)
#else
#define NICP_API __declspec(dllimport)
#endif