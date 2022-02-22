# RackAFX-wrapper
A Simple Header wrapper over Rackfx GUI and Audio Process Frames

## Installation
1.Download the repository and extract the folder.

2.Copy the Source and the Header files from the folder.

3.Traverse to the RackaFx Installation Folder, "C:\Program Files (x86)\RackAFX 7.0\BaseClassFiles\RafxTwoPlus\PluginKernel".

4.Paste the copied files in the RackAfx installation Folder

## How to Include it in the project
1. Create a new project using RackAfx.

2. Open the Solution using RackAfx or just simply opening your visual studio solution file. 

3. In your solution explorer, find the project filter "PluginKernel". Right Click on it, click Add->ExistingItem and then select the AudioEffect.H file.


## Using it in your project
1. Create any GUI control using RackAfx GUI Manager

2. Open PluginCore.cpp and find the InitPluginParameters() function

3. Using the function call syntax mentioned below, you can map the plugin parameter to the Wrapper class. 

```
kernel.push(&<variable_name>,boundVariableType::<datatype>,controlID::<controlID>
```

4. After Mapping you can retrieve the latest value from your GUI parameter by using the following the function

```
getControl(controlID::<controlID>);
```
5. Conversely you can set a value to your GUI parameter by using the following the function.

```
setControlValue(<newValue>,controlID::<controlID>);
```

