@echo off
for /r %%i in (*.frag, *.vert) do %VULKAN_SDK%/Bin/glslangValidator.exe -V -o %%~nxi.spv %%i