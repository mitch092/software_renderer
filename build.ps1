#powershell.exe .\build.ps1
#powershell.exe .\build.ps1 -config
#powershell.exe .\build.ps1 -release
#powershell.exe .\build.ps1 -release -config

param (
    [switch]$release = $false,
    [switch]$config = $false
)

[string]$build_folder = "debug"
[string]$cmake_build = "Debug"
if($release){
    $build_folder = "release"
    $cmake_build = "RelWithDebInfo"
}
[string]$builds = "${PSScriptRoot}\builds\${build_folder}_msvc_ninja"

if($config){    
    Remove-Item $builds -Force -Recurse -ErrorAction SilentlyContinue
    New-Item -Path $builds -ItemType Directory
    Push-Location -Path $builds
    # -DLIBC=YES can be set in the CmakeLists.txt file itself. Forgot how to do this; it's done in the cmake cache files for LLVM/Clang.
    iex "cmake -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DCMAKE_BUILD_TYPE=${cmake_build} -DLIBC=YES -G `"Ninja`" ../../"
    Pop-Location
}

Push-Location -Path $builds
iex "cmake.exe --build ."
iex ".\software_rasterizer.exe"
Pop-Location
