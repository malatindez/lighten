$rootFolder = Resolve-Path '.\build\'

# Find all .vcxproj files within the root folder and its subfolders
$projectFiles = Get-ChildItem -Recurse -Filter *.vcxproj -Path $rootFolder

# List to store extracted include paths
$includePaths = @()

foreach ($file in $projectFiles) {
    $lines = Get-Content $file.FullName
    foreach ($line in $lines) {
        if ($line -match '<AdditionalIncludeDirectories>(.+?)<\/AdditionalIncludeDirectories>') {
            $includePathText = $Matches[1]
            $paths = $includePathText -split ';'

            foreach ($path in $paths) {
                $trimmedPath = $path.Trim()
                $isAbsolute = [System.IO.Path]::IsPathRooted($trimmedPath)
                
                if (-not $isAbsolute)
                {
                    try {
                        $basePath = $file.DirectoryName
                        $relativePath = $trimmedPath
                        $fullPath = Join-Path -Path $basePath -ChildPath $relativePath 
                        if (Test-Path $fullPath) {
                            $trimmedPath = Resolve-Path -Path $fullPath | Out-Null
                        } 
                    }
                    catch {

                    }
                }
                if ($trimmedPath -ne '') {
                    $includePaths += $trimmedPath
                }
            }
        }
    }
}

# Display extracted include paths
Write-Host "Extracted Include Paths:"
foreach ($path in $includePaths | Sort-Object -Unique) {
    Write-Host "  $path"
}
