# POPStarter SMB Configuration Tool (pops-smb-config)
A tool for updating the SMB configuration of POPStarter on the console itself without the need of a PC.

## How to use
### Requirements
In order to use this software, you already need to have a working installation of the [POPStarter SMB files](https://bitbucket.org/ShaolinAssassin/popstarter-documentation-stuff/wiki/smb-mode) with some requirements:
- The file SMBCONFIG.DAT should have the IP address with it's port.

- The file SMBCONFIG.DAT should have 3 lines, first one for IP and share name, and the other two for user and password (the last two can be empty, but should be there).

- The file IPCONFIG.DAT should be present, even if the tutorial says it's optional, in order to edit it (duh).
  
SMBCONFIG.DAT example:  
```
192.168.1.69:445 SMBSHARE
user 
password
```

### Limitations
The software does not have a text input, only number modification. Because of the former, this software can only update IP addresses and not share names, users and passwords.

### How to
The software is pretty self explanatory but here it goes anyways.  
First you're asked which Memory Card you want to work on, then you're asked which file you want to edit (either SMBCONFIG.DAT or IPCONFIG.DAT), after this, the chosen file is loaded and you can edit it.  
Editing is done by moving the cursor with the D-Pad and changing the values with R1/R2 and L1/L2.  
After editing, by pressing CROSS you can save the file or you can always go back by pressing CIRCLE.

If you want to be able to exit to both OpenPS2Loader or wLaunchELF you need to have them in in one of a few different locations listed below, I tried to check for the most common ones:

For OPL:
- mc?:/OPL/OPNPS2LD.ELF
- mc?:/APPS/OPNPS2LD.ELF

For wLE:
- mc?:/BOOT/BOOT.ELF
- mc?:/APPS/ULE.ELF

The "?" sign stands for either 0 for slot 1 and 1 for slot 2.

### To-do
- [ ] Add some sort of text input in order to edit Share name, user and password.
- [ ] Add the option to install the POPStarter network files in order to perform a first installation directly on the console (preferably after sorting out the text input option so the files can be installed with the user's desired Share name, user and password).
- [x] Add an option to either exit to OPL or wLE/uLE (will need to make a list of possible common installation paths for each software in order to check for it's existence and exit to either one of those).

### Screenshots
<img src="https://user-images.githubusercontent.com/58561253/218330029-45b06aca-7916-4e4a-8f73-9bc55c06993e.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/218330031-1464b085-f43e-4289-8872-46523bc277ef.png" width="320" height="224" />

<img src="https://user-images.githubusercontent.com/58561253/218330036-e14ef2e9-622e-4da3-855c-48feb696db37.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/218330037-197d09a4-7d34-483b-b32c-565811a29741.png" width="320" height="224" />

<img src="https://user-images.githubusercontent.com/58561253/218330039-f6f76cb2-106e-497d-83c6-1d1b48d46382.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/218330042-26c54c1d-2c6e-4b63-a72c-cd1eb76e72a9.png" width="320" height="224" />

<img src="https://user-images.githubusercontent.com/58561253/218330044-83de6391-3287-4621-925c-ca46fb919a4e.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/218330046-81b87fbd-3f6f-48a0-8932-b1b4722117e6.png" width="320" height="224" />

## How to build
- Download and build the latest [PS2SDK](https://github.com/ps2dev/ps2dev)
- Clone the repo: `git clone https://github.com/blckbearx/pops-smb-config.git`
- Enter the directory where the repo was cloned and run `make`
