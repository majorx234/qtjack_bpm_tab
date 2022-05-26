# qtjack_bpm_tab
- Gui for tabing BPM and sending periodical MIDI messages
- WIP!
 - tab button is working
  - midi messages are sent, timing is accurate
 - planned: audio input is visualized
 - planned: audio input with beat detection
 - planned: dial to change phase of sended midi messages

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
- build project:
```
git clone https://github.com/majorx234/qtjack_bpm_tab.git
cd qtjack_bpm_tab
mkdir build
cd build
cmake ..
make
```
- run:
```
./bpm_tab_main
```
## Reference 
- uses parts of analyzerplugin concept of Mixxx: (https://github.com/mixxxdj/mixxx.git)
- uses parts of qm-vamp-plugins: (queenmarybeatdetection & TempoTrackV2) (https://github.com/c4dm/qm-vamp-plugins)
- uses Kissfft: (https://github.com/mborgerding/kissfft)
- uses Idea for visualization of wave by Matthias Nagorni (https://www.heise.de/hintergrund/Parallelprogrammierung-mit-C-und-Qt-Teil-3-Plattformunabhaengige-Audioprogrammierung-1576739.html)
  

## History
- 2022-05-26 Add Super Circular Buffer structure to have circular buffer with continously data
- 2022-05-23 Modify QM bpm detection for use in bpm_tab widget
- 2022-05-21 Add Visualization fo Jack Input
- 2022-05-20 Now timing behavior is accurate
- 2022-05-19 switch to own repo
- 2022-04-14 first version sending periodic Midi messsages

## ToDo
- beat detection on audio data
- beat triggering of tab button
- Add possibility to send midi beat clock
