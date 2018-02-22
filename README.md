# Sensel Morph Bela examples

This repo demonstrates how to use the Sensel Morph (http://sensel.com, a "multi-touch, highly sensitive, low power and mobile" pressure sensor, with Bela (http://bela.io), "the open source hardware and sofware platform for ultra-low latency sensor and audio processing".

# Testing your Sensel Morph with SenselApp

First, make sure your Sensel is functioning and up to date by installing the SenselApp and connecting to your device. Play with the SenselApp visualiser to get a feel for the Morph's capabilities.

# Using Sensel Morph as a MIDI device with Bela

The simplest way to use Sensel Morph with Bela is as a MIDI device. Sensel Morph can be configured as a MIDI device from the SenselApp. Even if you don't have a musical overlay for Sensel Morph, you can set the "no overlay" default device behaviour to be MPE (for more on MPE MIDI see: [https://www.midi.org/articles/midi-polyphonic-expression-mpe](https://www.midi.org/articles/midi-polyphonic-expression-mpe)). Make sure you can see Sensel Morph's MIDI messages from your desktop machine (using e.g. MIDI Monitor for Mac).

To test this setup, connect Sensel Morph to Bela using the USB Host port on the BeagleBone Black. Access the Bela IDE, open and run the C++ example `05-Communication/MIDI`. You should see Sensel Morph's MIDI messages displayed in the Bela IDE console. The MIDI example shows how to use MIDI data in a Bela project.

# Installing the Sensel API on Bela

Sensel have released an API for accessing Sensel Morph data under the MIT License: [www.github.com/sensel/sensel-api](http://www.github.com/sensel/sensel-api). It is possible to access high-level multitouch data as well as raw force frame data using the API, and the Sensel team recently published an installer for the API that is compatible with the BeagleBoard's hard float architecture.

This Bela blog post provides an introduction to installing external libraries on Bela: [http://blog.bela.io/2017/01/04/using-an-external-library-with-bela/](http://blog.bela.io/2017/01/04/using-an-external-library-with-bela/). This process requires your Bela to have a WiFi connection: [https://github.com/BelaPlatform/Bela/wiki/Connecting-Bela-to-wifi](https://github.com/BelaPlatform/Bela/wiki/Connecting-Bela-to-wifi).

Steps to install the Sensel API:

- Connect your Bela to your computer, wait for it to boot, and open a terminal window
- `ssh root@192.168.7.2`
- `cd Bela; mkdir sensel ; cd sensel; git clone https://github.com/sensel/sensel-api`
- `sudo dpkg -i sensel-api/sensel-install/senselliblinux0.8.2-arm.deb`

To use the Sensel library, add the following Make Parameter to your Bela project in the Bela Project Settings menu: `LDLIBS=-lsensel.`

# Running the Sensel API C Examples on Bela

The Sensel API C examples have been ported to Bela projects here: [https://github.com/jarmitage/bela-sensel/projects/sensel-examples/](https://github.com/jarmitage/bela-sensel/projects/sensel-examples/). The difference between the original examples and the versions in this repo is their integration with Bela's `setup()`, `render()` and `cleanup()` functions, and the use of Bela's [AuxiliaryTask](https://github.com/BelaPlatform/Bela/blob/8d0196cb5748ad46a13b973c836bd479a5bd49d4/core/AuxiliaryTasks.cpp) class for scheduling Sensel API data calls.

The first example prints information about the device, and you should see an output like this:

    Building render.cpp... 
    ...done
    Linking... 
    ...done
    Build finished
    Running project...
    
    Found device: ttyACM0
    
    Sensel Device: SM00000000
    Firmware Version: 0.14.120
    Width: 230.000000mm
    Height: 130.000000mm
    Cols: 185
    Rows: 105
    Couldn't initialise audio rendering
    Error: unable to initialise audio
    make: *** [runide] Error 255
    Bela stopped

If your Sensel does not appear to be connected, try to `reboot` your Bela with the Sensel connected via USB and run the example again.

The second and third examples will print contact data and force frame data (http://guide.sensel.com/api/). The fourth example demonstrates reading multiple Sensel Morph devices simultaneously; this has not been tested on Bela.

# TODO

- Investigate the different Sensel Morph scan speeds/resolutions
- Add basic audio outputs to the examples
- Test latency
- Add more examples
