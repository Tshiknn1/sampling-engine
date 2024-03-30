# sampling-engine

This sampling engine, as of yet unnamed, is designed to be as system-agnostic as possible. In this case, since we are interacting with Qt, all audio gets routed through QEndpoint. This is the only object that Qt needs to touch, and the class itself only contains the methods Qt needs to push audio in and get buffered audio out.

Internally, the QEndpoint only touches a MasterBus object, which itself determines its behavior by reading a script, provided by the user, which creates various objects and behaviors and links them to the MasterBus.

The MasterBus contains a queue of buffered audio, as well as vectors that allow it to interact with all of its subsidiary audio, timing, and value generators.

Generators are subclassed from a few different abstract classes, like AudioGen, TimingGen, ValueGen, State, etc.

## Concerns

Obviously, in a complicated application, this has the potential to become very complex and introduce speed issues, memory management issues, etc. Careful attention will need to be paid to ensure that there are no memory leaks and audio will continue to be buffered. Adding more complexity is the necessity for this system to respond to real-time input changes.

Also, though, there is the consideration of future flexibility. I am attempting to design this with a high degree of modularity, which is why even the very basic demo version looks pretty different from other similar projects. Part of it is that I want the functions that generate audio to be abstracted far away from any library pollutants (i.e., I don't care for my audio-generating objects to subclass QIODevice and the like because then my stuff is going to be filled with Qt stuff). If I ever want to move to a different platform (& I probably will), I don't want to have to spend a bunch of time ripping library functions out of it. In addition, I will inevitably want to add new ValueGen types in the future, so it's most productive to build a platform with a high degree of flexibility.

## Outstanding questions

Outstanding questions largely concern things like timing and data flow. i.e., it would be nice if each AudioGen object could maintain a private buffer that all gets summed together at some later point; theoretically, certain objects (those that don't need to accept real-time state information) could buffer indefinitely, while others are constantly waiting for parameter updates. I would assume that my program will run in a separate thread from Qt, but will the program itself deploy multiple threads, or is it all going to run in a single thread? If it can be made performant enough without additional threading, that would be great. The MasterBus object, at least, will probably need to control asynchronous access to its buffer threads; ValueGen objects that take user inputs will also need to manage asynchronous access to the user inputs they're tied to. Must I learn to use std::atomic?

Other, smaller questions:
- How to implement stereo audio? Look to other projects for inspiration