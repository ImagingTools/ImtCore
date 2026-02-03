# IEC 62443-4-2 CR 3.7 Usage Examples

## Example 1: Basic Functionality Management

```cpp
#include <imtfunc/Init.h>
#include <imtfunc/CFunctionalityManagerComp.h>
#include <imtfunc/CFunctionalityInfo.h>

// Create functionality manager
imtfunc::CFunctionalityManagerComp* pManager = new imtfunc::CFunctionalityManagerComp();

// Register network protocols
auto* pTCP = new imtfunc::CFunctionalityInfo(
    "protocol.network.tcp",
    "TCP Protocol",
    "Transmission Control Protocol for reliable communication",
    imtfunc::FunctionalityType::PROTOCOL,
    true,   // enabled by default
    true,   // required
    2);     // medium-high risk
pManager->RegisterFunctionality(pTCP);

auto* pUDP = new imtfunc::CFunctionalityInfo(
    "protocol.network.udp",
    "UDP Protocol",
    "User Datagram Protocol for connectionless communication",
    imtfunc::FunctionalityType::PROTOCOL,
    false,  // disabled by default
    false,  // optional
    2);     // medium-high risk
pManager->RegisterFunctionality(pUDP);

auto* pWebSocket = new imtfunc::CFunctionalityInfo(
    "protocol.network.websocket",
    "WebSocket Protocol",
    "WebSocket for real-time bidirectional communication",
    imtfunc::FunctionalityType::PROTOCOL,
    false,  // disabled by default
    false,  // optional
    2);     // medium-high risk
pWebSocket->AddDependency("protocol.network.tcp");
pManager->RegisterFunctionality(pWebSocket);
```

## Example 2: Controlling Server Functionality

```cpp
// Check if protocol is enabled before starting server
if (pManager->IsFunctionalityEnabled("protocol.network.tcp"))
{
    // Start TCP server
    StartTcpServer();
}

if (pManager->IsFunctionalityEnabled("protocol.network.websocket"))
{
    // Start WebSocket server (depends on TCP)
    StartWebSocketServer();
}

// Disable UDP if not needed
if (pManager->CanDisableFunctionality("protocol.network.udp"))
{
    pManager->SetFunctionalityEnabled("protocol.network.udp", false,
        "UDP not required for this deployment");
}
```

## Example 3: Feature Control

```cpp
// Register application features
auto* pExport = new imtfunc::CFunctionalityInfo(
    "feature.data.export",
    "Data Export",
    "Export data to external files (CSV, JSON, XML)",
    imtfunc::FunctionalityType::FEATURE,
    true,   // enabled by default
    false,  // optional
    2);     // medium-high risk (data exfiltration)
pManager->RegisterFunctionality(pExport);

auto* pRemoteAccess = new imtfunc::CFunctionalityInfo(
    "feature.admin.remote",
    "Remote Administration",
    "Remote administrative access to system",
    imtfunc::FunctionalityType::FEATURE,
    false,  // disabled by default
    false,  // optional
    3);     // high risk
pManager->RegisterFunctionality(pRemoteAccess);

// In secure environments, disable high-risk features
if (IsHighSecurityEnvironment())
{
    pManager->SetFunctionalityEnabled("feature.admin.remote", false,
        "Remote administration disabled per security policy");
    pManager->SetFunctionalityEnabled("feature.data.export", false,
        "Data export disabled to prevent data exfiltration");
}
```

## Example 4: API Endpoint Control

```cpp
// Register API endpoints
auto* pUserCreate = new imtfunc::CFunctionalityInfo(
    "api.graphql.mutation.createUser",
    "Create User Mutation",
    "GraphQL mutation for creating new users",
    imtfunc::FunctionalityType::API_ENDPOINT,
    true,   // enabled by default
    false,  // optional
    3);     // high risk
pManager->RegisterFunctionality(pUserCreate);

auto* pUserDelete = new imtfunc::CFunctionalityInfo(
    "api.graphql.mutation.deleteUser",
    "Delete User Mutation",
    "GraphQL mutation for deleting users",
    imtfunc::FunctionalityType::API_ENDPOINT,
    false,  // disabled by default
    false,  // optional
    3);     // high risk
pManager->RegisterFunctionality(pUserDelete);

// In GraphQL resolver, check if endpoint is enabled
if (pManager->IsFunctionalityEnabled("api.graphql.mutation.deleteUser"))
{
    // Execute delete operation
    DeleteUser(userId);
}
else
{
    // Return error - functionality disabled
    return Error("Delete user functionality is disabled");
}
```

## Example 5: Security Protocol Control

```cpp
// Register TLS versions
auto* pTLS12 = new imtfunc::CFunctionalityInfo(
    "security.tls.v1_2",
    "TLS 1.2",
    "Transport Layer Security version 1.2",
    imtfunc::FunctionalityType::SECURITY_PROTOCOL,
    true,   // enabled by default
    false,  // optional (can be disabled if only TLS 1.3)
    1);     // medium-low risk
pManager->RegisterFunctionality(pTLS12);

auto* pTLS13 = new imtfunc::CFunctionalityInfo(
    "security.tls.v1_3",
    "TLS 1.3",
    "Transport Layer Security version 1.3",
    imtfunc::FunctionalityType::SECURITY_PROTOCOL,
    true,   // enabled by default
    true,   // required
    0);     // low risk
pManager->RegisterFunctionality(pTLS13);

// For maximum security, disable older protocols
if (IsMaxSecurityMode())
{
    pManager->SetFunctionalityEnabled("security.tls.v1_2", false,
        "TLS 1.2 disabled - only TLS 1.3 allowed in max security mode");
}
```

## Example 6: Configuration Validation

```cpp
// Validate current configuration
QByteArrayList issues = pManager->ValidateConfiguration();

if (!issues.isEmpty())
{
    qWarning() << "Functionality configuration has issues:";
    for (const QByteArray& issue : issues)
    {
        qWarning() << "  -" << issue;
    }
    
    // Optionally reset to defaults
    if (UserConfirmsReset())
    {
        pManager->ResetToDefaults();
    }
}
```

## Example 7: Querying Functionality State

```cpp
// Get all protocols
QByteArrayList protocols = pManager->GetAllFunctionality(
    static_cast<int>(imtfunc::FunctionalityType::PROTOCOL));

qDebug() << "Registered protocols:" << protocols;

// Get enabled protocols
QByteArrayList enabledProtocols = pManager->GetEnabledFunctionality(
    static_cast<int>(imtfunc::FunctionalityType::PROTOCOL));

qDebug() << "Enabled protocols:" << enabledProtocols;

// Get disabled functionality (for audit)
QByteArrayList disabled = pManager->GetDisabledFunctionality();
qDebug() << "Disabled functionality (demonstrating least functionality):" << disabled;
```

## Example 8: Dependency Checking

```cpp
// Check what depends on TCP
QByteArrayList tcpDependents = pManager->GetDependentFunctionality("protocol.network.tcp");

qDebug() << "Functionality depending on TCP:" << tcpDependents;

// Try to disable something with dependents
if (!pManager->CanDisableFunctionality("protocol.network.tcp"))
{
    QByteArrayList dependents = pManager->GetDependentFunctionality("protocol.network.tcp");
    qDebug() << "Cannot disable TCP - required by:" << dependents;
}
```

## Example 9: Compliance Documentation

```cpp
#include <imtfunc/CSecurityComplianceDocumentationProviderComp.h>

// Create documentation provider
auto* pDocProvider = new imtfunc::CSecurityComplianceDocumentationProviderComp();
pDocProvider->SetFunctionalityManager(pManager);

// Generate compliance report
QString report = pDocProvider->GenerateComplianceReport("IEC 62443-4-2", "MARKDOWN");
QFile reportFile("compliance_report.md");
if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text))
{
    QTextStream out(&reportFile);
    out << report;
    reportFile.close();
}

// Check compliance status
imtfunc::ComplianceStatus status = pDocProvider->GetComplianceStatus("IEC 62443-4-2 CR 3.7");
qDebug() << "CR 3.7 Status:" << status.status;
qDebug() << "Evidence:" << status.evidence;

// Generate disabled functionality documentation
QString disabledDoc = pDocProvider->GetDisabledFunctionalityDocumentation("HTML");
QFile disabledFile("disabled_functionality.html");
if (disabledFile.open(QIODevice::WriteOnly | QIODevice::Text))
{
    QTextStream out(&disabledFile);
    out << disabledDoc;
    disabledFile.close();
}
```

## Example 10: Integration with Existing Systems

```cpp
// Integration with feature licensing (imtlic)
#include <imtlic/IFeatureInfo.h>

void RegisterLicensedFeature(imtlic::IFeatureInfo* pFeatureInfo, 
                              imtfunc::CFunctionalityManagerComp* pFuncManager)
{
    // Create corresponding functionality info
    auto* pFuncInfo = new imtfunc::CFunctionalityInfo(
        pFeatureInfo->GetFeatureId(),
        pFeatureInfo->GetFeatureName(),
        pFeatureInfo->GetFeatureDescription(),
        imtfunc::FunctionalityType::FEATURE,
        !pFeatureInfo->IsOptional(),  // required if not optional
        !pFeatureInfo->IsOptional(),
        pFeatureInfo->IsPermission() ? 2 : 1);  // Higher risk if permission-related
    
    // Add dependencies from feature dependencies
    pFuncInfo->SetDependencies(pFeatureInfo->GetDependencies());
    
    // Register in functionality manager
    pFuncManager->RegisterFunctionality(pFuncInfo);
}

// Integration with server components
#include <imtrest/CTcpServerComp.h>

bool ShouldStartTcpServer(imtfunc::IFunctionalityManager* pManager)
{
    return pManager->IsFunctionalityEnabled("protocol.network.tcp");
}

void ConfigureServersFromFunctionalityManager(
    imtfunc::IFunctionalityManager* pManager,
    imtrest::CTcpServerComp* pTcpServer,
    imtrest::CUdpServerComp* pUdpServer)
{
    // Enable/disable servers based on functionality state
    if (pManager->IsFunctionalityEnabled("protocol.network.tcp"))
    {
        pTcpServer->Start();
    }
    else
    {
        pTcpServer->Stop();
    }
    
    if (pManager->IsFunctionalityEnabled("protocol.network.udp"))
    {
        pUdpServer->Start();
    }
    else
    {
        pUdpServer->Stop();
    }
}
```

## Example 11: Risk-Based Configuration

```cpp
// Configure based on security level
void ConfigureForSecurityLevel(imtfunc::IFunctionalityManager* pManager, int securityLevel)
{
    QByteArrayList allFunc = pManager->GetAllFunctionality();
    
    for (const QByteArray& funcId : allFunc)
    {
        imtfunc::IFunctionalityInfo* pInfo = pManager->GetFunctionalityInfo(funcId);
        if (!pInfo || pInfo->IsRequired())
            continue;
        
        int riskLevel = pInfo->GetRiskLevel();
        
        // Disable functionality if its risk exceeds security tolerance
        bool shouldEnable = (riskLevel < securityLevel);
        
        if (shouldEnable != pManager->IsFunctionalityEnabled(funcId))
        {
            pManager->SetFunctionalityEnabled(funcId, shouldEnable,
                QString("Security level %1 adjustment").arg(securityLevel));
        }
    }
}

// Usage:
// Security Level 0 = Allow all (testing)
// Security Level 1 = Allow up to low risk
// Security Level 2 = Allow up to medium-low risk  
// Security Level 3 = Allow up to medium-high risk (default)
// Security Level 4 = Only required functionality (maximum security)

ConfigureForSecurityLevel(pManager, 2);  // Enable low and medium-low risk only
```

