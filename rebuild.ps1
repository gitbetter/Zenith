if (Test-Path _Bin -PathType Container) {
    rm -r -force _Bin/;
}

mkdir _Bin; cd _Bin; cmake -G "Visual Studio 15 2017 Win64" ..; cd ..;
