# pops-smb-config
A tool for updating the SMB configuration of POPStarter on the console itself without the need of a PC.

## How to use
### Requirements
In order to use this software, you already need to have a working installation of the [POPStarter SMB files](https://bitbucket.org/ShaolinAssassin/popstarter-documentation-stuff/wiki/smb-mode) with some requirements:
- The file SMBCONFIG.DAT should have the IP address with it's port.

- The file SMBCONFIG.DAT should have 3 lines, first one for IP and share name, and the other two for user and password (the last two can be empty, but should be there).  
  
SMBCONFIG.DAT example:  
```
192.168.1.69:445 SMBSHARE
user 
password
```
- The file IPCONFIG.DAT should be present, even if the tutorial says it's optional, in order to edit it (duh).

### Limitations
The software does not have a text input, only number modification. Because of the former, this software can only update IP addresses and not share names, users and passwords.

### How to
The software is pretty self explanatory but here it goes anyways.  
First you're asked which Memory Card you want to work on, then you're asked which file you want to edit (either SMBCONFIG.DAT or IPCONFIG.DAT), after this, the chosen file is loaded and you can edit it.  
Editing is done by moving the cursor with the D-Pad and changing the values with R1/R2 and L1/L2.  
After editing, by pressing CROSS you can save the file or you can always go back by pressing CIRCLE.  

### Screenshots
<img src="https://user-images.githubusercontent.com/58561253/158274333-1540dd8d-d45c-4759-929a-319359bd624f.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/158274336-dcf16797-d050-4aae-a6ff-fc0e9f8808a8.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/158274338-915ea7e4-19cc-40a8-bbd4-58abece218d7.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/158274341-4483fc62-da20-4d12-83ef-3e42410b66cf.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/158274344-0fe4425c-ef8f-402b-bbe7-639c92458d9b.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/158274346-2ffeec73-2cba-4898-a4f0-1840381b3c7d.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/158274347-10500025-f3c1-433b-87c5-f6c65fda792d.png" width="320" height="224" /> <img src="https://user-images.githubusercontent.com/58561253/158274349-9f65afd7-5846-4e64-910e-fb1da89ce3cc.png" width="320" height="224" />

## How to build
- Download and build the latest [PS2SDK](https://github.com/ps2dev/ps2dev)
- Clone the repo: `git clone https://github.com/blckbearx/pops-smb-config.git`
- Enter the directory where the repo was cloned and run `make`
