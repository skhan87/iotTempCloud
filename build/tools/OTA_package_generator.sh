#!/bin/bash
#
# Script for generating Over-The-Air update package.
#
# Used with aconno firmware to decrypt OTA update private key and use it to 
# generate an OTA update package out of currently compiled firmware.
#
# Meant to be invoked from makefile using a dedicated target.
#
# Usage:
#	* if not initially, make this script executable:
#			i.e. run 'chmod +x thisScript.sh' in terminal
#	* run script by giving it Nordic hardware platform as command line argument:
#			i.e. run './thisScript.sh nrf52832' for nrf52832 platform
#
# For encrypting files use:
#     openssl enc -aes-256-cbc -md sha512 -pbkdf2 -iter 100000 -salt -in file.txt -out file.txt.enc
#
# Hrvoje Brezak
# hrvoje@aconno.de
# aconno d.o.o.
# 14-09-2020

### Helper functions
function yell() { echo "$0: $*" >&2; }
function die() { yell "$*"; exit 111; }
function try() { "$@" || die "cannot $*"; }
function printHelp()
{
	echo
	echo
	echo '----------------------------------'
	echo 'HELP: OTA package generator script'
	echo '----------------------------------'
	echo 'This script is used to generate OTA update package for aconno products based on Nordic platform.'
	echo 'It is meant to be invoked on the command line using aconno project makefile target.'
	echo ''
	echo 'USAGE'
	echo '-----'
	echo 'Navigate inside aconno project directory using a terminal and use the following:'
	echo '	make <platform> pack_OTA_update'
	echo ''
	echo 'PLATFORMS'
	echo '---------'
	echo 'Currently supported platforms: nrf52832, nrf52840'
	echo 'Note that platforms can also be specific with compile targets:'
	echo '	make nrf52832_debug pack_OTA_update'
	echo 'This triggers both firmware compilation and subsequent OTA package generation.'
	echo ''
	echo 'MODES'
	echo '-----'
	echo 'Default update mode is <application only>. This option will create OTA package which shall'
	echo 'update application part of the memory only, leaving softdevice and bootloader memory intact.'
	echo 'This is what you would want in most cases. From time to time however, it is necessary to update'
	echo 'to newer version of softdevice or bootloader, to enjoy the benefits of new features and bug fixes'
	echo 'from Nordic. In this case user should select option to update all three and provide paths to hex'
	echo 'hex files of the newer softdevice and bootloader. Note that neither softdevice nor bootloader can'
	echo 'be updated without the other. This is a limitation set by Nordic.'
	echo ''
	echo 'Along with paths, user also has to provide a revision number of the new softdevice. This is required'
	echo 'by Nordic. One can find the appropriate ID by running "nrfutil pkg generate --help" inside a terminal'
	echo 'and reading notes under "--sd-req TEXT".'
	echo ''
	echo 'Note that application name is at the moment hardcoded to Nordic default name: '
	echo '	$OUTPUTDIR/$PLATFORM\_xxaa.hex'
	echo 'If this changes, modify bash variable HEXPATH accordingly.'
	echo '----------------------------------'
	echo ''
}

### Default paths
SCRIPTPATH=$(dirname $(realpath $0))
DEFAULT_KEYPATH=$SCRIPTPATH/dfu_key.pem.enc
DEFAULT_OUTPUTDIR=$(dirname $SCRIPTPATH)/_build

### Argument sanity checks
### (should be invoked with supported platform argument)
if [ $# -lt 1 ]
then
	die "Error: missing platform argument!"
fi

PLATFORM=$(echo $1 | cut -d'_' -f 1)
if [ "$PLATFORM" == "nrf52832" ]
then
	SOFTDEVICEID=0xAF
elif [ "$PLATFORM" == "nrf52840" ]
then
	SOFTDEVICEID=0xAE
else
	die "Error: invalid platform argument!"
fi

### OTA update mode selection
while :
do
	echo 'Default update mode is <Application only>. Continue?'
	read -p '(<Enter> to proceed, <s> for more options, <h> for help): ' MCHOICE

	if [ "$MCHOICE" == "" ]
	then
		MCHOICE=1
		echo
		break
	elif [ "$MCHOICE" == "s" ]
	then
		echo
		echo 'Update modes:'
		echo '     1) Application only'
		echo '     2) Application + Softdevice + Bootloader'
		read -p 'Enter mode: ' MCHOICE
		break
	elif [ "$MCHOICE" == "h" ]
	then
		printHelp
	else
		echo
		yell "Invalid mode choice!"
		echo
	fi
done

if [ "$MCHOICE" == "2" ]
then
	echo ''
	echo 'Building application + softdevice + bootloader OTA package.'
	while :
	do
		echo ''
		echo 'Enter following information to continue: '
		read -p '	- new softdevice .hex file path: ' SDHEXPATH
		if [ ! -f $SDHEXPATH ] || [ "$SDHEXPATH" == "" ]
		then
			echo "This file does not exist!"
			continue
		fi

		read -p '	- new softdevice ID: ' SDID
		if [ "$SDID" == "" ]
		then
			echo "Invalid softdevice ID entered!"
			continue
		fi

		read -p '	- new bootloader .hex file path: ' BLHEXPATH
		if [ ! -f $BLHEXPATH ] || [ "$BLHEXPATH" == "" ]
		then
			echo "This file does not exist!"
			continue
		fi

		# Valid paths entered
		break
	done
fi


### User input : private key path
while :
do
	echo ''
	read -p 'Enter path to encrypted private key (default: ../build/tools/dfu_key.pem.enc): ' KEYPATH

	if [ "$KEYPATH" == "" ]
	then
		KEYPATH=$DEFAULT_KEYPATH
		break
	elif [ ! -f $KEYPATH ]
	then
		echo "This file does not exist!"
		continue
	else
		# Valid custom path entered
		break
	fi
done

### User input : build directory path
while :
do
	read -p 'Enter path to output directory (default: ../build/_build): ' OUTPUTDIR

	if [ "$OUTPUTDIR" == "" ]
	then
		OUTPUTDIR=$DEFAULT_OUTPUTDIR
		break
	elif [ ! -d $OUTPUTDIR ]
	then
		echo "This directory does not exist!"
		continue
	else
		# Valid custom directory entered
		break
	fi
done

HEXPATH=$OUTPUTDIR/$PLATFORM\_xxaa.hex

### Decrypt private key
try openssl enc -aes-256-cbc -md sha512 -pbkdf2 -iter 100000 -d -in $KEYPATH -out $SCRIPTPATH/dfu_key.pem

### Generate OTA package
echo
if [ "$MCHOICE" == "1" ]
then
	try nrfutil pkg generate --application $HEXPATH --application-version-string "1.0.0" --app-boot-validation VALIDATE_GENERATED_CRC --hw-version 52 --sd-req $SOFTDEVICEID --key-file $SCRIPTPATH/dfu_key.pem $OUTPUTDIR/OTA_$PLATFORM\_updatePackage.zip
elif [ "$MCHOICE" == "2" ]
then
	try nrfutil pkg generate --application $HEXPATH --application-version-string "1.0.0" --app-boot-validation VALIDATE_GENERATED_CRC --hw-version 52 --sd-req $SOFTDEVICEID --sd-id $SDID --softdevice $SDHEXPATH --sd-boot-validation VALIDATE_GENERATED_CRC --bootloader $BLHEXPATH --bootloader-version 2 --key-file $SCRIPTPATH/dfu_key.pem $OUTPUTDIR/OTA_$PLATFORM\_updatePackage.zip
fi
echo OTA update packed.

### Clean up
rm $SCRIPTPATH/dfu_key.pem
