param(
    [switch]$development=$false,
    [switch]$headless=$false,
    [switch]$clean=$false,
    [string]$projectName
)

if (-Not (Test-Path Engine/_Bin -PathType Container)) {
	mkdir Engine/_Bin;
}

if ($clean) {
	rm -r -force Engine/_Bin/; mkdir Engine/_Bin;
}

cd Engine/_Bin;

$exec = 'cmake -G "Visual Studio 16 2019"'

if ($development) {
    $exec = "$exec -DDEVELOPMENT=ON"
}

if ($headless) {
	$exec = "$exec -DHEADLESS=ON"
}

if ($projectName) {
	$exec = "$exec -DUSER_PROJECT_NAME=$projectName"
}

Invoke-Expression "$exec ../..;"

if (Get-Command "devenv.com" -ErrorAction SilentlyContinue) {
	Invoke-Expression "devenv.com Zenith.sln /build"
}

if ($projectName) {
	cd ../../$projectName
    if (-Not (Test-Path ./Build -PathType Container)) {
    	mkdir Build; 
    }
    if ($clean) {
    	rm -r -force Build; mkdir Build;
    }
    cd Build;
    Invoke-Expression "$exec ../"
    explorer .; cd ..;
} else {
	explorer .; cd ../..;
}
