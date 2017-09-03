## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em3 linker.cmd package/cfg/empty_min_pem3.oem3

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/empty_min_pem3.xdl
	$(SED) 's"^\"\(package/cfg/empty_min_pem3cfg.cmd\)\"$""\"C:/Users/Kuba/workspace_v6_1_3/Wireless_Sensor_Platform_RTOS_2_20_End_Device/.config/xconfig_empty_min/\1\""' package/cfg/empty_min_pem3.xdl > $@
	-$(SETDATE) -r:max package/cfg/empty_min_pem3.h compiler.opt compiler.opt.defs
