# Body re-Switcher
One day a bunch of kids visits Crazy Professor's lab and
"accidentally" switch their bodies. Now **you** are the one who
have to clean that mess! But there is a catch: once two bodies
have been switched they can't be switched back. Can you find the solution to this puzzle?

## Releases
Use below links to download releases you want:
- [MS Windows ZIP file](https://github.com/felix-leg/bodyreswitcher/releases/download/v1.0.1/bodyreswitcher_mswin_build_1.0.1.zip) - unzip to any folder and start `bodyreswitcher.exe`
- [Debian amd64 .deb package](https://github.com/felix-leg/bodyreswitcher/releases/download/v1.0.1/bodyreswitcher_1.0.1_amd64.deb) - use `dpkg -i` to install this package on you Debian (or any Debian-like) system
- [Autoconf sources](https://github.com/felix-leg/bodyreswitcher/releases/download/v1.0.1/body-re-switcher-1.0.1.tar.gz) - untar the archive and run standard `./configure` → `make` → `make install` commands

## How to play
Once the game starts, choose the difficultly from the "New game" menu.

Now you are presented with a graph showing kids initials and possible connections
representing body switches you can make. Each kid is drawn as a double circle. 
In the bigger circle is written the current owner of the body, while the smaller circle
shows the rightful owner of that body. As you can guess, your job is to make both of those
circles show the same letter (which means the right person is in the right body).

To do a switch select first body by clicking on the bigger circle. Now click on another body
in the same way. If the switch is possible (the circles are connected with a blue line),
you will see the switch animation. 

Except switching bodies, you can also do two other things:
- **Call for reinforcements** adds 2 extra people to the graph in their own bodies.
- **Undo** gives you the ability to undo last *n* switches you have made. Unfortunately there is 
a limit of how many times you can do this.
