# TestEngine3_2018_2019
Test Engine 3

This project has the main goal of being the 3rd experimentation ground for which to test more programming concepts I learned since the last project. In order to help push the experimentation further I also have a sub-goal of creating a 2d fighting game of sorts. This project is again being built from scratch but unlike my preivous projects this one is not currently utilizing SDL or any other low level OS handling frameworks. Instead I've being making calls to the Windows API dircectly to generate an opengl window and gather user input. While I plan to primarly develop and test this game on Windows I've still architected things in a mutli-platform way with a single file holding all of the platform specific code. The idea is to keep all operating system related stuff confined to one file to make things really simple to port to other platforms. I can just swap out the implementation file for the windows operating system and stick in one that I create for linux, leaving all the higher level game/engine code untouched.

This project also includes features such as live coding editing through dll reloading, input recording and playback, and extremely fast compile times by compiling everything into two files (one which acts as the dll with all the actual game specific code and the other, platform specific layer being the exe).

This projects marks a big change for me in terms of how I think and develop programs. I have shyed away from more object-oriented ways of thinking and adopted a more natural programming approach to solving problems (that works for me anyway). With this new approach I don't try and pre-plan much of the game architecture before programming. I instead adopt a more bottom-up approach, letting the code sort of guide my initial architectural decisions. I find this way helps to avoid a lot more rewrites when compared to pre-planning your code structure. This also means I don't tend to develop many class heiarchies from the get-go, typically being the case with the oop mindset, which eventually led to overabstractions and unnecessary complexity.

This change is big for me because I believe the most important asepct of programming is not necessarily the need to learn all the differrent algorithms or data structures (though still important) but the need to write code quickly and efficiently by being comfortable tackling new problems which come about almost every day. This is an ongoing project and I'm still learning and experimenting and hope to gain much more insight into the programming process that works the best for me.
