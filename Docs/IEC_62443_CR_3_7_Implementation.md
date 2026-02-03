# IEC 62443-4-2 CR 3.7: Least Functionality Implementation

## Overview

This document describes the implementation of IEC 62443-4-2 Component Requirement 3.7 (Least Functionality) in ImtCore.

## Requirement

**IEC 62443-4-2 CR 3.7** states:

> The control system shall provide the capability to specifically prohibit or restrict the use of unnecessary functions, ports, protocols, and/or services.

This is based on the principle of "least functionality" - the system should provide only essential capabilities and prohibit or restrict unnecessary functions that could increase the attack surface.

## Implementation

### Module: `imtfunc`

The implementation is provided in the new `imtfunc` module under `Include/imtfunc/`.

### Key Components

#### 1. IFunctionalityInfo

Interface describing a controllable piece of functionality:

```cpp
// Get information about functionality
QByteArray GetId() const;
QString GetName() const;
QString GetDescription() const;
FunctionalityType GetType() const;  // FEATURE, PROTOCOL, SERVICE, API_ENDPOINT, SECURITY_PROTOCOL
bool IsRequired() const;
bool IsEnabledByDefault() const;
QByteArrayList GetDependencies() const;
int GetRiskLevel() const;  // 0-3 (low to high risk)
```

#### 2. IFunctionalityManager

Central manager for controlling system functionality:

```cpp
// Register controllable functionality
bool RegisterFunctionality(IFunctionalityInfo* pFunctionalityInfo);

// Check and control functionality state
bool IsFunctionalityEnabled(const QByteArray& functionalityId) const;
bool SetFunctionalityEnabled(const QByteArray& functionalityId, bool enabled, const QString& reason);
bool CanDisableFunctionality(const QByteArray& functionalityId) const;

// Query functionality
QByteArrayList GetAllFunctionality(int type = -1) const;
QByteArrayList GetEnabledFunctionality(int type = -1) const;
QByteArrayList GetDisabledFunctionality(int type = -1) const;
QByteArrayList GetDependentFunctionality(const QByteArray& functionalityId) const;

// Configuration management
bool ResetToDefaults();
QByteArrayList ValidateConfiguration() const;
```

#### 3. ISecurityComplianceDocumentationProvider

Generates compliance documentation and reports:

```cpp
// Get compliance status
ComplianceStatus GetComplianceStatus(const QString& requirementId) const;
QStringList GetSupportedRequirements() const;

// Generate documentation
QString GenerateComplianceReport(const QString& standardId, const QString& format) const;
QString GetDisabledFunctionalityDocumentation(const QString& format) const;
QString GetSecurityControlsDocumentation(const QString& format) const;
QString GetConfigurationGuidance(const QString& format) const;
```

## Usage Example

### 1. Register Functionality

```cpp
// Get functionality manager from component system
IFunctionalityManager* pManager = // ... obtain from component system

// Register a network protocol
CFunctionalityInfo* pTcpInfo = new CFunctionalityInfo(
    "protocol.network.tcp",
    "TCP Protocol",
    "Transmission Control Protocol for reliable network communication",
    FunctionalityType::PROTOCOL,
    true,   // enabled by default
    true,   // required (cannot be disabled)
    2);     // medium-high risk

pManager->RegisterFunctionality(pTcpInfo);

// Register an optional protocol with dependencies
CFunctionalityInfo* pWebSocketInfo = new CFunctionalityInfo(
    "protocol.network.websocket",
    "WebSocket Protocol",
    "WebSocket protocol for real-time bidirectional communication",
    FunctionalityType::PROTOCOL,
    false,  // disabled by default
    false,  // optional (can be disabled)
    2);     // medium-high risk

pWebSocketInfo->AddDependency("protocol.network.tcp");
pManager->RegisterFunctionality(pWebSocketInfo);
```

### 2. Control Functionality

```cpp
// Check if functionality is enabled
if (pManager->IsFunctionalityEnabled("protocol.network.tcp"))
{
    // TCP is enabled - start TCP server
}

// Disable optional functionality
if (pManager->CanDisableFunctionality("protocol.network.udp"))
{
    pManager->SetFunctionalityEnabled("protocol.network.udp", false,
        "Disabled per security policy - not required for this deployment");
}

// Get list of disabled functionality
QByteArrayList disabled = pManager->GetDisabledFunctionality();
```

### 3. Generate Compliance Documentation

```cpp
// Get documentation provider from component system
ISecurityComplianceDocumentationProvider* pDocProvider = // ... obtain from component system

// Generate compliance report
QString report = pDocProvider->GenerateComplianceReport("IEC 62443-4-2", "MARKDOWN");

// Get compliance status for specific requirement
ComplianceStatus status = pDocProvider->GetComplianceStatus("IEC 62443-4-2 CR 3.7");
if (status.status == "COMPLIANT")
{
    // System is compliant
}

// Get documentation of disabled functionality
QString disabledDoc = pDocProvider->GetDisabledFunctionalityDocumentation("HTML");
```

## Functionality Categories

### FunctionalityType::FEATURE
Application features such as:
- Reporting and analytics
- Data export/import
- Advanced configuration options
- User management features

### FunctionalityType::PROTOCOL
Network protocols such as:
- TCP, UDP
- HTTP, HTTPS
- WebSocket
- GraphQL, REST

### FunctionalityType::SERVICE
System services such as:
- Remote access
- File transfer
- Logging service
- Update service

### FunctionalityType::API_ENDPOINT
Specific API endpoints or operations such as:
- GraphQL mutations
- REST API paths
- Administrative operations

### FunctionalityType::SECURITY_PROTOCOL
Security protocol options such as:
- TLS 1.2, TLS 1.3
- Cipher suites
- Authentication methods

## Risk Levels

Functionality is assigned a risk level from 0-3:

- **0 (Low)**: Read-only features, display functionality
- **1 (Medium-Low)**: Standard operations, limited privileges
- **2 (Medium-High)**: Write operations, network protocols, data modification
- **3 (High)**: Remote access, administrative operations, system configuration

## Dependency Management

The system tracks dependencies between functionality:

- **Required functionality** cannot be disabled
- Functionality with **enabled dependents** cannot be disabled
- Enabling functionality **automatically checks dependencies**
- Configuration validation **detects missing dependencies**

## Best Practices

### Security Configuration

1. **Start Minimal**: Begin with only required functionality enabled
2. **Enable on Demand**: Enable additional functionality only when needed
3. **Regular Review**: Periodically audit enabled functionality
4. **Document Justification**: Document security justification for high-risk functionality
5. **Validate Configuration**: Use `ValidateConfiguration()` to ensure consistency

### Deployment Considerations

1. **Default Configuration**: Set appropriate defaults for your security level
2. **Role-Based Control**: Consider integrating with role-based access control
3. **Audit Logging**: Log all functionality state changes with reasons
4. **Testing**: Test critical functionality after making changes
5. **Compliance Documentation**: Generate and maintain compliance reports

## Compliance

This implementation provides the following compliance capabilities:

### CR 3.7 Requirements Met

✅ **Capability to prohibit/restrict functionality**: `SetFunctionalityEnabled()` method

✅ **Control over functions**: FunctionalityType::FEATURE management

✅ **Control over ports**: Through network protocol management

✅ **Control over protocols**: FunctionalityType::PROTOCOL and SECURITY_PROTOCOL management

✅ **Control over services**: FunctionalityType::SERVICE management

✅ **Documentation**: `ISecurityComplianceDocumentationProvider` generates audit documentation

### Audit Support

The implementation provides:
- List of all controllable functionality
- Current enabled/disabled state
- Risk assessment for each functionality
- Dependency relationships
- Configuration validation
- Compliance status reporting

## Integration Points

### Existing ImtCore Components

The functionality management system can be integrated with:

- **`IFeatureInfo`** (imtlic): Feature licensing and optional features
- **`IPermissionsProvider`** (imtauth): Role-based access control
- **`IDeviceConfigurationManager`** (imtdev): Device-specific configuration
- **`ISslConfigurationManager`** (imtcom): SSL/TLS protocol configuration
- **Server components** (imtrest): TCP/UDP/WebSocket server control

### Future Enhancements

Potential enhancements include:

1. **Persistence**: Save functionality state to configuration files
2. **GUI**: Admin interface for managing functionality
3. **Role Integration**: Link to user roles and permissions
4. **Automatic Discovery**: Auto-register functionality from components
5. **Policy Engine**: Rule-based functionality control
6. **Metrics**: Track usage to identify unused functionality

## Testing

### Unit Tests

Create tests for:
- Functionality registration
- Enable/disable operations
- Dependency validation
- Configuration consistency
- Compliance status generation

### Integration Tests

Test integration with:
- Server components (enable/disable protocols)
- Feature licensing system
- Configuration persistence
- Access control system

## References

- **IEC 62443-4-2:2019**: Security for industrial automation and control systems - Part 4-2: Technical security requirements for IACS components
- **CR 3.7**: Least Functionality requirement
- **NIST SP 800-53 CM-7**: Least Functionality control

