# invoke SourceDir generated makefile for empty_min.pem3
empty_min.pem3: .libraries,empty_min.pem3
.libraries,empty_min.pem3: package/cfg/empty_min_pem3.xdl
	$(MAKE) -f C:\Users\Kuba\workspace_v6_1_3\Wireless_Sensor_Platform_RTOS_2_20_End_Device/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Kuba\workspace_v6_1_3\Wireless_Sensor_Platform_RTOS_2_20_End_Device/src/makefile.libs clean

