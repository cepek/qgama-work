# Improved script with assistance from OpenAI's ChatGPT (2023-10-28)
# Original script by Ales Cepek

param (
    [string]$ver = "2.06.X",  # release version
    [string]$rel = "",       # optional release directory
    [string]$windeployqt = "C:\Qt\6.2.2\msvc2019_64\bin\windeployqt.exe"
)

# Initialize an array to store error messages
$errors = @()

# Check if mandatory tools and paths exist
if (-Not (Test-Path -Path $windeployqt)) {
    $errors += "Program '$windeployqt' not found. Please check the path."
}

# Check if release directory exists
$releaseDir = Join-Path -Path $PWD -ChildPath "qgama-$ver"
if (Test-Path -Path $releaseDir) {
    $errors += "Directory '$releaseDir' already exists."
}

# Check if release archive exists
$releaseZip = Join-Path -Path $PWD -ChildPath "qgama-$ver.zip"
if (Test-Path -Path $releaseZip) {
    $errors += "File '$releaseZip' already exists."
}

# Display all errors
if ($errors.Count -gt 0) {
    $errors | ForEach-Object { Write-Host $_ }
    exit
}

# Create necessary directories
New-Item -ItemType Directory -Path $releaseDir | Out-Null
New-Item -ItemType Directory -Path (Join-Path -Path $releaseDir -ChildPath "bin") | Out-Null

# Copy necessary files
Copy-Item (Join-Path -Path $PWD -ChildPath "qgama.exe") -Destination $releaseDir
Copy-Item (Join-Path -Path $PWD -ChildPath "gama$rel\*.exe") -Destination (Join-Path -Path $releaseDir -ChildPath "bin")

# Run windeployqt
Invoke-Expression "$windeployqt $($releaseDir)\qgama.exe"

# Compress the archive
Compress-Archive -Path $releaseDir -DestinationPath $releaseZip

# Generate file hashes
Get-FileHash -Algorithm sha256 $releaseZip | Out-File (Join-Path -Path $PWD -ChildPath "qgama-$ver.sha256")
Get-FileHash -Algorithm md5 $releaseZip | Out-File (Join-Path -Path $PWD -ChildPath "qgama-$ver.md5")

Write-Host "`nScript execution completed successfully.`n"
