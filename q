[33mcommit 7d3b398f905cc1d87bd93365dbc846f9fdb415a0[m
Author: Craftrect <craft.rect@gmail.com>
Date:   Sat Sep 28 21:35:57 2013 +0200

    Initial commit

[1mdiff --git a/RectanguLand.Common/RectanguLand.Common.vcxproj b/RectanguLand.Common/RectanguLand.Common.vcxproj[m
[1mnew file mode 100644[m
[1mindex 0000000..17b20e2[m
[1m--- /dev/null[m
[1m+++ b/RectanguLand.Common/RectanguLand.Common.vcxproj[m
[36m@@ -0,0 +1,79 @@[m
[32m+[m[32m﻿<?xml version="1.0" encoding="utf-8"?>[m
[32m+[m[32m<Project DefaultTargets="Build" ToolsVersion="12.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">[m
[32m+[m[32m  <ItemGroup Label="ProjectConfigurations">[m
[32m+[m[32m    <ProjectConfiguration Include="Debug|Win32">[m
[32m+[m[32m      <Configuration>Debug</Configuration>[m
[32m+[m[32m      <Platform>Win32</Platform>[m
[32m+[m[32m    </ProjectConfiguration>[m
[32m+[m[32m    <ProjectConfiguration Include="Release|Win32">[m
[32m+[m[32m      <Configuration>Release</Configuration>[m
[32m+[m[32m      <Platform>Win32</Platform>[m
[32m+[m[32m    </ProjectConfiguration>[m
[32m+[m[32m  </ItemGroup>[m
[32m+[m[32m  <PropertyGroup Label="Globals">[m
[32m+[m[32m    <ProjectGuid>{3F94F4C4-DAC0-4B56-B9BA-79FD5269C8B5}</ProjectGuid>[m
[32m+[m[32m    <RootNamespace>RectanguLandCommon</RootNamespace>[m
[32m+[m[32m    <SccProjectName>SAK</SccProjectName>[m
[32m+[m[32m    <SccAuxPath>SAK</SccAuxPath>[m
[32m+[m[32m    <SccLocalPath>SAK</SccLocalPath>[m
[32m+[m[32m    <SccProvider>SAK</SccProvider>[m
[32m+[m[32m  </PropertyGroup>[m
[32m+[m[32m  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />[m
[32m+[m[32m  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">[m
[32m+[m[32m    <ConfigurationType>Utility</ConfigurationType>[m
[32m+[m[32m    <UseDebugLibraries>true</UseDebugLibraries>[m
[32m+[m[32m    <PlatformToolset>v120_CTP_Nov2012</PlatformToolset>[m
[32m+[m[32m    <CharacterSet>Unicode</CharacterSet>[m
[32m+[m[32m  </PropertyGroup>[m
[32m+[m[32m  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">[m
[32m+[m[32m    <ConfigurationType>Application</ConfigurationType>[m
[32m+[m[32m    <UseDebugLibraries>false</UseDebugLibraries>[m
[32m+[m[32m    <PlatformToolset>v120</PlatformToolset>[m
[32m+[m[32m    <WholeProgramOptimization>true</WholeProgramOptimization>[m
[32m+[m[32m    <CharacterSet>MultiByte</CharacterSet>[m
[32m+[m[32m  </PropertyGroup>[m
[32m+[m[32m  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />[m
[32m+[m[32m  <ImportGroup Label="ExtensionSettings">[m
[32m+[m[32m  </ImportGroup>[m
[32m+[m[32m  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">[m
[32m+[m[32m    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />[m
[32m+[m[32m  </ImportGroup>[m
[32m+[m[32m  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">[m
[32m+[m[32m    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />[m
[32m+[m[32m  </ImportGroup>[m
[32m+[m[32m  <PropertyGroup Label="UserMacros" />[m
[32m+[m[32m  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">[m
[32m+[m[32m    <IncludePath>$(SolutionDir)/dep/include/;$(SolutionDir)/src/$(ProjectName);$(IncludePath)</IncludePath>[m
[32m+[m[32m  </PropertyGroup>[m
[32m+[m[32m  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">[m
[32m+[m[32m    <ClCompile>[m
[32m+[m[32m      <WarningLevel>Level3</WarningLevel>[m
[32m+[m[32m      <Optimization>Disabled</Optimization>[m
[32m+[m[32m      <SDLCheck>true</SDLCheck>[m
[32m+[m[32m    </ClCompile>[m
[32m+[m[32m    <Link>[m
[32m+[m[32m      <GenerateDebugInformation>true</GenerateDebugInformation>[m
[32m+[m[32m    </Link>[m
[32m+[m[32m  </ItemDefinitionGroup>[m
[32m+[m[32m  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">[m
[32m+[m[32m    <ClCompile>[m
[32m+[m[32m      <WarningLevel>Level3</WarningLevel>[m
[32m+[m[32m      <Optimization>MaxSpeed</Optimization>[m
[32m+[m[32m      <FunctionLevelLinking>true</FunctionLevelLinking>[m
[32m+[m[32m      <IntrinsicFunctions>true</IntrinsicFunctions>[m
[32m+[m[32m      <SDLCheck>true</SDLCheck>[m
[32m+[m[32m    </ClCompile>[m
[32m+[m[32m    <Link>[m
[32m+[m[32m      <GenerateDebugInformation>true</GenerateDebugInformation>[m
[32m+[m[32m      <EnableCOMDATFolding>true</EnableCOMDATFolding>[m
[32m+[m[32m      <OptimizeReferences>true</OptimizeReferences>[m
[32m+[m[32m    </Link>[m
[32m+[m[32m  </ItemDefinitionGroup>[m
[32m+[m[32m  <ItemGroup>[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Common\ComPtr.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Common\Precompiled.h" />[m
[32m+[m[32m  </ItemGroup>[m
[32m+[m[32m  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />[m
[32m+[m[32m  <ImportGroup Label="ExtensionTargets">[m
[32m+[m[32m  </ImportGroup>[m
[32m+[m[32m</Project>[m
\ No newline at end of file[m
[1mdiff --git a/RectanguLand.Common/RectanguLand.Common.vcxproj.filters b/RectanguLand.Common/RectanguLand.Common.vcxproj.filters[m
[1mnew file mode 100644[m
[1mindex 0000000..6e96f56[m
[1m--- /dev/null[m
[1m+++ b/RectanguLand.Common/RectanguLand.Common.vcxproj.filters[m
[36m@@ -0,0 +1,7 @@[m
[32m+[m[32m﻿<?xml version="1.0" encoding="utf-8"?>[m
[32m+[m[32m<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">[m
[32m+[m[32m  <ItemGroup>[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Common\Precompiled.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Common\ComPtr.h" />[m
[32m+[m[32m  </ItemGroup>[m
[32m+[m[32m</Project>[m
\ No newline at end of file[m
[1mdiff --git a/RectanguLand.Common/RectanguLand.Common.vcxproj.user b/RectanguLand.Common/RectanguLand.Common.vcxproj.user[m
[1mnew file mode 100644[m
[1mindex 0000000..ef5ff2a[m
[1m--- /dev/null[m
[1m+++ b/RectanguLand.Common/RectanguLand.Common.vcxproj.user[m
[36m@@ -0,0 +1,4 @@[m
[32m+[m[32m﻿<?xml version="1.0" encoding="utf-8"?>[m
[32m+[m[32m<Project ToolsVersion="12.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">[m
[32m+[m[32m  <PropertyGroup />[m
[32m+[m[32m</Project>[m
\ No newline at end of file[m
[1mdiff --git a/RectanguLand.Content/RectanguLand.Content.vcxproj b/RectanguLand.Content/RectanguLand.Content.vcxproj[m
[1mnew file mode 100644[m
[1mindex 0000000..2a12c7e[m
[1m--- /dev/null[m
[1m+++ b/RectanguLand.Content/RectanguLand.Content.vcxproj[m
[36m@@ -0,0 +1,126 @@[m
[32m+[m[32m﻿<?xml version="1.0" encoding="utf-8"?>[m
[32m+[m[32m<Project DefaultTargets="Build" ToolsVersion="12.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">[m
[32m+[m[32m  <ItemGroup Label="ProjectConfigurations">[m
[32m+[m[32m    <ProjectConfiguration Include="Debug|Win32">[m
[32m+[m[32m      <Configuration>Debug</Configuration>[m
[32m+[m[32m      <Platform>Win32</Platform>[m
[32m+[m[32m    </ProjectConfiguration>[m
[32m+[m[32m    <ProjectConfiguration Include="Release|Win32">[m
[32m+[m[32m      <Configuration>Release</Configuration>[m
[32m+[m[32m      <Platform>Win32</Platform>[m
[32m+[m[32m    </ProjectConfiguration>[m
[32m+[m[32m  </ItemGroup>[m
[32m+[m[32m  <PropertyGroup Label="Globals">[m
[32m+[m[32m    <ProjectGuid>{65FD5CFB-939A-4E6B-AEF7-58704692DF33}</ProjectGuid>[m
[32m+[m[32m    <SccProjectName>SAK</SccProjectName>[m
[32m+[m[32m    <SccAuxPath>SAK</SccAuxPath>[m
[32m+[m[32m    <SccLocalPath>SAK</SccLocalPath>[m
[32m+[m[32m    <SccProvider>SAK</SccProvider>[m
[32m+[m[32m    <RootNamespace>RectanguLandContent</RootNamespace>[m
[32m+[m[32m  </PropertyGroup>[m
[32m+[m[32m  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />[m
[32m+[m[32m  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">[m
[32m+[m[32m    <ConfigurationType>StaticLibrary</ConfigurationType>[m
[32m+[m[32m    <UseDebugLibraries>true</UseDebugLibraries>[m
[32m+[m[32m    <PlatformToolset>v120_CTP_Nov2012</PlatformToolset>[m
[32m+[m[32m    <CharacterSet>Unicode</CharacterSet>[m
[32m+[m[32m  </PropertyGroup>[m
[32m+[m[32m  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">[m
[32m+[m[32m    <ConfigurationType>StaticLibrary</ConfigurationType>[m
[32m+[m[32m    <UseDebugLibraries>false</UseDebugLibraries>[m
[32m+[m[32m    <PlatformToolset>v120_CTP_Nov2012</PlatformToolset>[m
[32m+[m[32m    <WholeProgramOptimization>true</WholeProgramOptimization>[m
[32m+[m[32m    <CharacterSet>Unicode</CharacterSet>[m
[32m+[m[32m  </PropertyGroup>[m
[32m+[m[32m  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />[m
[32m+[m[32m  <ImportGroup Label="ExtensionSettings">[m
[32m+[m[32m  </ImportGroup>[m
[32m+[m[32m  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">[m
[32m+[m[32m    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />[m
[32m+[m[32m  </ImportGroup>[m
[32m+[m[32m  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">[m
[32m+[m[32m    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />[m
[32m+[m[32m  </ImportGroup>[m
[32m+[m[32m  <PropertyGroup Label="UserMacros" />[m
[32m+[m[32m  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">[m
[32m+[m[32m    <IncludePath>$(SolutionDir)/dep/include/;$(SolutionDir)/src/;$(IncludePath)</IncludePath>[m
[32m+[m[32m    <LibraryPath>$(SolutionDir)/dep/lib/$(Configuration)/$(PlatformTarget);$(LibraryPath)</LibraryPath>[m
[32m+[m[32m    <OutDir>$(SolutionDir)\lib\$(Configuration)\</OutDir>[m
[32m+[m[32m  </PropertyGroup>[m
[32m+[m[32m  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">[m
[32m+[m[32m    <IncludePath>$(SolutionDir)/dep/include/;$(SolutionDir)/src/;$(IncludePath)</IncludePath>[m
[32m+[m[32m    <LibraryPath>$(SolutionDir)/dep/lib/$(Configuration)/$(PlatformTarget);$(LibraryPath)</LibraryPath>[m
[32m+[m[32m    <OutDir>$(SolutionDir)\lib\$(Configuration)\</OutDir>[m
[32m+[m[32m  </PropertyGroup>[m
[32m+[m[32m  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">[m
[32m+[m[32m    <ClCompile>[m
[32m+[m[32m      <WarningLevel>Level3</WarningLevel>[m
[32m+[m[32m      <Optimization>Disabled</Optimization>[m
[32m+[m[32m      <SDLCheck>true</SDLCheck>[m
[32m+[m[32m      <AdditionalIncludeDirectories>$(SolutionDir)\src\$(ProjectName)</AdditionalIncludeDirectories>[m
[32m+[m[32m      <PrecompiledHeaderFile>Precompiled.h</PrecompiledHeaderFile>[m
[32m+[m[32m      <PrecompiledHeader>Use</PrecompiledHeader>[m
[32m+[m[32m      <AdditionalOptions>/Zm110 %(AdditionalOptions)</AdditionalOptions>[m
[32m+[m[32m    </ClCompile>[m
[32m+[m[32m    <Link>[m
[32m+[m[32m      <GenerateDebugInformation>true</GenerateDebugInformation>[m
[32m+[m[32m    </Link>[m
[32m+[m[32m    <Lib>[m
[32m+[m[32m      <AdditionalDependencies>zip.lib</AdditionalDependencies>[m
[32m+[m[32m    </Lib>[m
[32m+[m[32m  </ItemDefinitionGroup>[m
[32m+[m[32m  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">[m
[32m+[m[32m    <ClCompile>[m
[32m+[m[32m      <WarningLevel>Level3</WarningLevel>[m
[32m+[m[32m      <Optimization>MaxSpeed</Optimization>[m
[32m+[m[32m      <FunctionLevelLinking>true</FunctionLevelLinking>[m
[32m+[m[32m      <IntrinsicFunctions>true</IntrinsicFunctions>[m
[32m+[m[32m      <SDLCheck>true</SDLCheck>[m
[32m+[m[32m      <PrecompiledHeaderFile>precompiled.h</PrecompiledHeaderFile>[m
[32m+[m[32m      <PrecompiledHeader>Use</PrecompiledHeader>[m
[32m+[m[32m      <AdditionalOptions>/Zm110 %(AdditionalOptions)</AdditionalOptions>[m
[32m+[m[32m      <AdditionalIncludeDirectories>$(SolutionDir)\src\$(ProjectName)</AdditionalIncludeDirectories>[m
[32m+[m[32m    </ClCompile>[m
[32m+[m[32m    <Link>[m
[32m+[m[32m      <GenerateDebugInformation>true</GenerateDebugInformation>[m
[32m+[m[32m      <EnableCOMDATFolding>true</EnableCOMDATFolding>[m
[32m+[m[32m      <OptimizeReferences>true</OptimizeReferences>[m
[32m+[m[32m    </Link>[m
[32m+[m[32m    <Lib>[m
[32m+[m[32m      <AdditionalDependencies>zip.lib</AdditionalDependencies>[m
[32m+[m[32m    </Lib>[m
[32m+[m[32m  </ItemDefinitionGroup>[m
[32m+[m[32m  <ItemGroup>[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\AssetManager.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\dds.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\EffectCollection.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\FileSystemResourceLocation.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\Loader\ImageLoader.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\Loader\TextureLoader.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\Loader\TextureLoaderD3D11.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\Precompiled.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\ResourceLocation.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\ResourceManager.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\TextureManager.h" />[m
[32m+[m[32m    <ClInclude Include="..\src\RectanguLand.Content\ZipResourceLocation.h" />[m
[32m+[m[32m  </ItemGroup>[m
[32m+[m[32m  <ItemGroup>[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\AssetManager.cpp" />[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\EffectCollection.cpp" />[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\FileSystemResourceLocation.cpp" />[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\Loader\ImageLoader.cpp" />[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\Loader\TextureLoader.cpp" />[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\Loader\TextureLoaderD3D11.cpp" />[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\Precompiled.cpp">[m
[32m+[m[32m      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">Create</PrecompiledHeader>[m
[32m+[m[32m      <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">Create</PrecompiledHeader>[m
[32m+[m[32m    </ClCompile>[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\ResourceLocation.cpp" />[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\ResourceManager.cpp" />[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\TextureManager.cpp" />[m
[32m+[m[32m    <ClCompile Include="..\src\RectanguLand.Content\ZipResourceLocation.cpp" />[m
[32m+[m[32m  </ItemGroup>[m
[32m+[m[32m  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />[m
[32m+[m[32m  <ImportGroup Label="ExtensionTargets">[m
[32m+[m[32m  </ImportGroup>[m
[32m+[m[32m</Project>[m
\ No newline at end of file[m
[1mdiff --git a/RectanguLand.Content/RectanguLand.Content.vcxproj.filters b/RectanguLand.Content/RectanguLand.Content.vc