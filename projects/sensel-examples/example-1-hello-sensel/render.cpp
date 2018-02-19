// Based on https://github.com/sensel/sensel-api/tree/master/sensel-examples/sensel-c/example-1-hello-sensel

#include <Bela.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sensel.h>
#include <sensel_device.h>

void setupSensel();
void printSenselInfo();

SENSEL_HANDLE handle = NULL; //Handle that references a Sensel device
SenselDeviceList list; //List of all available Sensel devices
SenselFirmwareInfo fw_info; //Firmware info from the Sensel device
SenselSensorInfo sensor_info; //Sensor info from the Sensel device

bool setup(BelaContext *context, void *userData)
{    
	setupSensel();
	printSenselInfo();   
	return 0;
}

void render(BelaContext *context, void *userData)
{

}

void cleanup(BelaContext *context, void *userData)
{

}

void printSenselInfo()
{
	senselGetFirmwareInfo(handle, &fw_info); // Get the firmware info
	senselGetSensorInfo(handle, &sensor_info); // Get the sensor info

	fprintf(stdout, "\nSensel Device: %s\n" ,list.devices[0].serial_num );
	fprintf(stdout, "Firmware Version: %d.%d.%d\n", fw_info.fw_version_major, fw_info.fw_version_minor, fw_info.fw_version_build);
	fprintf(stdout, "Width: %fmm\n", sensor_info.width);
	fprintf(stdout, "Height: %fmm\n", sensor_info.height);
	fprintf(stdout, "Cols: %d\n", sensor_info.num_cols);
	fprintf(stdout, "Rows: %d\n", sensor_info.num_rows);
}

void setupSensel() 
{
	//Get a list of avaialble Sensel devices
	senselGetDeviceList(&list);
	if (list.num_devices == 0)
	{
		fprintf(stdout, "No device found\n");
	}

	//Open a Sensel device by the id in the SenselDeviceList, handle initialized 
	senselOpenDeviceByID(&handle, list.devices[0].idx);
}