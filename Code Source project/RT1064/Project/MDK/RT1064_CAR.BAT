SET PATH=C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin;C:\Program Files\Microsoft MPI\Bin\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.1\bin;C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.1\libnvvp;C:\Program Files (x86)\VMware\VMware Workstation\bin\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\Bandizip\;C:\Program Files\dotnet\;C:\Program Files\NVIDIA Corporation\NVIDIA NvDLISR;C:\Program Files\NVIDIA Corporation\Nsight Compute 2023.1.0\;C:\mingw64\bin;C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\170\Tools\Binn\;C:\Program Files (x86)\Microsoft SQL Server\150\Tools\Binn\;C:\Program Files\Microsoft SQL Server\150\Tools\Binn\;C:\Program Files\Microsoft SQL Server\150\DTS\Binn\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files\MATLAB\R2023a\bin;C:\Program Files\Git\cmd;C:\Program Files\PowerShell\7\;C:\Program Files\Keil_v5\ARM\GCC\bin;C:\Users\JIERUI\AppData\Local\Programs\Python\Python311\Scripts\;C:\Users\JIERUI\AppData\Local\Programs\Python\Python311\;C:\Users\JIERUI\AppData\Local\Microsoft\WindowsApps;C:\Users\JIERUI\AppData\Local\Programs\Microsoft VS Code Insiders\bin;C:\Users\JIERUI\AppData\Local\JetBrains\Toolbox\scripts;C:\intelFPGA\18.0\modelsim_ase\win32aloem;C:\intelFPGA\18.0\modelsim_ae\win32aloem;C:\MentorGraphics\modeltech64_2020.4\win64;C:\MentorGraphics\PADSVX.2.13\SDD_HOME\CAMCAD;
SET CPU_TYPE=MIMXRT1064DVL6A
SET CPU_VENDOR=NXP
SET UV2_TARGET=RT1064_CAR
SET CPU_CLOCK=0x00B71B00
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\isr.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\main.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\servo.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\motor.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\encoder.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\camera.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\image.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\buzzer.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\schedule.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\display.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\mymath.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\pid.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\control.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\filter.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\zf_common_clock.__i"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via ".\Objects\RT1064_CAR.lnp"
"C:\Program Files\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" ".\Objects\RT1064_CAR.axf" --i32combined --output ".\Objects\RT1064_CAR.hex"
