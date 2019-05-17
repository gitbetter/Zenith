param(
    [switch]$development=$false,
    [switch]$demo=$false
)

if (Test-Path _Bin -PathType Container) {
    rm -r -force _Bin/;
}

mkdir _Bin; cd _Bin;

$exec = 'cmake -G "Visual Studio 15 2017 Win64"'

if ($development) {
    $exec = "$exec -DDEVELOPMENT=ON"
}

if ($demo) {
    $exec = "$exec -DDEMO=ON"
}

$exec = "$exec ..;"

Invoke-Expression $exec

explorer .; cd ..;
