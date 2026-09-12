// Copyright Tempo Simulation, LLC. All Rights Reserved

#include "TempoROSBootstrap.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "FTempoROSBootstrapModule"

FString GetTempoROSDllDirectory()
{
	const FString TempoROSPluginPath = IPluginManager::Get().FindPlugin(TEXT("TempoROS"))->GetBaseDir();
	return FPaths::Combine(TempoROSPluginPath, TEXT("Source"), TEXT("ThirdParty"), TEXT("rclcpp"), TEXT("Binaries"), TEXT("Windows"));
}

void FTempoROSBootstrapModule::StartupModule()
{
#if PLATFORM_WINDOWS
	FPlatformProcess::PushDllDirectory(*GetTempoROSDllDirectory());
#endif
#if PLATFORM_LINUX
	// The bundled rmw_fastrtps_cpp links libpython3.11 (rosidl typesupport build). If ROS initializes
	// before the Python plugin has loaded UE's interpreter library, rmw_fastrtps_cpp fails to dlopen and
	// rmw_implementation silently falls back to rmw_cyclonedds_cpp. Preload it here, earliest possible.
	const FString PythonLib = FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries/ThirdParty/Python3/Linux/lib/libpython3.11.so.1.0"));
	PythonLibHandle = FPlatformProcess::GetDllHandle(*PythonLib);
	if (!PythonLibHandle)
	{
		UE_LOG(LogTemp, Warning, TEXT("TempoROSBootstrap: could not preload %s; rmw_fastrtps_cpp may fail to load"), *PythonLib);
	}
#endif
}

void FTempoROSBootstrapModule::ShutdownModule()
{
#if PLATFORM_WINDOWS
	FPlatformProcess::PopDllDirectory(*GetTempoROSDllDirectory());
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTempoROSBootstrapModule, TempoROSBootstrap)
