# iRon - lightweight overlays for iRacing <!-- omit in toc -->

This project provides a few lightweight overlays for iRacing. Included are: a relative display with optional minimap, a dashboard with fuel calculator, a throttle/brake input graph, and a standings display. 

The implementation aims to be small, easy to modify, and free of external dependencies.

# Contents <!-- omit in toc -->

- [Installing & Running](#installing--running)
- [Configuration](#configuration)
- [Overlays](#overlays)
  - ["Relative"](#relative)
  - ["DDU"](#ddu)
  - ["Inputs"](#inputs)
  - ["Standings"](#standings)
  - ["Cover"](#cover)
- [Building from source](#building-from-source)
- [Dependencies](#dependencies)
- [Bug reports and feature requests](#bug-reports-and-feature-requests)
- [Donations](#donations)

## Installing & Running

The app does not require installation. Just copy the executable to a folder of your choice. Make sure the folder is not write protected, as iRon will attempt to save its configuration file in the working directory.

To use, simply run the executable. It doesn't matter whether you do this before or after launching iRacing. A console window will pop up, indicating that iRon is running. Once you're in the car in iRacing, the overlays should show up, and you can configure things to your liking.

---

## Configuration

To place and resize the overlays, press ALT-j. This will enter a mode in which you can move overlays around with the mouse and resize them by dragging their bottom-right corner. Press ALT-j again to go back to normal mode.

Overlays can be switched on and off at runtime using the hotkeys displayed during startup. All hotkeys are configurable.

Certain aspects of the overlays, such as colors, font types, sizes etc. can be customized. To do that, open the file **config.json** that iRon created and experiment by editing the (hopefully mostly self-explanatory) parameters. You can do that while the app is running -- the changes will take effect immediately whenever the file is saved.

_Note that currently, the config file will be created only after the overlays have been "touched" for the first time, usually by dragging or resizing them._


## Overlays

---

### "Relative"

Like the "Relative" box in iRacing, but with additional information such as license, iRating, and laps driven since the last pit stop. You can also highlight your friends by adding their names to a buddy list in the config file.

At the top, there is an optional minimap. It can be set to either relative mode (own car fixed in the center) or absolute mode (start/finish line fixed in the center).

![relative](https://github.com/lespalt/iRon/blob/main/relative.png?raw=true)

### "DDU"

A dashboard that concentrates important pieces of information for which you would otherwise have to flip through various iRacing boxes.

The fuel calculator shows the estimated remaining laps, remaining amount of fuel, estimated fuel used per lap, estimated _additional_ fuel required to finish the race, and the fuel amount that is scheduled to be added on the next pit stop. To compute the estimated fuel consumption, the last 4 laps under green and without pit stops are taken into account, and a 5% safety margin is added. These parameters can be customized.

![ddu](https://github.com/lespalt/iRon/blob/main/ddu.png?raw=true)

### "Inputs"

Shows throttle/brake/steering in a moving graph. I find it useful mainly to practice braking accuracy.

![inputs](https://github.com/lespalt/iRon/blob/main/inputs.png?raw=true)

### "Standings"

Shows the standings of the entire field, including safety rating, iRating, and number of laps since the last pit stop ("pit age"). I usually leave this off by default, but switch it on during cautions.

![standings](https://github.com/lespalt/iRon/blob/main/standings.png?raw=true)

### "Cover"

No screenshot, because all it is is a blank rectangle. Can be useful to cover up distracting in-game dashboards, like the one in the next-gen NASCAR.

---

## Building from source

This app is built with Visual Studio 2022. The free version should suffice, though I haven't tried it. The project/solution files should work out of the box. Depending on your system, you may need to separately install prerequisites to build DirectX applications.

---

## Dependencies

There are no external dependencies other than standard Windows components (DirectX) that should already be present on most systems.

Build dependencies (most notably the iRacing SDK and picojson) are kept to a minimum and are included in the repository.

---

## Bug reports and feature requests

If you encounter a problem, please file a github issue and I'll do my best to address it. Pull requests are welcome too, of course.

If you'd like to see a specific feature added, feel free to file a github issue as well. If it's something small, I may actually get to it :-)

---

## Donations

If you like this project enough to wonder whether you can contribute financially: thank you! I'm not looking for donations, but **please consider giving to Ukraine-related charities instead**.
