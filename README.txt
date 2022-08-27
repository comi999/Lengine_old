##############################################################################
# What is Lengine?
##############################################################################
Lengine is a project used to iterate and learn game engine development,
including parts like incorporating an ECS, including graphics and sound, and
other parts. The main feature of the Lengine is that it also supports console
graphics as a viable graphics backend, just for novelty. There's a lot that 
still needs to be done, like including physics, expanding the list of graphics
features and backends supported, and adding more game engine specific features
like scenes etc.

##############################################################################
# How do I set it up?
##############################################################################
1. First, clone this repo into a folder.
2. Run: "git submodule init" in the checked out directory. This will
   initialise the submodules.
3. Run: "git submodule update" in the checked out directory. This will
   download the submodules.
4. Create a folder in the parent directory of the checked out folder. Should
   be something like {Lengine directory}/../Lengine_Built
5. In this new "Lengine_Built" directory, run: "cmake ../Lengine" (you will of
   course need cmake installed to do this, version 3.10 or above)
6. Navigate to {Lengine_Built}/Resources and run the ResourcePackager.exe
7. Open the Lengine.sln in Lengine_Built
8. Feel free to check out the Lengine-Example solution. It gives some basic
   example on how things work. It by no means shows all features, since
   Lengine is constantly changing and being worked on, however,
   Lengine-Example should always be counted on to work or show an example of
   the most up to date working version of Lengine.