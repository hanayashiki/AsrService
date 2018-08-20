corerun:
	dotnet CoreRun/bin/Debug/netcoreapp2.0/CoreRun.dll
startservice:
	dotnet Service/bin/Debug/netcoreapp2.0/Service.dll\
	       	--decoder-config=Service/async_decoder_config.json
test_api_requests:
	python3 APITest/autoapi.py

