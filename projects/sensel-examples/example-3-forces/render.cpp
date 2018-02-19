// Based on https://github.com/sensel/sensel-api/tree/master/sensel-examples/sensel-c/example-3-forces

#include <Bela.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sensel.h>
#include <sensel_device.h>

void setupSensel();
void readSenselContacts(void*);

SENSEL_HANDLE handle = NULL; //Handle that references a Sensel device
SenselDeviceList list; //List of all available Sensel devices
SenselFrameData *frame = NULL; //SenselFrame data that will hold the contacts
SenselSensorInfo sensor_info; //Sensor info from the Sensel device

float total_force = 0.0f;

// Bela Auxiliary Task
AuxiliaryTask senselTask;    // Auxiliary task to read Sensel
int readInterval = 1000;     // Change this to change how often the Sensel is read (in Hz)
int readCount = 0;           // How long until we read again...
int readIntervalSamples = 0; // How many samples between reads

bool setup(BelaContext *context, void *userData)
{
	setupSensel();
	
	senselTask = Bela_createAuxiliaryTask(readSenselContacts, 50, "bela-sensel");
	readIntervalSamples = context->audioSampleRate / readInterval;
	
	return true;
}


void render(BelaContext *context, void *userData)
{
	for(int n = 0; n < context->audioFrames; n++) {
		// Keep this code: it schedules the touch sensor readings
		if(++readCount >= readIntervalSamples) {
			readCount = 0;
			Bela_scheduleAuxiliaryTask(senselTask);
		}
	}
}

void cleanup(BelaContext *context, void *userData)
{

}

// Auxiliary task to read the Sensel
void readSenselContacts(void*)
{
	unsigned int num_frames = 0;
	//Read all available data from the Sensel device
	senselReadSensor(handle);
	//Get number of frames available in the data read from the sensor
	senselGetNumAvailableFrames(handle, &num_frames);
	for (int f = 0; f < num_frames; f++)
	{
		//Read one frame of data
		senselGetFrame(handle, frame);
		//Calculate the total force
		total_force = 0;
        for (int i = 0; i < sensor_info.num_cols*sensor_info.num_rows; i++)
        {
			total_force = total_force + frame->force_array[i];
        }
        fprintf(stdout, "Total Force : %f\n", total_force);
	}
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

	senselGetSensorInfo(handle, &sensor_info); //Get the sensor info
	senselSetFrameContent(handle, FRAME_CONTENT_PRESSURE_MASK); //Set the frame content to scan force data
	senselAllocateFrameData(handle, &frame); //Allocate a frame of data, must be done before reading frame data
    senselStartScanning(handle); //Start scanning the Sensel device
}