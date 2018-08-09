@echo off
for /f %%i in ('docker container list -q') do (
  set youngest=%%i
)
docker container stop %youngest%