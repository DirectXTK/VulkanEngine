import os
import sys
import subprocess
import Utils
from pathlib import Path
from io import BytesIO
from urllib.request import urlopen

def InstallVulkanSDK():
    VulkanDir = "./Vendor/VulkanSDK/"
    VulkanInstallURL = "https://sdk.lunarg.com/sdk/download/1.3.296.0/windows/VulkanSDK-1.3.296.0-Installer.exe"
    PermissionGranted = False
    while not PermissionGranted:
        reply = str(input("Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(VulkanDir))).lower().strip()[:1]
        if reply == 'n':
            return 
        PermissionGranted = (reply =='y')

    print("Donwloading {0:s} to {1:s}".format(VulkanInstallURL,VulkanDir))
    Utils.DownloadFile(VulkanInstallURL,f"{VulkanDir}/VulkanSDK-1.3.296.0-Installer.exe")
    print("Running vulkan SDK installer...")
    os.startfile(os.path.abspath(VulkanDir+"VulkanSDK-1.3.296.0-Installer.exe"))
    os.remove(os.path.abspath(VulkanDir+"VulkanSDK-1.3.296.0-Installer.exe"))
    quit()
def ChechkVulkanSDKAndVersion():
    VulkanSDK = os.environ.get("VULKAN_SDK")
    if(VulkanSDK is None):
        InstallVulkanSDK()
        return False
    print("Re-run this script after installation!")
    return True
def DownloadAllEssentialPrograms():
    DownloadLoc= "./DownloadLoc/"
    AppNames= ["Discord.exe","VScode.exe"]
    InstallURL= ["https://discord.com/api/downloads/distributions/app/installers/latest?channel=stable&platform=win&arch=x64",
        "https://code.visualstudio.com/sha/download?build=stable&os=win32-x64-user",
                
                ]
    Index=0
    for x in InstallURL:
        Utils.DownloadFile(x,f"{DownloadLoc}/{AppNames[Index]}")
        os.startfile(os.path.abspath(DownloadLoc+AppNames[Index]))
        Index=Index+1

ChechkVulkanSDKAndVersion()
DownloadAllEssentialPrograms()
 