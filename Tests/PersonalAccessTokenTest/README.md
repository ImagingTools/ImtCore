# Personal Access Token (PAT) Testing

## Overview

This directory contains comprehensive tests for the Personal Access Token (PAT) system in imtauth. The tests validate all core functionality of the PAT manager including token creation, validation, revocation, expiration, and lifecycle management.

## Test Structure

### Files

- **CPersonalAccessTokenTest.h** - Test class header with Qt Test declarations
- **CPersonalAccessTokenTest.cpp** - Test implementation with 7 test methods
- **CPersonalAccessTokenTestMain.cpp** - Test entry point using CStandardTestExecutor
- **Partitura/PersonalAccessTokenTest.acc** - Component configuration for test environment
- **Partitura/PersonalAccessTokenTest.accl** - Component layout configuration

### Test Cases

#### 1. testTokenCreation()
**Purpose**: Validates that tokens are created correctly with proper format and metadata.

**Tests**:
- Token creation returns success status
- Generated token ID is not empty
- Raw token has the expected `imt_pat_` prefix
- Token can be retrieved by ID after creation
- Token metadata (userId, name) matches input parameters

**Expected Behavior**: Tokens should be created with cryptographically secure random data, proper prefix, and all metadata should be stored correctly.

#### 2. testTokenValidation()
**Purpose**: Ensures that token validation works correctly for both valid and invalid tokens.

**Tests**:
- Valid tokens are successfully validated
- Validated userId and scopes match creation parameters
- Invalid/malformed tokens are rejected
- Token validation returns false for non-existent tokens

**Expected Behavior**: Only tokens that exist in the system and have not been revoked or expired should validate successfully.

#### 3. testTokenRevocation()
**Purpose**: Verifies that token revocation immediately invalidates tokens.

**Tests**:
- Token is valid before revocation
- Revocation operation succeeds
- Token becomes invalid after revocation
- Revoked tokens cannot be used for authentication

**Expected Behavior**: Revoking a token should immediately make it unusable while keeping it in the system for audit purposes.

#### 4. testTokenExpiration()
**Purpose**: Tests that expired tokens are properly rejected during validation.

**Tests**:
- Tokens with past expiration dates are invalid
- Tokens with future expiration dates are valid
- Expiration checking happens during validation

**Expected Behavior**: The system should automatically reject tokens that have passed their expiration timestamp.

#### 5. testGetTokenIds()
**Purpose**: Validates that all tokens for a user can be retrieved.

**Tests**:
- Multiple tokens can be created for the same user
- GetTokenIds returns all token IDs for a user
- All created token IDs are present in the returned list
- Non-existent users return empty token lists

**Expected Behavior**: Users should be able to enumerate all their tokens by ID.

#### 6. testUpdateLastUsedAt()
**Purpose**: Ensures that token usage timestamps are properly tracked.

**Tests**:
- Last used timestamp can be updated
- Timestamp changes after update
- Updated timestamp is more recent than original
- Updating non-existent tokens fails gracefully

**Expected Behavior**: The system should track when tokens are used for authentication to support security auditing.

#### 7. testDeleteToken()
**Purpose**: Verifies that tokens can be permanently deleted from the system.

**Tests**:
- Token exists before deletion
- Deletion operation succeeds
- Token cannot be retrieved after deletion
- Deleted token is removed from user's token list
- Deleted tokens cannot be validated
- Deleting non-existent tokens fails gracefully

**Expected Behavior**: Deletion should permanently remove tokens from the system, unlike revocation which keeps them for audit purposes.

## Configuration

### Component Configuration (Partitura/PersonalAccessTokenTest.acc)

The test uses ACF (Acula Component Framework) to set up the testing environment with the following components:

1. **JsonSerializer** - Provides JSON persistence for token objects
2. **PersonalAccessToken** - Factory component for creating token objects
3. **TokenCollection** - Object collection for storing tokens in memory
4. **PersonalAccessTokenManager** - The manager component being tested

The configuration links these components together to create a working PAT system for testing.

## Running Tests

### Prerequisites

1. **IMTCOREDIR** environment variable must be set to the ImtCore repository root
2. **Config/ImtCoreTest.awc** configuration file must exist
3. Qt Test framework and all ImtCore dependencies must be available
4. ACF (Acula Component Framework) must be properly installed

### Build

The test is integrated into the main CMake build system:

```bash
# From ImtCore root directory
mkdir build
cd build
cmake ..
cmake --build . --target PersonalAccessTokenTest
```

### Run

```bash
# From build directory
./Bin/Debug_*/PersonalAccessTokenTest  # Or Release_* depending on build type
```

### TeamCity Integration

The test is automatically built and run as part of the CI/CD pipeline configured in TeamCity. Results are reported back to GitHub through the workflow integration.

## Security Considerations

The tests validate important security features:

1. **Token Hashing**: Tokens are stored as SHA-256 hashes, never in plaintext
2. **Token Format**: Tokens use the `imt_pat_` prefix for identification
3. **Revocation**: Revoked tokens are immediately unusable
4. **Expiration**: Expired tokens are automatically rejected
5. **Validation**: Token validation uses secure comparison methods

## Test Data Isolation

Each test method uses unique user IDs to avoid interference:
- `test_user_123` - Token creation tests
- `test_user_456` - Validation tests
- `test_user_789` - Revocation tests
- `test_user_expired` - Expiration tests
- `test_user_multi` - Multiple token tests
- `test_user_lastused` - Last used timestamp tests
- `test_user_delete` - Deletion tests

This ensures tests can run independently and in any order.

## Extending Tests

To add new test cases:

1. Add test method declaration in `CPersonalAccessTokenTest.h`:
   ```cpp
   private slots:
       void testNewFeature();
   ```

2. Implement the test in `CPersonalAccessTokenTest.cpp`:
   ```cpp
   void CPersonalAccessTokenTest::testNewFeature()
   {
       QVERIFY2(m_tokenManagerPtr != nullptr, "Token manager not initialized");
       // Test implementation
   }
   ```

3. Follow Qt Test conventions using `QVERIFY2()`, `QCOMPARE()`, etc.

## Troubleshooting

### Common Issues

1. **"Failed to create component accessor"**
   - Ensure IMTCOREDIR environment variable is set correctly
   - Verify PersonalAccessTokenTest.acc file exists and is valid
   - Check that Config/ImtCoreTest.awc exists

2. **"Failed to get PersonalAccessTokenManager component"**
   - Verify ImtAuthPck library is built and available
   - Check that PersonalAccessToken component is registered
   - Ensure all dependencies are linked correctly

3. **Build Errors**
   - Verify all Qt modules are available (Core, Test, Widgets)
   - Ensure ACF framework is properly installed
   - Check that imtauth and imtbase libraries are built

## References

- **PAT Documentation**: `Include/imtauth/PersonalAccessTokens.md`
- **PAT Implementation**: `Include/imtauth/CPersonalAccessTokenManagerComp.cpp`
- **Interface Definition**: `Include/imtauth/IPersonalAccessTokenManager.h`
- **Build Configuration**: `Tests/PersonalAccessTokenTest/CMake/CMakeLists.txt`
