param(
    [switch]$development=$false
)

if (Test-Path _Bin -PathType Container) {
    rm -r -force _Bin/;
}

mkdir _Bin; cd _Bin; 
if ($development) {
    cmake -G "Visual Studio 15 2017 Win64" -DDEVELOPMENT=ON ..;
} else {
    cmake -G "Visual Studio 15 2017 Win64" ..;
}

explorer .; cd ..;
