# qtjack_bpm_tab
- Gui for tabing BPM and sending periodical MIDI messages
- WIP!
 - tab button is working
  - midimessages are sent, but timing is not accurate
 - planned is audio input taht is visualized
 - planned audioinput with beat detection
 - planned dial to change phase of sended midi messages

## Info
Dependencies:

* Qt 5.4 or greater (https://github.com/majorx234/qtjack)
* QJack, which itself depends on Jack2

On Ubuntu, you need to install dependencies:
```
sudo apt-get install libjack-jackd2-dev
```

On Arch Linux:
```
sudo pacman -S jack2 qt5-base
```

If you have any questions feel free to drop me a mail me at
```
majorx234@googlemail.com

```

## Usage
- build project: ```
git clone https://github.com/majorx234/qtjack_bpm_tab.git
cd qtjack_bpm_tab
mkdir build
cd build
cmake ..
make
```
- run ```
./bpm_tab_main
```

## History
- 2022-05-19 switch to own repo
- 2022-04-14 first version sending periodic Midi messsages
