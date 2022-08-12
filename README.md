# WrappedAFX

![Logo](https://github.com/inpinseptipin/WrappedFX/blob/master/res/Logo.PNG)

## About
WrappedAFX is a simple header wrapper that consolidates all the functionality offered by RackAFX and provides a simple interface to write an audio plugin.

## How to Install
1. Download the latest release from [here](https://github.com/inpinseptipin/WrappedAFX/releases/tag/v1.1)

2. Extract the Zip Archive, in the Extracted Folder, Copy the contents inside the directory "Install" 

![Install_2](https://github.com/inpinseptipin/WrappedAFX/blob/master/res/Install_2.PNG)
 
3. Traverse to the RackaFx Installation Folder :  **C:\Program Files (x86)\RackAFX 7.0\BaseClassFiles\RafxTwoPlus\PluginKernel**.

![Install_1](https://github.com/inpinseptipin/WrappedAFX/blob/master/res/Install_1.PNG)

4. Paste the files in the RackAfx installation Folder

![Install_3](https://github.com/inpinseptipin/WrappedAFX/blob/master/res/Install_3.PNG)
## Linking WrappedAFX with your RackAFX project

1. Create a new project using RackAfx.

2. Open the compiler using RackAFX 

3. In your solution explorer, find the project filter "PluginKernel/PluginCore".

4. Right Click on it, click Add->ExistingItem and then select the WrappedFX.h file.

![Include_1](https://github.com/inpinseptipin/WrappedAFX/blob/master/res/Include_1.png)

![Include_2](https://github.com/inpinseptipin/WrappedAFX/blob/master/res/Include_2.png)

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


# How to Uninstall

1. Copy the contents inside the directory "Uninstall" 

2. Traverse to the RackaFx Installation Folder :  **C:\Program Files (x86)\RackAFX 7.0\BaseClassFiles\RafxTwoPlus\PluginKernel**.

3. Paste the files in the RackAfx installation Folder
