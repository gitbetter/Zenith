param(
    [switch]$development=$false
    [switch]$demo=$false
)

if (-not (Test-Path _Bin -PathType Container)) {
    $exec = 'powershell .\rebuild.ps1'
    if ($development) {
	$exec = "$exec -development"
    }
    if ($demo) {
	$exec = "$exec -demo"
    }
    Invoke-Expression $exec
} else {
    cd _Bin;
    $exec = 'cmake -G "Visual Studio 15 2017 Win64"'
    if ($development) {
        $exec = "$exec -DDEVELOPMENT=ON"
    } 
    if ($demo) {
	$exec = "$exec -DDEMO=ON"
    }
    $exec = "$exec ..;"
    Invoke-Expression $exec
    cd ..;
}
