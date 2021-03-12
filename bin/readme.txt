This is a readme file for the Prime17 project.

Prime17
This is a father and son rather amateurish project. We are aware there is a lot to complain about, but the time and know-how is limited.  The project evolved from  a year long secondary school thesis focused on primes. Matej Zeman (a student and an author) and  Martin Zeman (consultant) of the thesis decided to continue in revealing mysteries of the primes even after the thesis was submitted for approval. 

Tasks
Prime17 project currently support only one distributed task:
 measurement of distribution of primes distribution within bit slices in intervals of length given by power of two (see jigsaw graph on the website). 
When this task is completed (or the unusual behavior explained) we may move onto another task which is verification of the BPSW primality test. 

You may contribute to the project by:
- sharing of your CPU computational power
- development or maintenance of the source codes
- providing an answer to the unusual distribution of the primes


Installation
There is no package nor installer yet so there may be some manual configuration steps required.
Just unzip all the files in a folder with write access. Then just run the prime17 (linux) or prime17.exe (Windows 64) file. 
A basic wizard should guide you through the very first configuration. 

Linux Installation  
1. Missing rights for an execution. Run following commands from the terminal:
 chmod a+rx prime17
 chmod a+rx cpu25_tmp.sh
 ...
 chmod a+rx cpu100.sh

 2. Some libraries may be missing. In case of Ubuntu you may run the following commands from the terminal:
sudo apt update
sudo apt upgrade
sudo apt install libgmp3-dev
sudo apt install libjsoncpp-dev
sudo apt install libcurl4-openssl-dev

Windows Installation
There exists only a 64 bit version compiled via MSYS2-MinGW64 environment. Zip file should contain all necessary libraries.

Source Codes
Source codes are available on Github under the following link: https://github.com/MartinZeman17/prime17.git
You are welcome to contribute!

Reinstallation
You can enforce reconfiguration by running command prime17 -w and updating user or computer information.    


CPU utilisation
You may change utilisation of the CPU even during runtime of the program. 
Just run prime17 with the command line parameter -t or -t_tmp while another instance of prime17 is running.
Set new cpu utilisation in percent (100 - all the virtual CPU cores will be utilised, 0 - no CPU core will be available and the program will end).
The new settings will be saved to a config file and will take effect once the current work in progress is completed.
You may also used prepared cmd 
'po99or sh files for this purpose.

In case you terminate the program unexpectedly (closing, task manager, kill command, restart, power off) the current work in progress would be lost.
After a certain timeout (an hour or so) the task will be reassigned to the first available computer =  no big harm done! 

Progress
Progress of each computer and/or user is being calculated only from the work successfully delivered to a database.

Web
You may check progress of the work at the prime17 website at the address: https://prime17.000webhostapp.com/ 

Contacts
Martin Zeman
email: martin.zeman17@gmail.com
LinkedIn: https://www.linkedin.com/in/martinzeman17/
StackOveflow: https://stackoverflow.com/users/13875546/martin-zeman


Matěj Zeman
email: matej.zeman02@gmail.com
LinkedIn: https://www.linkedin.com/in/matej-zeman-199867205/







