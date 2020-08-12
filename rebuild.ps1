param(
    [switch]$development=$false
)

if (Test-Path _Bin -PathType Container) {
    rm -r -force _Bin/;
}

mkdir _Bin; cd _Bin;

$exec = 'cmake -G "Visual Studio 16 2019"'

if ($development) {
    $exec = "$exec -DDEVELOPMENT=ON"
}

$exec = "$exec ..;"

Invoke-Expression $exec

explorer .; cd ..;
