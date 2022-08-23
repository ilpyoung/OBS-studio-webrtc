REM if exist dependencies2019.zip (curl -kLO https://cdn-fastly.obsproject.com/downloads/dependencies2019.zip -f --retry 5 -z dependencies2019.zip) else (curl -kLO https://cdn-fastly.obsproject.com/downloads/dependencies2019.zip -f --retry 5 -C -)
if exist dependencies2019.zip (curl -u %FTP_LOGIN%:%FTP_PASSWORD% -kLO %FTP_PATH_PREFIX%/windows/dependencies2019.zip -f --retry 5 -z dependencies2019.zip) else (curl -u %FTP_LOGIN%:%FTP_PASSWORD% -kLO %FTP_PATH_PREFIX%/windows/dependencies2019.zip -f --retry 5 -C -)
if exist vlc.zip (curl -kLO https://cdn-fastly.obsproject.com/downloads/vlc.zip -f --retry 5 -z vlc.zip) else (curl -kLO https://cdn-fastly.obsproject.com/downloads/vlc.zip -f --retry 5 -C -)
REM if exist cef_binary_%CEF_VERSION%_windows_x64.zip (curl -kLO https://cdn-fastly.obsproject.com/downloads/cef_binary_%CEF_VERSION%_windows_x64.zip -f --retry 5 -z cef_binary_%CEF_VERSION%_windows_x64.zip) else (curl -kLO https://cdn-fastly.obsproject.com/downloads/cef_binary_%CEF_VERSION%_windows_x64.zip -f --retry 5 -C -)
if exist cef_binary_%CEF_VERSION%_windows_x64.zip (curl -u %FTP_LOGIN%:%FTP_PASSWORD% -kLO %FTP_PATH_PREFIX%/windows/cef_binary_%CEF_VERSION%_windows_x64.zip -f --retry 5 -z cef_binary_%CEF_VERSION%_windows_x64.zip) else (curl -u %FTP_LOGIN%:%FTP_PASSWORD% -kLO %FTP_PATH_PREFIX%/windows/cef_binary_%CEF_VERSION%_windows_x64.zip -f --retry 5 -C -)
if exist openssl-1.1.tgz (curl -kLO https://libwebrtc-community-builds.s3.amazonaws.com/openssl-1.1.tgz -f --retry 5 -z openssl-1.1.tgz) else (curl -kLO https://libwebrtc-community-builds.s3.amazonaws.com/openssl-1.1.tgz -f --retry 5 -C -)
7z x dependencies2019.zip -odependencies2019
7z x vlc.zip -ovlc
7z x cef_binary_%CEF_VERSION%_windows_x64.zip -oCEF_64
%CD%\libWebRTC-%LIBWEBRTC_VERSION%-x64-Release-H264-OpenSSL_1_1_1n.exe /S /SD /D="%CD%\libwebrtc"
tar -xzf openssl-1.1.tgz
