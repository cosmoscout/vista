# ------------------------------------------------------------------------------------------------ #
#                                    This file is part of ViSTA                                    #
# ------------------------------------------------------------------------------------------------ #

<#
    .SYNOPSIS
    This function runs clang-format on all source files.

    .DESCRIPTION
    This function runs clang-format on all source files.
#>

$coreDir = "$PSScriptRoot/../VistaCoreLibs"
$demoDir = "$PSScriptRoot/../VistaDemo"

$fileEndings = @('*.cpp', '*.hpp', '*.h', '*.inl')

$itemsToCheck = Get-ChildItem -Path $coreDir, $demoDir -Recurse -Include $fileEndings

try {
    # If we have a recent PowerShell version we can run clang-format in parallel
    # which is much faster. But we still need to support PowerShell version 5.
    $parallelSupported = $PSVersionTable.PSVersion.Major -ge 7
    if ($parallelSupported) {
        $itemsToCheck | ForEach-Object -Parallel {
            $file = $_
            Write-Output "Formatting $file ..."
            clang-format -i "$file"
        }
    } else {
        $itemsToCheck | ForEach-Object {
            $file = $_
            Write-Output "Formatting $file ..."
            clang-format -i "$file"
        }
    }
} catch {
    throw $_
}