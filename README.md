# Galvanizer

---

Build information is found on the bottom of the readme file.

### Included Application

The included test application demonstrates the displaying of a texture,
response to mouse, window and keyboard events. Hovering over any of the 3 windows (main window (1), the gray window
containing the yellow one (2) and the yellow window(3)) will outline them with an orange outline. Clicking and dragging
the (2) window will drag it to the mouse pointer location. The application will correctly resize all of its elements
based on the monitors' scaling factor and will react on changes (at least on Linux with Wayland - not tested on
Windows). Clicking on the (0) window and typing on keyboard will print to the console the inputted key presses together
with the OS provided codepoints.

The code of the test application is not well written as it currently serves me like a test platform so it
includes traces of other former code (eg. unused variables, etc).

### Galvanizer Information

Galvanizer is a hardware accelerated GUI ~~framework~~(library) that was designed with multithreading and plugin support
out-of-box. I officially only support Linux however, as of time of writing, there are 0 linux specific code paths so it
should work equally well on other platforms.

The multithreading aspect of it is the ability to run each `GalvanizerObject` on its own thread easily. You
achieve that by just creating an `EventLoop` instance and pointing the object's `EventLoopRef` to it.

Plugin support is enabled by (blueprint) factory design pattern. Each object __must__ have an associated named factory
which `ObjectFactories::Build` can call to construct the said object. This is also implemented such that the future plan
of adding "on the fly" loading of plugins is possible where the application could decide to load or unload a plugin
without needing a restart.

Galvanizer is ment to be very small in terms of predefined gui elements, however it supplies the tools necessary to
build such elements and interact with them. As of writing, it doesn't support loading of custom shaders and using them
in place of the default ones but, it is on the "nice to have" list. A really good feature of Galvanizer is the fact that
adding support for rendering 3D objects (or anything besides what the default shaders allow) is relatively easy using
the builtin OpenGL abstraction layer.

Event system follows some ideas of Win32. It is dispatcher-callback based where the event gets received by the dispatch
function which in return calls the callback function(s) based on the events properties (`EventVisibility` and
`ChildPriority`). An example of an event is window scale (monitor scaling); `EventVisibility` is set to `Root` and
`ChildPriority` is set to `Last`. This makes the event get propagated to all the children of the received object
however, the parent is the first one that receives the event via callback. Afterward, it sends the event to the
children's dispatchers.

Events can be sent by the event loop or by calling directly the dispatcher function of the targeted object.

Events themselves are also dynamic where the application/plugin can define their own on the fly and the system will
compensate for it. `EventManager` stores the type and the string associated with the event type
(eg. `EventManager::get().RegisterEvent<KeyEvent>("KeyEvent")`) which allows for
easier creation of debugging tools for event inspection.

Important for this design of multithreaded event system is the guarantee that if the object is created, it __will__
receive at least `Init`, `Close` and `Terminate` (plus the object constructor and destructor). Constructor and
destructor are executed on parents event loop thread, while if the child obj has their own thread/event loop, `Init` and
`Close` events will be sent from that event loop. This is needed if the object needs to interact for example with a
library that is very sensitive from which thread it's being called.

### Building

To build the application and the library, you need to have `CMake` and `Python` installed on the system. Beyond that,
building should be the same as with any other C++ project.

Note for Windows users using VS: you can install `C++ CMake tools for Windows` (if they are not installed already) in
the `Visual Studio Installer`. This allows you to just open the root folder containing the `CMakeLists.txt` and build
the project from the IDE.

As mentioned above, I only officially support Linux builds, so fixing Windows-specific issues is not being prioritized.
