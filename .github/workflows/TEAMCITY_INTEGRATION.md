# GitHub Actions - TeamCity Integration

This GitHub Actions workflow integrates with an external TeamCity build server to trigger builds and report results back to GitHub.

## Overview

The `teamcity-trigger.yml` workflow:
1. Triggers a TeamCity build when pull requests are created/updated or commits are pushed to main/master
2. Waits for the TeamCity build to complete
3. Reports the build status back to GitHub (success/failure)

## Configuration

### Prerequisites

1. A TeamCity server with a configured build configuration for this project
2. A TeamCity access token with permissions to trigger builds and query build status

### Required GitHub Repository Variables

Configure the following variables in your GitHub repository settings (Settings → Secrets and variables → Actions → Variables tab):

| Variable Name | Description | Example |
|------------|-------------|---------|
| `TEAMCITY_URL` | Your TeamCity server URL (without trailing slash) | `https://teamcity.example.com` |
| `TEAMCITY_TOKEN` | TeamCity access token with build trigger permissions | `eyJ0eXAiOiJKV1Q...` |
| `TEAMCITY_BUILD_TYPE` | TeamCity build configuration ID | `ImtCore_Build` or `ProjectId_BuildConfigId` |

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

#### 3. TeamCity Build Configuration ID
1. Open your build configuration in TeamCity
2. The build configuration ID is shown in the URL or in the build configuration settings
3. Format is usually: `ProjectId_BuildConfigurationId`
4. Example: `ImtCore_CMakeBuild` or `ImagingTools_ImtCore_Build`

## Setting Up GitHub Repository Variables

1. Go to your repository on GitHub
2. Click **Settings** → **Secrets and variables** → **Actions**
3. Click on the **Variables** tab
4. Click **New repository variable**
5. Add each of the three variables listed above

## Workflow Triggers

The workflow triggers on:
- **Pull Requests**: Any pull request event (opened, synchronized, reopened)
- **Push to main/master**: Direct commits to main or master branches

## Build Information Passed to TeamCity

The workflow passes the following information to TeamCity:
- `env.GIT_BRANCH`: The Git branch reference (e.g., `refs/heads/main` or `refs/pull/123/merge`)
- `env.GIT_COMMIT`: The Git commit SHA

You can access these in your TeamCity build configuration if needed.

## Timeout

The workflow will wait up to 1 hour (3600 seconds) for the TeamCity build to complete. If the build takes longer, the workflow will timeout and fail.

You can adjust this by modifying the `MAX_WAIT` variable in the workflow file.

## Troubleshooting

### "TeamCity configuration not found in repository variables"
- Ensure all three required variables (TEAMCITY_URL, TEAMCITY_TOKEN, TEAMCITY_BUILD_TYPE) are configured in your repository settings under the Variables tab

### "Failed to trigger TeamCity build"
- Verify the TeamCity URL is correct and accessible
- Verify the access token has sufficient permissions
- Check that the build configuration ID exists and is correct
- Ensure the TeamCity server accepts REST API requests from GitHub Actions runners

### "Timeout waiting for TeamCity build to complete"
- The build is taking longer than 1 hour
- Increase the `MAX_WAIT` value in the workflow file
- Or optimize your TeamCity build to complete faster

### "TeamCity build failed"
- The build failed in TeamCity
- Click on the "Build URL" link in the GitHub Actions log to view detailed build logs in TeamCity
- Fix the build issues in TeamCity and retry

## Example TeamCity Build Configuration

Your TeamCity build configuration should:
1. Be configured to build from the Git repository
2. Have the necessary build steps (CMake, QMake, etc.)
3. Have appropriate VCS triggers or rely solely on GitHub Actions to trigger builds

## Security Notes

- Store your TeamCity token securely in GitHub repository variables
- Consider using GitHub Secrets instead of Variables if you need additional security for the token (the workflow supports both)
- Use a dedicated service account or token with minimal required permissions
- Consider using TeamCity's token expiration features
- Regularly rotate access tokens

## Support

For issues with:
- **GitHub Actions workflow**: Check the Actions tab in GitHub for detailed logs
- **TeamCity build**: Check the TeamCity web interface for build logs and configuration
- **Integration**: Ensure variables are correctly configured and TeamCity REST API is accessible
