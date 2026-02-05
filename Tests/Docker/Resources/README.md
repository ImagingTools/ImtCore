# Example Resources Directory

This directory contains example resource files to demonstrate how to use the resources folder.

## Files in this directory:

### example-config.json
A sample JSON configuration file demonstrating how configuration files can be stored and referenced from startup scripts.

**Usage in scripts:**
```bash
# Linux
if [ -f "/app/custom-apps/resources/example-config.json" ]; then
    cp /app/custom-apps/resources/example-config.json /opt/myapp/config/config.json
fi
```

```powershell
# Windows
$configFile = "C:\app\custom-apps\resources\example-config.json"
if (Test-Path $configFile) {
    Copy-Item $configFile "C:\Program Files\MyApp\config\config.json"
}
```

### example-setup.sql
A sample SQL script demonstrating how database scripts can be stored and executed from installer scripts.

**Usage in scripts:**
```bash
# Linux
if [ -f "/app/custom-apps/resources/example-setup.sql" ]; then
    psql -U postgres -d mydb -f /app/custom-apps/resources/example-setup.sql
fi
```

```powershell
# Windows
$sqlScript = "C:\app\custom-apps\resources\example-setup.sql"
if (Test-Path $sqlScript) {
    & "C:\Program Files\PostgreSQL\16\bin\psql.exe" -U postgres -d mydb -f $sqlScript
}
```

## What to place in resources/:

1. **Installers:**
   - `.exe` files (Windows installers)
   - `.msi` files (Windows installers)
   - `.run` files (Linux installers)
   - `.deb`, `.rpm` packages
   - `.tar.gz`, `.zip` archives

2. **SQL Scripts:**
   - Database setup scripts
   - Migration scripts
   - Seed data scripts

3. **Configuration Files:**
   - `.json`, `.yaml`, `.xml` configuration files
   - `.ini`, `.conf` config files
   - Environment files

4. **Data Files:**
   - Test data files
   - Reference data
   - Any other files your application needs

## Important Notes:

- Files in the `resources/` directory are copied into the Docker image during build
- They are accessible at runtime from your installer and startup scripts
- Keep the `.gitkeep` file to ensure the directory structure is preserved in git
- Large binary files should be added to `.gitignore` if not needed in version control
