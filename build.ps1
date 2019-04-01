if (-not (Test-Path _Bin -PathType Container)) {
    powershell .\rebuild.ps1;
} else {
    cd _Bin; cmake -G "Visual Studio 15 2017 Win64" ..; cd ..;
}
