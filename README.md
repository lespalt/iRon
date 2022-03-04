# iRon - lightweight overlays for iRacing <!-- omit in toc -->

This project provides a few lightweight overlays for iRacing. Included are: a relative display with optional minimap, a dashboard with fuel calculator, a throttle/brake input graph, and a standings display.

I created iRon for my own personal use. As such, its feature set is limited to what I considered sensible in practice given the way I use iRacing. I don't currently plan to extend it further. That said, I'm making it available in the hope it might be useful to others in the iRacing community, either for direct use or as a starting point for other homebrew overlays.

The project's code base aims to be small, easy to modify, and free of external dependencies.

# Contents <!-- omit in toc -->

- [Where to Download](#where-to-download)
- [Overlays](#overlays)
  - [*Relative*](#relative)
  - [*DDU*](#ddu)
  - [*Inputs*](#inputs)
  - [*Standings*](#standings)
  - [*Cover*](#cover)
- [Installing & Running](#installing--running)
- [Configuration](#configuration)
- [Building from source](#building-from-source)
- [Dependencies](#dependencies)
- [Bug reports and feature requests](#bug-reports-and-feature-requests)
- [Donations](#donations)

---

## Where to Download

The latest binary release can be found [here](https://github.com/lespalt/iRon/releases/latest).

## Overlays

### *Relative*

Like the *Relative* box in iRacing, but with additional information such as license, iRating, and laps driven since the last pit stop. You can also highlight your friends by adding their names to a buddy list.

At the top is an optional minimap. It can be set to either relative mode (own car fixed in the center) or absolute mode (start/finish line fixed in the center).

![relative](https://github.com/lespalt/iRon/blob/main/relative.png?raw=true)

### *DDU*

A dashboard that concentrates important pieces of information for which you would otherwise have to flip through various boxes in iRacing.

The fuel calculator shows the estimated remaining laps, remaining amount of fuel, estimated fuel used per lap, estimated _additional_ fuel required to finish the race, and the fuel amount that is scheduled to be added on the next pit stop. To compute the estimated fuel consumption, the last 4 laps under green and without pit stops are taken into account, and a 5% safety margin is added. These parameters can be customized.

![ddu](https://github.com/lespalt/iRon/blob/main/ddu.png?raw=true)

### *Inputs*

Shows throttle/brake/steering in a moving graph. I find it useful to practice consistent braking.

![inputs](https://github.com/lespalt/iRon/blob/main/inputs.png?raw=true)

### *Standings*

Shows the standings of the entire field, including safety rating, iRating, and number of laps since the last pit stop ("pit age"). I usually leave this off by default and switch it on during cautions. Or glimpse at it pre-race to get a sense of the competition level.

Like the "Relative" overlay, this will highlight buddies in green (Dale Jr. in the example below).

![standings](https://github.com/lespalt/iRon/blob/main/standings.png?raw=true)

### *Cover*

No screenshot for this one, because all it is is a blank rectangle. Can be useful to cover up distracting in-game dashboards, like the one in the next-gen NASCAR.

---

## Installing & Running

The app does not require installation. Just copy the executable to a folder of your choice. Make sure the folder is not write protected, as iRon will attempt to save its configuration file in the working directory.

To use it, simply run the executable. It doesn't matter whether you do this before or after launching iRacing. A console window will pop up, indicating that iRon is running. Once you're in the car in iRacing, the overlays should show up, and you can configure things to your liking. I recommend running iRacing in borderless window mode. Overlays *might* work in other modes as well, but I haven't tested it.

---

## Configuration

To place and resize the overlays, press ALT-j. This will enter a mode in which you can move overlays around with the mouse and resize them by dragging their bottom-right corner. Press ALT-j again to go back to normal mode.

Overlays can be switched on and off at runtime using the hotkeys displayed during startup. All hotkeys are configurable.

Certain aspects of the overlays, such as colors, font types, sizes etc. can be customized. To do that, open the file **config.json** that iRon created and experiment by editing the (hopefully mostly self-explanatory) parameters. You can do that while the app is running -- the changes will take effect immediately whenever the file is saved.

_Note that currently, the config file will be created only after the overlays have been "touched" for the first time, usually by dragging or resizing them._

---

## Building from source

This app is built with Visual Studio 2022. The free version should suffice, though I haven't verified it. The project/solution files should work out of the box. Depending on your Visual Studio setup, you may need to install additional prerequisites (static libs) needed to build DirectX applications.

---

## Dependencies

There are no runtime dependencies other than standard Windows components like DirectX.  Those should already be present on most if not all systems that can run iRacing.

Build dependencies (most notably the iRacing SDK and picojson) are kept to a minimum and are included in the repository.

---

## Bug reports and feature requests

If you encounter a problem, please file a github issue and I'll do my best to address it. Pull requests with fixes are welcome too, of course.

If you'd like to see a specific feature added, feel free to file a github issue as well. If it's something small, I may actually get to it :-) No promises though, as unfortunately the time I can spend on this project is quite limited.

---

## Donations

If you like this project enough to wonder whether you can contribute financially: first of all, thank you! I'm not looking for donations, but **please consider giving to Ukraine-related charities instead**.
