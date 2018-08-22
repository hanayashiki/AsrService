start_web_service:
	dotnet Service/bin/Debug/netcoreapp2.1/publish/Service.dll \
		--decoder-config=ServiceConf/async_decoder_config.json

