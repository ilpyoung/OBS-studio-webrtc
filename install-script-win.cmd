set DepsPath=C:\obs-deps\win64
set QTDIR64=C:\Qt\5.15.2\msvc2019_64
set CEF_ROOT_DIR=C:\obs-deps\cef
set build_config=Release
set VIRTUALCAM-GUID=A3FCE0F5-3493-419F-958A-ABA1250EC20B
set WEBRTC_LIB=C:\Users\L\Documents\libwebrtc-bin\package\libwebrtc-win-x64\release\webrtc.lib
set WEBRTC_INCLUDE_DIR=C:\Users\L\Documents\libwebrtc-bin\package\libwebrtc-win-x64\include
set OPENSSL_ROOT_DIR=C:/obs-deps/openssl-1.1/x64
set LIBOBS_DIR=C:\clone_build\OBS-studio-webrtc\build64_\libobs
set OBS_VERSION="27.1.3"

mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 -DBUILD_NDI=ON -DLibObs_DIR=%CD%\libobs -DLIBOBS_LIB=%CD%\libobs\Release\obs.lib -Dw32-pthreads_DIR=%CD%\deps\w32-pthreads -DOBS_FRONTEND_LIB=%CD%\UI\obs-frontend-api\Release\obs-frontend-api.lib -DLIBOBS_INCLUDE_DIR=%CD%\..\libobs -DBUILD_BROWSER=true -DBUILD_WEBSOCKET=OFF -DDepsPath=%DepsPath64% -DQt5_DIR=%QTDIR64%\lib\cmake\Qt5 -DCMAKE_SYSTEM_VERSION=10.0 -DOBS_VERSION_OVERRIDE=%OBS_VERSION% -DCOPIED_DEPENDENCIES=false -DCOPY_DEPENDENCIES=true -DENABLE_VLC=OFF -DCOMPILE_D3D12_HOOK=true -DCEF_ROOT_DIR=%CEF_ROOT_DIR% -DWEBRTC_INCLUDE_DIR="%WEBRTC_INCLUDE_DIR%" -DWEBRTC_LIB="%WEBRTC_LIB%" -DOPENSSL_ROOT_DIR="%OPENSSL_ROOT_DIR%" -DVIRTUALCAM_GUID="%VIRTUALCAM-GUID%" -DQt5Widgets_DIR=%QTDIR64%\lib\cmake\Qt5Widgets -DQt5Svg_DIR=%QTDIR64%\lib\cmake\Qt5Svg -DQt5Xml_DIR=%QTDIR64%\lib\cmake\Qt5Xml ..

cd ..

cmd /k
