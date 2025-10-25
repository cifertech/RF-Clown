<div align="center">

  <img src="https://user-images.githubusercontent.com/62047147/195847997-97553030-3b79-4643-9f2c-1f04bba6b989.png" alt="logo" width="100" height="auto" />
  
  <h1> RF-Clown </h1>
  <p> A fully open-source BLE and Bluetooth jammer </p>


<!-- Badges -->
<a href="https://github.com/cifertech/nrfbox" title="Go to GitHub repo"><img src="https://img.shields.io/static/v1?label=cifertech&message=RF-Clown&color=purple&logo=github" alt="cifertech - RF-Clown"></a>
<a href="https://github.com/cifertech/nrfbox"><img src="https://img.shields.io/github/stars/cifertech/RF-Clown?style=social" alt="stars - RF-Clown"></a>
<a href="https://github.com/cifertech/nrfbox"><img src="https://img.shields.io/github/forks/cifertech/RF-Clown?style=social" alt="forks - RF-Clown"></a>

   
<h4>
    <a href="https://twitter.com/techcifer">TWITTER</a>
  <span> · </span>
    <a href="https://www.instagram.com/cifertech/">INSTAGRAM</a>
  <span> · </span>
    <a href="https://www.youtube.com/@techcifer">YOUTUBE</a>
  <span> · </span>
    <a href="https://cifertech.net/">WEBSITE</a>
  </h4>
</div>
<br/>

## 📖 Explore the Full Documentation

Ready to dive deeper into RF-Clown's details? Discover the full story, in-depth tutorials, and all the exciting features in our comprehensive [documentation](https://cifertech.net/rf-clown-your-portable-ble-bluetooth-jamming-tool/). Click the link and explore further!
  
<div>&nbsp;</div>

<!-- About the Project -->
## :star2: About the Project
RF-Clown is an open-source BLE and Bluetooth jammer inspired by similar projects in the community. Unlike many closed-source implementations, RF-Clown emphasizes transparency and education. The project is meant for testing and learning about RF technologies and communication protocols.  

**Why RF-Clown?**  
1. Promote open-source collaboration in RF research.  
2. Encourage exploration of RF vulnerabilities.   

<div align="center"> 
  <img src="https://github.com/user-attachments/assets/7be8e9b4-07ea-4f39-9cd4-d8588a95a920" alt="screenshot" width="Auto" height="Auto" />
</div>

<div>&nbsp;</div>

<!-- Features -->
### 🎯 Features

- **Triple NRF24** (GT24 Mini modules) for increased coverage and performance  
- **External antenna** setup: 4× IPEX cables + 8 dBi antennas  
- **OLED display** + **3 tactile switches** for simple on-device navigation  
- **NeoPixel Feedback**: LED indicates the current mode (BLE, Bluetooth, or combined).  
- **Portable Design**: Powered by a lithium battery with a TP4056 charger for convenience.  
- **Custom PCB**: Includes an onboard CP2102 for programming, LF33 voltage regulator, and other essential components.  
- **Open-Source Code**: Freely available for exploration and modification.  

<div>&nbsp;</div>

## :eyes: RF-Clown Versions:

<table>
  <tr>
    <td style="text-align: center;">
      <img src="https://github.com/user-attachments/assets/64df30e1-52b5-4cfb-91c8-b530d30b85e5" alt="RF-Clown v1" style="width: 600px; border: 1px solid #ccc; border-radius: 5px;">
      <p style="font-style: italic; font-size: 14px; margin-top: 5px;">RF-Clown v1</p>
    </td>    
    <td style="text-align: center;">
      <img src="https://github.com/user-attachments/assets/92f980b4-5c40-438e-ad00-413e6b39dc6d" alt="RF-Clown v2" style="width: 700px; border: 1px solid #ccc; border-radius: 5px;">
      <p style="font-style: italic; font-size: 14px; margin-top: 5px;">RF-Clown v2</p>
    </td>
  </tr>
</table>

<div>&nbsp;</div>

## 🔗 Uploading the Firmware (.bin) to ESP32

If you prefer not to upload the code manually or encounter errors during compilation, you can directly upload the precompiled `.bin` file to the ESP32. Follow these steps:

### Step 1: Download the Required Tools
1. **ESP32 Flash Download Tool**: Download the tool from Espressif's official site: [ESP32 Flash Download Tool](https://www.espressif.com/sites/default/files/tools/flash_download_tool_3.9.7_2.zip).
2. **USB Drivers**: Make sure the drivers for the ESP32 are installed on your system. You can download the drivers from [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) if you're using the CP2102 chip.

### Step 2: Prepare the `.bin` File
1. Download the precompiled `.bin` file from the repository. 

### Step 3: Upload the `.bin` File to ESP32
1. Connect your ESP32 to your computer using a USB cable.
2. Open the **ESP32 Flash Download Tool** and configure the following:
   - Select the appropriate **chip type** (ESP32) and click **OK**.
   - In the **Download Path Config** section, add the `.bin` file and set the start address to `0x1000`.
   - Select the correct **COM port** where your ESP32 is connected.
   - Choose the correct **Baud rate** (115200 is generally recommended).

3. Click on **Start** to begin uploading the `.bin` file to your ESP32.

### Step 4: Verify the Upload
Once the upload is completed, the tool will confirm a successful flash. You can now restart your ESP32 and run the firmware.

&nbsp;
### Arduino IDE Settings:
If you want to upload the sketch code to the ESP32, use the following settings in the Tools menu:
- Built using ESP32 Board Version 1.0.5


<div>&nbsp;</div>

<!-- License -->
## :warning: License

Distributed under the MIT License. See LICENSE.txt for more information.

<div>&nbsp;</div>

<!-- Contact -->
## :handshake: Contact

▶ Support me on Patreon [patreon.com/cifertech](https://www.patreon.com/cifertech)

CiferTech - [@twitter](https://twitter.com/techcifer) - CiferTech@gmali.com

Project Link: [https://github.com/cifertech/RF-Clown](https://github.com/cifertech/RF-Clown)

<div>&nbsp;</div>

