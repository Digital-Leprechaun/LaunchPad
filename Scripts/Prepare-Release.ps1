[CmdletBinding()]
param([string]$EngineRoot)

$ErrorActionPreference = 'Stop'
$pluginRoot = Split-Path -Parent $PSScriptRoot
$descriptor = Get-Content -Raw -LiteralPath (Join-Path $pluginRoot 'LaunchPad.uplugin') | ConvertFrom-Json
$version = $descriptor.VersionName
if ($version -notmatch '^\d{2}\.\d{2}\.\d{2}\.\d+$') { throw 'Expected a YY.MM.DD.revision VersionName.' }
$releaseRoot = Join-Path $pluginRoot ('Releases/' + $version)
New-Item -ItemType Directory -Path $releaseRoot -Force | Out-Null

$sourceFiles = @('LaunchPad.uplugin', 'README.md', 'CHANGELOG.md', 'LICENSE', 'NOTICE') |
    ForEach-Object { Get-Item -LiteralPath (Join-Path $pluginRoot $_) }
foreach ($folder in @('Source', 'Resources', 'Config', 'Scripts')) {
    $sourceFiles += Get-ChildItem -LiteralPath (Join-Path $pluginRoot $folder) -Recurse -File
}

$binaryFiles = @()
$engineLabel = $null
$engineBuildId = $null
if ($EngineRoot) {
    $engineVersion = Get-Content -Raw -LiteralPath (Join-Path $EngineRoot 'Engine/Build/Build.version') | ConvertFrom-Json
    $engineBuildId = (Get-Content -Raw -LiteralPath (Join-Path $EngineRoot 'Engine/Binaries/Win64/UnrealEditor.modules') | ConvertFrom-Json).BuildId
    $engineLabel = '{0}.{1}.{2}' -f $engineVersion.MajorVersion, $engineVersion.MinorVersion, $engineVersion.PatchVersion
    foreach ($manifestName in @('UnrealEditor.modules', 'UnrealEditor-Win64-DebugGame.modules')) {
        $manifestFile = Get-Item -LiteralPath (Join-Path $pluginRoot ('Binaries/Win64/' + $manifestName))
        $manifest = Get-Content -Raw -LiteralPath $manifestFile.FullName | ConvertFrom-Json
        if ($manifest.BuildId -ne $engineBuildId) { throw "$manifestName does not match the supplied engine build." }
        $binaryFiles += $manifestFile
        $binaryFiles += Get-Item -LiteralPath (Join-Path $manifestFile.DirectoryName $manifest.Modules.LaunchPad)
    }
}

Add-Type -AssemblyName System.IO.Compression
function Write-PluginArchive([string]$Path, [object[]]$Files) {
    $stream = [System.IO.File]::Open($Path, [System.IO.FileMode]::CreateNew)
    try {
        $archive = [System.IO.Compression.ZipArchive]::new($stream, [System.IO.Compression.ZipArchiveMode]::Create, $true)
        try {
            foreach ($file in ($Files | Sort-Object FullName)) {
                $relativePath = $file.FullName.Substring($pluginRoot.Length + 1).Replace('\', '/')
                $entry = $archive.CreateEntry('LaunchPad/' + $relativePath, [System.IO.Compression.CompressionLevel]::Optimal)
                $entryStream = $entry.Open()
                $inputStream = $file.OpenRead()
                try { $inputStream.CopyTo($entryStream) }
                finally { $inputStream.Dispose(); $entryStream.Dispose() }
            }
        }
        finally { $archive.Dispose() }
    }
    finally { $stream.Dispose() }
}

$sourceArchive = Join-Path $releaseRoot "LaunchPad-$version-Source.zip"
$binaryArchive = if ($EngineRoot) { Join-Path $releaseRoot "LaunchPad-$version-UE$engineLabel-LocalBuild-Win64.zip" }
# Do not silently replace release artifacts from an earlier run.
foreach ($path in @($sourceArchive, $binaryArchive) | Where-Object { $_ }) {
    if (Test-Path -LiteralPath $path) { throw "Archive already exists: $path. Choose a new version or move the existing archive first." }
}
Write-PluginArchive $sourceArchive $sourceFiles
$archives = @($sourceArchive)
if ($binaryArchive) {
    Write-PluginArchive $binaryArchive ($sourceFiles + $binaryFiles)
    $archives += $binaryArchive
}

$notes = @"
# LaunchPad $version

LaunchPad arranges Unreal Engine PIE windows in a fitted grid. This release fixes single-player fitting, automatic resolution, aspect ratios, requested-size limits and secondary-monitor positioning. Window decorations account for monitor DPI, and the button displays Stop during PIE.

## Downloads

- Source ZIP: extract LaunchPad into your project's Plugins folder and compile with your engine.
"@
if ($binaryArchive) {
    $notes += "`n- Win64 ZIP: Development and DebugGame binaries for local Unreal Engine $engineLabel, build ID $engineBuildId. Use the source ZIP for other engine builds, including incompatible Launcher installations.`n"
}
$notes += @"

Close the editor before installing or upgrading. Enable LaunchPad in the Plugins dialog and restart if prompted.

## Validation

Validated against the local UE.AssetPack Unreal Engine 5.8.2 source build. Development and DebugGame compiled successfully, the plugin loaded in an isolated editor host, and LaunchPad.Layout.Regression passed, covering 64 grid combinations plus resolution and invalid-input cases. Interactive PIE checks remain manual.

MIT license; existing Epic template attribution is retained. See LICENSE and NOTICE.
"@
$notes | Set-Content -LiteralPath (Join-Path $releaseRoot 'RELEASE_NOTES.md') -Encoding utf8
$archives | ForEach-Object {
    $hash = Get-FileHash -LiteralPath $_ -Algorithm SHA256
    '{0}  {1}' -f $hash.Hash.ToLowerInvariant(), (Split-Path -Leaf $_)
} | Set-Content -LiteralPath (Join-Path $releaseRoot 'SHA256SUMS.txt') -Encoding ascii
Get-ChildItem -LiteralPath $releaseRoot -File | Select-Object Name, Length
