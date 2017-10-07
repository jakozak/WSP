# invoke SourceDir generated makefile for empty_min.pem3
empty_min.pem3: .libraries,empty_min.pem3
.libraries,empty_min.pem3: package/cfg/empty_min_pem3.xdl
	$(MAKE) -f C:\Projects\Wireless_Sensor_Platform/src/makefile.libs

clean::
	$(MAKE) -f C:\Projects\Wireless_Sensor_Platform/src/makefile.libs clean

