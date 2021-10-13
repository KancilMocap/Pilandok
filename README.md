# Pilandok

## Tested Compilers

- MSVC 2019
  - Windows SDK 10.0.19041.0
  - C++ ATL for latest v142 Build Tools (ARM64)
  - C++ ATL for latest v142 Build Tools (x86 & x64)
  - C++ MFC for latest v142 Build Tools (ARM64)
  - C++ MFC for latest v142 Build Tools (x86 & x64)

## Libraries Used

- [Cryptopp](https://github.com/weidai11/cryptopp) version 8.6.0, licensed under the Boost Software License 1.0 (See [https://github.com/weidai11/cryptopp/blob/master/License.txt](https://github.com/weidai11/cryptopp/blob/master/License.txt))

## Compilation Instructions

- Download the prebuilt static Cryptopp releases from [https://drive.google.com/drive/folders/1tlAOTFFkR6ZWTRoRkCFklAI86sZ3zNqc?usp=sharing](https://drive.google.com/drive/folders/1tlAOTFFkR6ZWTRoRkCFklAI86sZ3zNqc?usp=sharing) and place them in ./lib
- For MSVC 2019, the following error may occur while compiling the project: `LNK2038: mismatch detected for 'RuntimeLibrary': value 'MTd_StaticDebug' doesn't match value 'MDd_DynamicDebug'`
- If that happens, go into the build folder and replace all '/MD' with '/MT'
