# WrappedAFX

![Logo](https://github.com/inpinseptipin/WrappedFX/blob/master/res/Logo.PNG)

## About
WrappedAFX is a simple header wrapper that consolidates all the functionality offered by RackAFX and provides a simple interface to write an audio plugin.

## How to Install
Download the Installer from [here](https://github.com/inpinseptipin/WrappedAFX/releases/download/v2.0/WrappedAFX_2.0_Installer.exe)

## Library Documentation
Documentation can be found [here](https://inpinseptipin.github.io/WrappedAFX/)

## Writing DSP Logic with your Gain Knob
1. To write the DSP logic for your audio plugin, Open the WrappedFX.h file and traverse to the run function.

2. To retrieve the latest value from the "Gain" knob, use the function with the following syntax below.

```
getParameter(controlID::<controlID>);
```

![DSP_1](https://github.com/inpinseptipin/WrappedAFX/blob/master/res/DSP_1.PNG)


## Sending Values to meters in RackAFX
1. Create a VU Meter on the prototype panel

![setPara_1](https://github.com/inpinseptipin/WrappedAFX/blob/master/res/setPara_1.PNG)

2. Pass the Value to the VU meter in the run function, using the function below
```
setMeterValue(<newValue>,controlID::<controlID>);
```
## Uninstall
WrappedAFX is installed on RackAFX 7.0 as a standalone patch and thus gets registered as its own entity in the Windows Registry, Therefore WrappedAFX can be uninstalled using the "add/remove" programs on Windows.
