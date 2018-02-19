// Based on https://github.com/sensel/sensel-api/tree/master/sensel-examples/sensel-c/example-4-multi

#include <Bela.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sensel.h>
#include <sensel_device.h>

void setupMultipleSensels();
void readSenselContacts(void*);

SENSEL_HANDLE handle[SENSEL_MAX_DEVICES] = { NULL }; //Handle that references a Sensel device
SenselDeviceList list; //List of all available Sensel devices
SenselFrameData *frame[SENSEL_MAX_DEVICES] = { NULL }; //SenselFrame data that will hold the contacts

static const char* CONTACT_STATE_STRING[] = { "CONTACT_INVALID","CONTACT_START", "CONTACT_MOVE", "CONTACT_END" };

// Bela Auxiliary Task
AuxiliaryTask senselTask;    // Auxiliary task to read Sensel
int readInterval = 1000;     // Change this to change how often the Sensel is read (in Hz)
int readCount = 0;           // How long until we read again...
int readIntervalSamples = 0; // How many samples between reads

bool setup(BelaContext *context, void *userData)
{
	setupMultipleSensels();
	
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
	for (int i = 0; i < list.num_devices; i++) {
		senselReadSensor(handle[i]);
		//Get number of frames available in the data read from the sensor
		senselGetNumAvailableFrames(handle[i], &num_frames);
		for (unsigned int f = 0; f < num_frames; f++)
		{
			//Read one frame of data
			senselGetFrame(handle[i], frame[i]);
			//Print out contact data
			if (frame[i]->n_contacts > 0) {
				fprintf(stdout, "\nMorph %d Num Contacts: %d\n", i, frame[i]->n_contacts);
				for (int c = 0; c < frame[i]->n_contacts; c++)
				{
					unsigned int state = frame[i]->contacts[c].state;
					fprintf(stdout, "Contact ID: %d State: %s\n", frame[i]->contacts[c].id, CONTACT_STATE_STRING[state]);

					//Turn on LED for CONTACT_START
					if (state == CONTACT_START) {
						senselSetLEDBrightness(handle[i], frame[i]->contacts[c].id, 100);
					}
					//Turn off LED for CONTACT_END
					else if (state == CONTACT_END) {
						senselSetLEDBrightness(handle[i], frame[i]->contacts[c].id, 0);
					}
				}
			}
		}
	}
}

void setupMultipleSensels() 
{
	//Get a list of avaialble Sensel devices
	senselGetDeviceList(&list);
	if (list.num_devices == 0)
	{
		fprintf(stdout, "No device found\n");
	}

	//Open all Sensel devices by the id in the SenselDeviceList, handle initialized 
	for (int i = 0; i < list.num_devices; i++) {
		senselOpenDeviceByID(&handle[i], list.devices[i].idx);

		//Set the frame content to scan contact data
		senselSetFrameContent(handle[i], FRAME_CONTENT_CONTACTS_MASK);
		//Allocate a frame of data, must be done before reading frame data
		senselAllocateFrameData(handle[i], &frame[i]);
		//Start scanning the Sensel device
		senselStartScanning(handle[i]);
    }
}