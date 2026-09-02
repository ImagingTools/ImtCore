# GitHub Actions - TeamCity Integration

This GitHub Actions workflow integrates with an external TeamCity build server to trigger builds and report results back to GitHub.

## Overview

The `teamcity-trigger.yml` workflow:
1. Triggers TeamCity builds (Windows and Linux) in parallel using a matrix strategy when pull requests are created/updated or commits are pushed to main/master
2. Waits for both TeamCity builds to complete with per-state timeouts (30min queued, 60min running) within an overall 90-minute job timeout
3. Fetches and displays build logs from TeamCity for easy debugging
4. Reports the build status back to GitHub and **blocks PR merges if builds fail**

## Key Features

- **Matrix Strategy**: Single job with platform matrix (windows/linux) instead of duplicate jobs for better maintainability
- **JSON API**: Uses JSON format with `jq` for safe payload construction instead of XML
- **Branch Handling**: Proper `branchName` parameter and ref checkout for accurate PR builds
- **Retry Logic**: Automatic retry on network errors with `curl --fail-with-body --retry`
- **Separate Timeouts**: Per-state maximum durations (30min queued, 60min running) within an overall 90-minute job timeout
- **Build Log Access**: Automatically fetches and displays the last 100 lines of build logs, build problems, and test failures
- **Blocking Behavior**: Failed builds block PR merges to ensure code quality
- **Better Debugging**: Enhanced error messages, build URL tracking, and automatic log retrieval

## Build Log Access

The workflow automatically fetches and displays build logs from TeamCity when a build completes:

### What Gets Displayed
- **Last 100 lines** of the build log
- **Build problems**: Compilation errors, warnings, and other build issues
- **Test failures**: Failed test cases with details
- **Build summary**: Build ID, status, platform, and direct links to full logs

### Accessing Logs
1. **In GitHub Actions**: Go to the Actions tab, select the workflow run, and view the "Fetch Build Logs" step
2. **Direct TeamCity URL**: Look for "Build log URL" or "Full logs" in the workflow output
3. **Build summary**: Check the "Report Build Status" step for a quick overview

### Blocking Behavior
The workflow will **fail and block PR merges** if TeamCity builds fail. This ensures that:
- ✅ Successful builds allow PR merges (green check)
- ❌ Failed builds block PR merges (red X)
- 📋 Build logs are always fetched for debugging, regardless of success or failure

This prevents merging broken code and maintains code quality standards.

## Merge Blocking via Commit Status

The workflow sets explicit **GitHub commit statuses** on every PR commit:

| Status Context | Description |
|----------------|-------------|
| `TeamCity CI (windows)` | Windows build result |
| `TeamCity CI (linux)` | Linux build result |

Each status transitions through `pending` → `success` / `failure` and includes a direct link to the TeamCity build log.

### Setting Up Required Status Checks (Branch Protection)

To enforce that PRs cannot be merged until both TeamCity builds pass:

1. Go to **Settings** → **Branches** → **Branch protection rules**
2. Click **Add rule** (or edit the rule for `main` / `master`)
3. Enable **Require status checks to pass before merging**
4. Search for and add these status checks:
   - `TeamCity CI (windows)`
   - `TeamCity CI (linux)`
5. (Optional) Enable **Require branches to be up to date before merging**
6. Click **Save changes**

Once configured, GitHub will show the status checks on every PR and block merging until both report `success`.

## Configuration

### Prerequisites

1. A TeamCity server with a configured build configuration for this project
2. A TeamCity access token with permissions to trigger builds and query build status

### Required GitHub Repository Variables

Configure the following variables in your GitHub repository settings (Settings → Secrets and variables → Actions → Variables tab):

| Variable Name | Description | Example |
|------------|-------------|---------|
| `TEAMCITY_URL` | Your TeamCity server URL (without trailing slash) | `https://teamcity.example.com` |
| `TEAMCITY_BUILD_TYPE_WINDOWS` | TeamCity build configuration ID for Windows pull requests | `ImtCore_Build_Windows` |
| `TEAMCITY_BUILD_TYPE_WINDOWS_MAIN` | TeamCity build configuration ID for Windows `main`/`master` pushes | `ImtCore_Build_Windows_Main` |
| `TEAMCITY_BUILD_TYPE_LINUX` | TeamCity build configuration ID for Linux pull requests | `ImtCore_Build_Linux` |
| `TEAMCITY_BUILD_TYPE_LINUX_MAIN` | TeamCity build configuration ID for Linux `main`/`master` pushes | `ImtCore_Build_Linux_Main` |

### Required GitHub Repository Secret

Configure this value in your GitHub repository settings (Settings → Secrets and variables → Actions → Secrets tab):

| Secret Name | Description |
|------------|-------------|
| `TEAMCITY_TOKEN` | TeamCity access token with permission to queue builds and read build results |

### How to Get TeamCity Configuration Values

#### 1. TeamCity Server URL
Your TeamCity server base URL, for example: `https://teamcity.example.com`

#### 2. TeamCity Access Token
1. Log into your TeamCity server
2. Go to your profile (click on your username in the top right)
3. Select **Access Tokens** from the menu
4. Click **Create access token**
5. Give it a name (e.g., "GitHub Actions Integration")
6. Set appropriate permissions (at minimum: trigger builds, view build status)
7. Copy the generated token

#### 3. TeamCity Build Configuration IDs
You need separate TeamCity build configurations for pull requests and `main`/`master` pushes on both Windows and Linux.

For each build configuration:
1. Open your build configuration in TeamCity
2. The build configuration ID is shown in the URL or in the build configuration settings
3. Format is usually: `ProjectId_BuildConfigurationId`
4. Examples:
  - Windows pull request: `ImtCore_CMakeBuild_Windows`
  - Windows `main`/`master`: `ImtCore_CMakeBuild_Windows_Main`
  - Linux pull request: `ImtCore_CMakeBuild_Linux`
  - Linux `main`/`master`: `ImtCore_CMakeBuild_Linux_Main`

## Setting Up GitHub Repository Variables

1. Go to your repository on GitHub
2. Click **Settings** → **Secrets and variables** → **Actions**
3. Click on the **Variables** tab
4. Click **New repository variable**
5. Add each of the five variables listed above:
   - `TEAMCITY_URL`
   - `TEAMCITY_BUILD_TYPE_WINDOWS`
   - `TEAMCITY_BUILD_TYPE_WINDOWS_MAIN`
   - `TEAMCITY_BUILD_TYPE_LINUX`
   - `TEAMCITY_BUILD_TYPE_LINUX_MAIN`
6. Click the **Secrets** tab and add `TEAMCITY_TOKEN` as a repository secret

## Workflow Triggers

The workflow triggers on:
- **Pull Requests**: Any pull request event (opened, synchronized, reopened)
- **Push to main/master**: Direct commits to main or master branches

Both Windows and Linux builds are triggered in parallel using a matrix strategy. Pull requests use `TEAMCITY_BUILD_TYPE_WINDOWS` and `TEAMCITY_BUILD_TYPE_LINUX`; `main`/`master` pushes use `TEAMCITY_BUILD_TYPE_WINDOWS_MAIN` and `TEAMCITY_BUILD_TYPE_LINUX_MAIN`.

## Build Information Passed to TeamCity

The workflow passes the following information to TeamCity:
- `branchName`: The Git branch name (e.g., `main`, `feature-branch`) - used by TeamCity for proper branch association
- `env.GIT_BRANCH`: The Git branch reference for backward compatibility
- `env.GIT_COMMIT`: The Git commit SHA

For pull requests, the workflow checks out the actual PR head SHA (not the merge commit) to ensure TeamCity builds exactly what was pushed to the PR branch.

## Timeout

The workflow has intelligent timeout handling with multiple layers:
- **Per-State Timeouts**: 
  - Queued State: Maximum 30 minutes (1800 seconds) - if the build stays queued longer, the workflow fails
  - Running State: Maximum 60 minutes (3600 seconds) - if the build runs longer, the workflow fails
- **Job-Level Timeout**: Overall 90 minutes at the GitHub Actions level (covers both queued and running time plus overhead)

These per-state timeouts help identify whether builds are stuck in queue or taking too long to execute. The total time a build can spend across all states is capped by the 90-minute job-level timeout.

You can adjust these by modifying the `MAX_QUEUED_SECONDS`, `MAX_RUNNING_SECONDS`, and `timeout-minutes` values in the workflow file.

## Troubleshooting

### "TeamCity configuration not found in repository variables"
- Ensure `TEAMCITY_URL` and all four build-type variables are configured under the Variables tab
- Ensure `TEAMCITY_TOKEN` is configured under the Secrets tab

### "Failed to trigger TeamCity build"
- Verify the TeamCity URL is correct and accessible
- Verify the access token has sufficient permissions
- Check that the build configuration ID exists and is correct
- Ensure the TeamCity server accepts REST API requests from GitHub Actions runners

### "Timeout waiting for TeamCity build to complete"
- **"Timed out: build stayed queued for Xs"**: The build waited in queue for more than 30 minutes
  - Check TeamCity agent availability
  - Ensure agents compatible with your build are running
- **"Timed out: build stayed running for Xs"**: The build ran for more than 60 minutes
  - Optimize your TeamCity build to complete faster
  - Or increase the `MAX_RUNNING_SECONDS` value in the workflow file
- **Job-level timeout (90min)**: Increase the `timeout-minutes` in the workflow file

### "TeamCity build failed"
- The build failed in TeamCity and the workflow has failed (PR is blocked)
- Check the "Fetch Build Logs" step in GitHub Actions for detailed error information
- Look for build problems and test failures in the workflow output
- Click on the "Build URL" link to view complete build logs in TeamCity
- Fix the build issues and push changes to retry
- The PR cannot be merged until builds pass

### "Build blocking PR merge"
- This is expected behavior when TeamCity builds fail
- Review the build logs in the "Fetch Build Logs" step
- Fix compilation errors, test failures, or other build issues
- Push fixes to trigger a new build
- Once builds pass, the PR can be merged

## Example TeamCity Build Configurations

You should have two separate TeamCity build configurations:

### Windows Build Configuration
- Configured to build on Windows agents
- Should include Windows-specific build steps (Visual Studio, MSVC compiler, etc.)
- VCS root pointing to the Git repository
- Optionally use QMake or CMake with Windows-specific settings

### Linux Build Configuration
- Configured to build on Linux agents
- Should include Linux-specific build steps (GCC/Clang compiler, make, etc.)
- VCS root pointing to the Git repository
- Optionally use QMake or CMake with Linux-specific settings

Both configurations:
1. Must be configured to build from the same Git repository
2. Should have the necessary build steps (CMake, QMake, etc.)
3. Can rely solely on GitHub Actions to trigger builds (no VCS triggers needed)
4. Should be configured to recognize branch names passed via the `branchName` parameter

## Security Notes

- Store your TeamCity token securely in GitHub repository variables
- Consider using GitHub Secrets instead of Variables if you need additional security for the token (the workflow supports both)
- Use a dedicated service account or token with minimal required permissions
- Consider using TeamCity's token expiration features
- Regularly rotate access tokens

## Technical Implementation Details

### JSON API Usage
The workflow uses TeamCity's JSON API instead of XML for better maintainability:
- Payloads are constructed using `jq` for safe JSON generation
- Avoids shell escaping issues common with XML
- Better error messages from TeamCity

### Error Handling
- Uses `curl --fail-with-body` to get response bodies even on HTTP errors
- Automatic retry with `--retry 5 --retry-delay 2 --retry-all-errors` for status checks
- Strict error handling with `set -euo pipefail` in all bash steps

### Matrix Strategy
Instead of duplicate jobs, the workflow uses a single job with a matrix:
```yaml
strategy:
  fail-fast: false
  matrix:
    platform: [windows, linux]
```
This reduces code duplication and makes the workflow easier to maintain.

## Support

For issues with:
- **GitHub Actions workflow**: Check the Actions tab in GitHub for detailed logs
- **TeamCity build**: Check the TeamCity web interface for build logs and configuration (URL shown in workflow logs)
- **Integration**: Ensure variables are correctly configured and TeamCity REST API is accessible
