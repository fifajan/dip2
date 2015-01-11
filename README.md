# dip2
3D Meta-Object-based Modelling System (Qt4 powered)

Hello to you!

BRIEF PROJECT INFO:<br />
This is my university masters degree diploma project **(2010)**.<br />
GUI is currently only in russian (sorry). Everything else (code & comments, etc) is in english.

Please find build & usage info below following pictures.

![Screenshot](https://raw.githubusercontent.com/fifajan/dip2/master/screen.png?raw=true "Screenshot")
![Surfaces examples](https://raw.githubusercontent.com/fifajan/dip2/master/surfaces.png?raw=true "Surfaces examples")

To build the project act as following:

$ cd _your_path_/dip2<br />
$ cd infix<br />
$ make clean ; make all<br />
$ cd ..<br />
$ qmake-qt4<br />
$ make<br />

To run the app:

1. Play with program controls not touching Open/Save dialogs.
2. Add some meta-objects and play with variables and camera (see file info/expr.txt for meta-object examples).
3. Save your work (please specify ".mox" file extension).
4. Load your work or some provided examples (examples catalog), pay special attention to BrAiN-b-L-o-W-i-N-g!!111 "propeller.mox" file,
note that it is pretty complex for this currently not very optimized app and can load during 1-10 minutes, depending of coolness of your CPU.
5. Note that you could try to validate .mox (XML actually) files against provided XML Schema as following:

$ cd _your_path_/dip2<br />
$ xmllint --schema xml/doc.xsd file.mox

6. Do whatever you want with the app.
7. Try to crash the app somehow :).

Than you for helping.

God bless you!
