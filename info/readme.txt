Hello to you!

BRIEF PROJECT INFO:
This is my university masters degree diploma project (2010).
GUI is currentlu only in russian (sorry). Everytjing else (comments etc) is in english.

To build the project act as following:

$ cd <path_in_your_fs>/dip2
$ cd infix
$ make clean ; make all
$ cd ..
$ qmake-qt4
$ make

To run the app:

$ ./dip2

Hope this unfinished GUI will be intuitively clear.

To test app following my plan (you probably will not do so :) ) please act as following:

1. Play with program controls not touching Open/Save dialogs.
2. Add some meta-objects and play with variables and camera (see file info/expr.txt for meta-object examples).
3. Save your work (please specify ".mox" file extension).
4. Load your work or some provided examples (examples catalog), pay special attention to BrAiN-b-L-o-W-i-N-g!!111 "propeller.mox" file,
note that it is pretty complex for this currently not very optimized app and can load during 1-10 minutes, depending of coolness of your CPU.
5. Note that you could try to validate .mox (XML actually) files against provided XML Schema as following:

$ cd <path_in_your_fs>/dip2
$ xmllint --schema xml/doc.xsd file.mox

6. Do whatever you want with the app.
7. Try to crash the app somehow :).

Thank you for helping.

God bless you!

