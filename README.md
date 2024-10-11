# Galvanizer

EventLop manager access

---

Posting events to a specified window needs to be done through a name based target system. It could (should)
be made faster through double pointer database reference. The problem comes up if an event is created with expired
target (aka. deleted obj) such as when an event has been queued up, and before it has been processed,
the object gets deleted. Through double pointer DB, the object can set its double pointer to null so when dispatcher
wants to dispatch the event to the object, it can know if the object is expired and simply drop the event.

---

Deleting objects: If the objet that received close event has children, it needs to wait for children to die. This is
accomplished by simply calling `Deconstruct` function from `Application` in destructor.

Problem:

- This is a recursive - meaning synchronous, call; while most of the event system is made with async in mind.
  This could result in issue.
- How to handle situation if the current or child object has its own event loop? One possibility is to destroy the event
  loop and set the reference to the parent event loop. Luckily, this is only on higher lvl of object inheritance, so
  base object should care on what thread it's currently running.

---

Every time a base window is about to build its child, it checks the plugins if they specified that child and proceeds
with calling the plugin window factory instead of child factory directly. This even allows for extremely flexible
solution where the plugin can dynamically remove or add factories without refreshing the objects.

Window objects maintain internally child factories. The tree of factories happen because each object contains its own
children list.

---

ReValidate window event is an event that works similarly to build event. It starts of at the base of the window and just
checks for matches with plugins and built objects. Objects should not do any kind of custom behaviour with the
ReValidate event as the event is only meant to be responded by the internal logic.
If it finds a match (object name and factory name), it builds the objects by calling the factory and sends a special
event that notifies the built object of its clone. The event includes a pointer to the original object. The newly
created object should initialize the move constructor of the original object to inherent all originals object
properties. The new object is tasked with deleting the old object

---

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
