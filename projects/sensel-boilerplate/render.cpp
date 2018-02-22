// Based on https://github.com/sensel/sensel-api/tree/master/sensel-examples/sensel-c/example-1-hello-sensel

#include <Bela.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sensel.h>
#include <sensel_device.h>

void senselSetup(int sampleRate, int frameMask, int contactMask);
void senselCleanup();
void senselPrintInfo();
void senselRead(void*);
void readSenselContacts(void*);
void readSenselForces(void*);
float samplesToMs(int samples);

SENSEL_HANDLE handle = NULL; //Handle that references a Sensel device
SenselDeviceList list; //List of all available Sensel devices
SenselFirmwareInfo fw_info; //Firmware info from the Sensel device
SenselSensorInfo sensor_info; //Sensor info from the Sensel device
SenselFrameData *frame = NULL; //SenselFrame data that will hold the contacts

// Sensel configuration options
int senselFrameMask = FRAME_CONTENT_PRESSURE_MASK + FRAME_CONTENT_LABELS_MASK + FRAME_CONTENT_CONTACTS_MASK + FRAME_CONTENT_ACCEL_MASK;
int senselContactMask = CONTACT_MASK_ELLIPSE + CONTACT_MASK_DELTAS + CONTACT_MASK_BOUNDING_BOX + CONTACT_MASK_PEAK;
SenselScanDetail senselScanDetail = SCAN_DETAIL_LOW; // HIGH, MEDIUM, LOW, UNKNOWN
unsigned short senselMaxFrameRate = 1000; // 125Hz default, 2kHz max, 1kHz actual max.
unsigned short senselContactMinForce = 160; // Multiply bgy unit scale for force (8). Default 160 (20g).

// Bela Auxiliary Task
AuxiliaryTask senselTask;    // Auxiliary task to read Sensel
int senselReadInterval = 1000;     // How often Sensel is read in Hz
int senselReadCount = 0;           // How long until we read again...
int senselReadIntervalSamples = 0; // How many samples between reads

bool taskDidStart = false;
bool taskDidEnd = false;
bool taskTimerOn = false;
int taskTimer = 0;

int gSampleRate = 0;

bool setup(BelaContext *context, void *userData)
{
	gSampleRate = context->audioSampleRate;
	senselSetup(gSampleRate, senselFrameMask, senselContactMask);
	return true;
}

void render(BelaContext *context, void *userData)
{
	for(int n = 0; n < context->audioFrames; n++) {
		if(++senselReadCount >= senselReadIntervalSamples) {
			senselReadCount = 0;
			Bela_scheduleAuxiliaryTask(senselTask);
		}
		
		if (taskDidStart == true) {
			taskTimerOn = true;
			taskDidStart = false;
		}
		if (taskDidEnd == true) {
			taskTimerOn = false;
			taskDidEnd = false;
			rt_printf("Task took: %.2f ms\n", samplesToMs(taskTimer));
			taskTimer = 0;
		}
		if (taskTimerOn == true) taskTimer++;
		
	}
}

void cleanup(BelaContext *context, void *userData)
{
	senselCleanup();
}

// Auxiliary task to read the Sensel contact data
void senselRead(void*)
{
	taskDidStart = true;
	
	unsigned int num_frames = 0;
	senselReadSensor(handle); //Read all available data from the Sensel device
	senselGetNumAvailableFrames(handle, &num_frames); //Get number of frames available in the data read from the sensor
	
	for (int f = 0; f < num_frames; f++)
	{
		senselGetFrame(handle, frame); //Read one frame of data
	}
	
	taskDidEnd = true;
}

float samplesToMs(int samples)
{
	return (float)samples * (1000/(float)gSampleRate);
}

void senselSetup(int sampleRate, int frameMask, int contactMask)
{
	senselGetDeviceList(&list); // Get a list of avaialble Sensel devices
	if (list.num_devices == 0) fprintf(stdout, "No device found\n");

	senselOpenDeviceByID(&handle, list.devices[0].idx); // Open Sensel by id in SenselDeviceList, handle initialized 

	senselGetFirmwareInfo(handle, &fw_info);   // Get the firmware info
	senselGetSensorInfo(handle, &sensor_info); // Get the sensor info
	senselPrintInfo(); // Print the firmware and sensor info

	// Sensel configuration options http://guide.sensel.com/sensel_h
	// senselSetBufferControl(handle, ); // Set the number of frame buffers the device should store internally. unsigned char num.
	// senselSetContactsEnableBlobMerge(handle, ); // Set contact blog merging setting. 0 to disable - 1 to enable.
	senselSetFrameContent(handle, frameMask);   // Set the frame information reported by the sensor
	senselSetContactsMask(handle, contactMask); // Set the contact information reported by the sensor
	senselSetContactsMinForce(handle, senselContactMinForce); // Set the minimum force a contact needs to have to be reported. unsigned short val. Default? Minimum?
	// senselSetDynamicBaselineEnabled(handle, ); // Set the dynamic baselining based on val.
	// senselSetLEDBrightness(handle, led_id, brightness); // Update brightness of one LED
	senselSetMaxFrameRate(handle, senselMaxFrameRate); // Sets the maximum framerate at which the device should report
	senselSetScanDetail(handle, senselScanDetail); // Set the level of scanning detail returned by the device. SenselScanDetail detail.
	// senselSetScanMode(handle, ); // Sets the current scan mode. SenselScanMode mode.

	senselAllocateFrameData(handle, &frame); // Allocate a frame of data, must be done before reading frame data
    senselStartScanning(handle); // Start scanning the Sensel device
    
	senselTask = Bela_createAuxiliaryTask(senselRead, 50, "bela-sensel"); // Create Bela auxiliary task to handle Sensel reading
	senselReadIntervalSamples = sampleRate / senselReadInterval; // Set read interval for reading Sensel
}

void senselPrintInfo()
{
	fprintf(stdout, "\nSensel Device: %s\n" ,list.devices[0].serial_num );
	fprintf(stdout, "Firmware Version: %d.%d.%d\n", fw_info.fw_version_major, fw_info.fw_version_minor, fw_info.fw_version_build);
	fprintf(stdout, "Width: %fmm\n", sensor_info.width);
	fprintf(stdout, "Height: %fmm\n", sensor_info.height);
	fprintf(stdout, "Cols: %d\n", sensor_info.num_cols);
	fprintf(stdout, "Rows: %d\n", sensor_info.num_rows);
}

void senselCleanup()
{
	senselStopScanning(handle); // Stop scanning
	senselSoftReset(handle); // All registers are reset to their initial state
}