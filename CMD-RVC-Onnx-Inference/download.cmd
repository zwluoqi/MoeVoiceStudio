set model=hubert4.0.7z

:download_model
echo Downloading ggml model %model%...


if exist "%model%" (
  echo Model %model% already exists. Skipping download.
  goto :eof
)

PowerShell -NoProfile -ExecutionPolicy Bypass -Command "Invoke-WebRequest -Uri https://huggingface.co/NaruseMioShirakana/MoeSS-SUBModel/resolve/main/%model% -OutFile %model%"

if %ERRORLEVEL% neq 0 (
  echo Failed to download ggml model %model%
  echo Please try again later or download the original Whisper model files and convert them yourself.
  goto :eof
)

echo Done! Model %model% saved in %root_path%\models\ggml-%model%.bin
echo You can now use it like this:
echo main.exe -m %root_path%\models\ggml-%model%.bin -f %root_path%\samples\jfk.wav