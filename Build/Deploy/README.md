# Deployment Helper Scripts

Windows batch scripts used by the TeamCity release pipelines (`ItDevProjects / Releases`) to stop/start services and processes around a silent install, without relying on a fixed guess-and-hope timeout.

## Scripts

### WaitForServiceState.bat

Polls `sc query` until a Windows service reaches `RUNNING` or `STOPPED`, or a timeout elapses.

**Usage:** `WaitForServiceState.bat <serviceName> <RUNNING|STOPPED> [timeoutSeconds=60]`

### WaitForProcess.bat

Polls `tasklist` until a plain executable (not registered as its own Windows service) is running or has exited, or a timeout elapses. Used for products supervised by another process/service rather than running as their own service.

**Usage:** `WaitForProcess.bat <imageName.exe> <RUNNING|STOPPED> [timeoutSeconds=60]`

Both scripts exit 0 when the target state is reached and 1 on timeout (or if the service does not exist) — callers decide whether that failure is fatal.
