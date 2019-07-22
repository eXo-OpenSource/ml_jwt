# JWT Server Module for MTA:SA
[![Build Status](https://img.shields.io/travis/com/eXo-OpenSource/ml_jwt/master.svg?style=flat-square&label=Linux:master)](https://travis-ci.com/eXo-OpenSource/ml_jwt)
[![Build Status](https://img.shields.io/appveyor/ci/StiviiK/ml-jwt/master.svg?style=flat-square&label=Windows:master)](https://ci.appveyor.com/project/StiviiK/ml-jwt)
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
string jwtSign(table claims, string algorithm, string secret)
```
* __claims:__ Lorem ipsum
* __algorithm:__ Lorem ipsum
* __secret__ Lorem ipsum

Returns the __jwt token__ if everything went fine, __false__ otherwise.

### Function: `jwtVerify`
```cpp
bool jwtSign(string token, string algorithm, string secret)
```
* __token:__ Lorem ipsum
* __algorithm:__ Lorem ipsum
* __secret__ Lorem ipsum

Returns __true__ if the jwt was successfully verified, __false__ otherwise.

### Function: `jwtGetClaims`
```cpp
table jwtSign(string token)
```
* __token:__ Lorem ipsum

Returns a table __containing the claims__.

## Contributors
* Stefan K.
