make clean
make
arm-buildroot-linux-uclibcgnueabi-strip ./bin/gw_sip.mod
cp -f ./bin/gw_sip.mod /root/Desktop/davinci/dm3730/dvsdk4_03/filesystem/dm3730rootfs-guowang/dm3730rootfs/opt/app1
