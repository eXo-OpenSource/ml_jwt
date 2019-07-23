# JWT Server Module for MTA:SA
[![Build Status](https://dev.azure.com/eXo-OpenSource/ml_jwt/_apis/build/status/eXo-OpenSource.ml_jwt?branchName=master)](https://dev.azure.com/eXo-OpenSource/ml_jwt/_build/latest?definitionId=1&branchName=master)
## Developing
1. Download and install the EditorConfig plugin for Visual Studio: [https://github.com/editorconfig/editorconfig-visualstudio#readme](https://github.com/editorconfig/editorconfig-visualstudio#readme)
2. Launch `premake.bat`
3. Open `Build/JWT.sln`

## Installing
1. Put the resulting `Bin/ml_jwt.dll/.so` into your MTA:SA server `modules` folder.
2. Add `<module src="ml_jwt.dll"/>` (or `.so` for Linux) to your `mtaserver.conf`.
3. Start the server

## API
### Function: `jwtSign`
```cpp
bool jwtSign(function(string/boolean) callback, table claims, string algorithm, string secret)
```
* __callback:__ Lorem ipsum
* __claims:__ Lorem ipsum
* __algorithm:__ Lorem ipsum
* __secret__ Lorem ipsum

Returns the __true__ if everything went fine, __false__ otherwise.

### Function: `jwtVerify`
```cpp
bool jwtVerify(string token, string algorithm, string secret)
```
* __token:__ Lorem ipsum
* __algorithm:__ Lorem ipsum
* __secret__ Lorem ipsum

Returns __true__ if the jwt was successfully verified, __false__ otherwise.

### Function: `jwtGetClaims`
```cpp
table jwtGetClaims(string token)
```
* __token:__ Lorem ipsum

Returns a table __containing the claims__.

## Contributors
* Stefan K.
