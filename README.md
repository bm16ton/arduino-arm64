This version of arduino was the happy medium to make all the board packages play nice and work, Its been patched so that menusthat go longer then screen can now be scrolled. Boards installed via json web addresses shoul be updateable via normal methods,the rest IE stm32, appollo etc dont have the gcc toolchain for arm64 listed so these files needed to be copied over (usually into hardware folder) and edits made to point to the toolchain, Also any tools listd in the boards tools folder that are binaries have been recompiled for arm64 (sometimes still residing in linux or linux64 etc folders just cuz im lazy), I believe ive added various new upload methods to variousboards (IE st-link using st-flash not whatever closed source nonsense upload app ST wants you to use) I think I added upload via usb mass storage to sum boards etc.

DOWNSIDE; I rewrote the "theme" to be all black and white wih as little bold etc as I needed. I should change this back someday, should be easy enuf to simply cp the original files over.

Check pin definitions especially for the STM32 stuff they are fucking crazy,I dont know whats happening at ST but I want a job their, older stm32 installs had pins defines this one doesnt, IE spi pins , ethernet port etc on nucleos etc.
Also stm32 install Ive found totaly twacked clock settings for a board or two so that usb couldnt work and the like so always check the variant files.


