# JWT Server Module for MTA:SA
[![Build Status](https://dev.azure.com/eXo-OpenSource/ml_jwt/_apis/build/status/eXo-OpenSource.ml_jwt?branchName=master)](https://dev.azure.com/eXo-OpenSource/ml_jwt/_build/latest?definitionId=1&branchName=master)

## Developing
1. Download and install the EditorConfig plugin for Visual Studio: [https://github.com/editorconfig/editorconfig-visualstudio#readme](https://github.com/editorconfig/editorconfig-visualstudio#readme)
2. Launch `premake.bat`
3. Open `Build/JWT.sln`

### Debugging
1. Create a directory junktion to MTA-Server directory in the root of the repo
    * `mklink /J mta-server "PATH_TO_INSTALL_DIR\MTA San Andreas 1.5\server"`
2. Create a symlink to the build output dll in the modules directory
    * `cd PATH_TO_INSTALL_DIR\MTA San Andreas 1.5\server\x64\modules` for x64
    * `cd PATH_TO_INSTALL_DIR\MTA San Andreas 1.5\server\mods\deathmatch\modules` for Win32
    * `mklink ml_jwt.dll "PATH_TO_REPO\Bin\Debug\ml_jwt.dll"`
3. Press `F5` in Visual Studio and the MTA-Server should start

## Installing
1. Put the resulting `Bin/ml_jwt.dll/.so` into your MTA:SA server `modules` folder.
2. Add `<module src="ml_jwt.dll"/>` (or `.so` for Linux) to your `mtaserver.conf`.
3. Start the server

## Depdencies
* libcrypto (openssl or compatible)
* libssl-dev (for the header files)

## API
### Function: `jwtSign`
```cpp
bool jwtSign(function(string/boolean) callback, table claims, string algorithm, string secret/public_key_path, string private_key_path)
```
* __callback:__ Lorem ipsum
* __claims:__ Lorem ipsum
* __algorithm:__ Lorem ipsum
* __secret/public_key_path__ Lorem ipsum
* __private_key_path__ Lorem ipsum

Returns __true__ if everything went fine, __false__ otherwise.

### Function: `jwtVerify`
```cpp
bool jwtVerify(function(boolean) callback, string token, string secret/public_key_path, bool is_file_path = false)
```
* __callback:__ Lorem ipsum
* __token:__ Lorem ipsum
* __secret/file_path__ Lorem ipsum
* __is_file_path__ Lorem ipsum

Returns __true__ if everything went fine, __false__ otherwise.

### Function: `jwtGetClaims`
```cpp
table jwtGetClaims(string token)
```
* __token:__ Lorem ipsum

Returns a table __containing the claims__.

## Download
via https://github.com/Contextualist/glare
* [Linux x64](https://glare.now.sh/eXo-OpenSource/ml_jwt/ml_jwt.so)
* [Windows x64](https://glare.now.sh/eXo-OpenSource/ml_jwt/ml_jwt_x64.dll)
* [Windows x86](https://glare.now.sh/eXo-OpenSource/ml_jwt/ml_jwt_win32.dll)

## Contributors
* Stefan K.
