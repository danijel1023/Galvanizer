# Galvanizer

---

ReValidate window event is an event that works similarly to build event. It starts of at the base of the window and just
checks for matches with plugins and built objects. Objects should not do any kind of custom behaviour with the
ReValidate event as the event is only meant to be responded by the internal logic.
If it finds a match (object name and factory name), it builds the objects by calling the factory and sends a special
event that notifies the built object of its clone. The event includes a pointer to the original object. The newly
created object should initialize the move constructor of the original object to inherent all originals object
properties. The new object is tasked with deleting the old object
