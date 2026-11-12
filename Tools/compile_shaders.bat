@echo off
setlocal

set SHADERCROSS=%~dp0Shadercross\shadercross.exe
set INPUT=%~1
set OUTPUT=%~2


if not exist "%OUTPUT%" mkdir "%OUTPUT%"

echo Compiling shaders...

REM =========================
REM Vertex Shaders
REM =========================
for %%f in ("%INPUT%*.vert.hlsl") do (
    echo Compiling %%~nxf

    "%SHADERCROSS%" "%%f" -s HLSL -d SPIRV -t vertex -e main -o "%OUTPUT%%%~nf.spv"

    "%SHADERCROSS%" "%%f" -s HLSL -d DXIL -t vertex -e main -o "%OUTPUT%%%~nf.dxil"

    REM reflection metadata: sampler and uniform buffer counts for SDL_GPUShaderCreateInfo
    "%SHADERCROSS%" "%%f" -s HLSL -d JSON -t vertex -e main -I "%INPUT%" -o "%OUTPUT%%%~nf.json" || set FAILED=1

)

REM =========================
REM Fragment Shaders
REM =========================
for %%f in ("%INPUT%*.frag.hlsl") do (
    echo Compiling %%~nxf

    "%SHADERCROSS%" "%%f" -s HLSL -d SPIRV -t fragment -e main -o "%OUTPUT%%%~nf.spv"

    "%SHADERCROSS%" "%%f" -s HLSL -d DXIL -t fragment -e main -o "%OUTPUT%%%~nf.dxil"

    REM reflection metadata: sampler and uniform buffer counts for SDL_GPUShaderCreateInfo
    "%SHADERCROSS%" "%%f" -s HLSL -d JSON -t fragment -e main -I "%INPUT%" -o "%OUTPUT%%%~nf.json" || set FAILED=1
)

echo Shader compilation complete.