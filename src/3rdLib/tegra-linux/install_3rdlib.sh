#!/bin/sh
#检测用户必须为ROOT
USER_ID=`id -u`
ROOT_UID=0
if [ $USER_ID -eq $ROOT_UID ]
then
	echo "[OK]run installso.sh by root user"
else
	echo "[Permission denied],must run installso.sh by root user!"
	exit 0
fi

LIB_DIR="$1"
PWD_DIR=`pwd`

#调用格式 Install_Ln_Shared3 curl/libcurl.so.4.4.0
Install_Ln_Shared3()
{
	SOFILE="$1"
	FULL=${SOFILE##*/}
	PATCH=${FULL%.*}
	MINOR=${PATCH%.*}
	MAJOR=${MINOR%.*}
	
	echo ""
	echo "install $SOFILE $LIB_DIR"
	install $SOFILE $LIB_DIR
	
	cd $LIB_DIR
	ln -s -f $FULL $MINOR || { rm -f $MINOR && ln -s $FULL $MINOR; };
	echo "ln -s -f $FULL $MINOR"
	ln -s -f $MINOR $MAJOR || { rm -f $MAJOR && ln -s $MINOR $MAJOR; };
	echo "ln -s -f $MINOR $MAJOR"
	cd $PWD_DIR
}

Install_Ln_Shared3_2()
{
	SOFILE="$1"
	FULL=${SOFILE##*/}
	PATCH=${FULL%.*}
	MINOR=${PATCH%.*}
	MAJOR=${MINOR%.*}
	
	echo ""
	echo "install $SOFILE $LIB_DIR"
	install $SOFILE $LIB_DIR
	
	cd $LIB_DIR
	ln -s -f $FULL $PATCH || { rm -f $PATCH && ln -s $FULL $PATCH; };
	echo "ln -s -f $FULL $PATCH"
	ln -s -f $PATCH $MAJOR || { rm -f $MAJOR && ln -s $PATCH $MAJOR; };
	echo "ln -s -f $PATCH $MAJOR"
	cd $PWD_DIR
}

#调用格式 Install_Ln_Shared1 curl/libcurl.so.4
Install_Ln_Shared1()
{
	SOFILE="$1"
	FULL=${SOFILE##*/}
	MAJOR=${FULL%.*}
	
	echo ""
	echo "install $SOFILE $LIB_DIR"
	install $SOFILE $LIB_DIR
	
	cd $LIB_DIR
	ln -s -f $FULL $MAJOR || { rm -f $MAJOR && ln -s $FULL $MAJOR; };
	echo "ln -s -f $FULL $MAJOR"
	cd $PWD_DIR
}

#调用格式 Install_Ln_Shared0 curl/libcurl.so
Install_Ln_Shared0()
{
	SOFILE="$1"
	FULL=${SOFILE##*/}
	
	echo ""
	echo "install $SOFILE $LIB_DIR"
	install $SOFILE $LIB_DIR
}

Install_Ln_Shared3 "iconv/lib/libiconv.so.2.6.0"
Install_Ln_Shared3 "sqlite/lib/libsqlite3.so.0.8.6"
Install_Ln_Shared3 "curl/lib/libcurl.so.4.4.0"
Install_Ln_Shared3 "mp4v2/lib/libmp4v2.so.2.0.0"
Install_Ln_Shared3 "jsoncpp/lib/libjsoncpp.so.1.6.5"
Install_Ln_Shared3 "libxml2/lib/libxml2.so.2.9.4"
Install_Ln_Shared3 "xlslib/lib/libxls.so.2.0.3"
Install_Ln_Shared3 "lm_sensors/lib/libsensors.so.4.4.0"
Install_Ln_Shared3 "util-linux-ng-2.16/lib/libuuid.so.1.3.0"
Install_Ln_Shared3 "parted/lib/libparted.so.2.0.0"
Install_Ln_Shared3 "zlib/lib/libz.so.1.2.8"
Install_Ln_Shared3 "libjpeg/lib/libjpeg.so.9.1.0"
Install_Ln_Shared3 "ffmpeg/lib/libavcodec.so.56.60.100"
Install_Ln_Shared3 "ffmpeg/lib/libavdevice.so.56.4.100"
Install_Ln_Shared3 "ffmpeg/lib/libavfilter.so.5.40.101"
Install_Ln_Shared3 "ffmpeg/lib/libavformat.so.56.40.101"
Install_Ln_Shared3 "ffmpeg/lib/libavutil.so.54.31.100"
Install_Ln_Shared3 "ffmpeg/lib/libavutil.so.54.31.100"
Install_Ln_Shared3 "ffmpeg/lib/libswresample.so.1.2.101"
Install_Ln_Shared3 "ffmpeg/lib/libswscale.so.3.1.101"
#Install_Ln_Shared3_2 "opencv/lib/libopencv_calib3d.so.2.4.9"
#Install_Ln_Shared3_2 "opencv/lib/libopencv_contrib.so.2.4.9"
Install_Ln_Shared3_2 "opencv/lib/libopencv_core.so.2.4.9"
#Install_Ln_Shared3_2 "opencv/lib/libopencv_features2d.so.2.4.9"
#Install_Ln_Shared3_2 "opencv/lib/libopencv_flann.so.2.4.9"
Install_Ln_Shared3_2 "opencv/lib/libopencv_highgui.so.2.4.9"
Install_Ln_Shared3_2 "opencv/lib/libopencv_imgproc.so.2.4.9"
#Install_Ln_Shared3_2 "opencv/lib/libopencv_legacy.so.2.4.9"
#Install_Ln_Shared3_2 "opencv/lib/libopencv_ml.so.2.4.9"
#Install_Ln_Shared3_2 "opencv/lib/libopencv_objdetect.so.2.4.9"
#Install_Ln_Shared3_2 "opencv/lib/libopencv_video.so.2.4.9"
Install_Ln_Shared0 "opencv/lib/libtbb.so" $LIB_DIR
Install_Ln_Shared3 "freetype/lib/libfreetype.so.6.13.0"
Install_Ln_Shared0 "nvssdk/lib/libnvssdk.so" $LIB_DIR
Install_Ln_Shared0 "nvssdk/lib/libossdk.so" $LIB_DIR
Install_Ln_Shared3 "iconv/lib/libiconv.so.2.6.0"
#Install_Ln_Shared1 "mosquitto/lib/libmosquitto.so.1"
Install_Ln_Shared1 "rtmpdump/lib/librtmp.so.0"

