param(
    [switch]$development=$false
)

if (-not (Test-Path _Bin -PathType Container)) {
    $exec = 'powershell .\rebuild.ps1'
    if ($development) {
	$exec = "$exec -development"
    }
    Invoke-Expression $exec
} else {
    cd _Bin;
    $exec = 'cmake -G "Visual Studio 16 2019"'
    if ($development) {
        $exec = "$exec -DDEVELOPMENT=ON"
    }
    $exec = "$exec ..;"
    Invoke-Expression $exec
    cd ..;
}
