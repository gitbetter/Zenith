param(
    [switch]$development=$false
)

if (-not (Test-Path _Bin -PathType Container)) {
    if ($development) {
        powershell .\rebuild.ps1 -development;
    } else {
	powershell .\rebuild.ps1;
    }
} else {
    cd _Bin;
    if ($development) {
        cmake -G "Visual Studio 15 2017 Win64" -DDEVELOPMENT=ON ..;
    } else {
        cmake -G "Visual Studio 15 2017 Win64" ..;
    }
    cd ..;
}
