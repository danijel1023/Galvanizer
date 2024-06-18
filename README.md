# Galvanizer

Every time a base window is about to build its child, it checks the plugins if they specified that child and proceeds
with
calling the plugin window factory instead of child factory directly. This even allows for extremely flexible solution
where
the plugin can dynamically remove or add factories without refreshing the objects.

Window objects maintain internally child factories. The tree of factories happen because each object contains its own
children
list.

Synchronising plugins might be a bit problematic... How???

ReValidate window event is an event that works similarly to build event. It starts of at the base of the window and just
checks for matches with plugins and built objects. Objects should not do any kind of custom behaviour with the
ReValidate event
as the event is only meant to be responded by the internal logic.
If it finds a match (object name and factory name), it builds the objects by calling the factory and sends a special
event
that notifies the built object of its clone. The event includes a pointer to the original object. The newly created
object
should initialize the move constructor of the original object to inherent all originals object properties. The new
object is
tasked with deleting the old object

To remove main window creation thread synchronisation problem, the main window factory gets called by the main thread
which in turn starts the worker thread and posts an Init event.

// describe how factory list is being stored

# Object creation and roles:

There are 3 steps before object gets fully initialized

1. Factory based creation (object constructor)
    - minimal initialization. In most cases it should be ignored.
2. Build event
    - For example, used by main window to spawn the event loop.
3. Init event
    - Used to ???
