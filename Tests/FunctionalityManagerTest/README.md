# Functionality Manager Test

This test suite validates the IEC 62443-4-2 CR 3.7 (Least Functionality) implementation.

## Test Coverage

### Functionality Registration
- `testRegisterFunctionality`: Tests registering functionality and preventing duplicates
- `testUnregisterFunctionality`: Tests removing functionality from registry
- `testGetFunctionalityInfo`: Tests retrieving functionality information
- `testGetAllFunctionality`: Tests listing all registered functionality with filtering

### State Management
- `testIsFunctionalityEnabled`: Tests checking enabled/disabled state
- `testSetFunctionalityEnabled`: Tests enabling and disabling functionality
- `testCanDisableFunctionality`: Tests checking if functionality can be disabled
- `testGetEnabledFunctionality`: Tests listing enabled functionality
- `testGetDisabledFunctionality`: Tests listing disabled functionality

### Dependency Management
- `testDependencyEnforcement`: Tests that dependencies must be enabled first
- `testDependentFunctionality`: Tests listing what depends on functionality
- `testCannotDisableWithDependents`: Tests preventing disable when dependents are enabled

### Configuration
- `testResetToDefaults`: Tests resetting all functionality to default state
- `testValidateConfiguration`: Tests detecting configuration inconsistencies
- `testRequiredFunctionality`: Tests that required functionality cannot be disabled

### Compliance Documentation
- `testComplianceStatus`: Tests getting IEC 62443-4-2 CR 3.7 compliance status
- `testGenerateComplianceReport`: Tests generating compliance reports in multiple formats
- `testDisabledFunctionalityDocumentation`: Tests generating documentation of disabled functionality

## Running Tests

### Build and Run

```bash
# Build the test
cd Tests/FunctionalityManagerTest
mkdir build
cd build
cmake ..
make

# Run the test
./FunctionalityManagerTest
```

### Expected Results

All tests should pass, demonstrating:
- ✅ Functionality can be registered and managed
- ✅ Enabled/disabled state is properly tracked
- ✅ Dependencies are enforced
- ✅ Required functionality cannot be disabled
- ✅ Configuration validation detects issues
- ✅ Compliance documentation is generated

## Test Scenarios

### Scenario 1: Basic Registration
1. Register TCP protocol functionality
2. Verify it appears in registry
3. Check initial enabled state

### Scenario 2: Dependency Chain
1. Register base protocol (TCP)
2. Register dependent protocol (WebSocket depends on TCP)
3. Verify WebSocket cannot be enabled without TCP
4. Enable TCP, then enable WebSocket
5. Verify TCP cannot be disabled while WebSocket is enabled

### Scenario 3: Required Functionality
1. Register required functionality (marked as required=true)
2. Verify it is enabled by default
3. Attempt to disable it
4. Verify disable operation fails
5. Confirm it remains enabled

### Scenario 4: Compliance Reporting
1. Register mix of enabled and disabled functionality
2. Generate CR 3.7 compliance status
3. Verify status indicates COMPLIANT or PARTIAL with evidence
4. Generate full compliance report
5. Verify report contains all required sections

## Integration Testing

For integration testing with real components:

1. **Server Components**: Test that disabling protocol functionality actually stops servers
2. **Feature Licensing**: Test integration with `IFeatureInfo` system
3. **Access Control**: Test integration with permission system
4. **Persistence**: Test saving and loading functionality state
5. **Audit Logging**: Test that state changes are logged

## Performance

The functionality manager should handle:
- At least 1000 registered functionality items
- Dependency trees up to 10 levels deep
- Sub-millisecond query operations
- Efficient validation with large dependency graphs

