Web page
- https://prime17.000webhostapp.com/

Development environment:
- Ubuntu 20 running from Windows WSL (version 1)
- VSCode with remote plugin to WSL (configuration files for VSCode are in .VSCode folder in Src)
- g++ from GCC (supporting C++17 standard) with some pure C code
- cross compiling in MSYS2 MinGW64 

Compiler
- How to add newer g++ and set it as a default compiler.
- First check the current default version via g++ -v
- sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 10
- sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 50
- sudo update-alternatives --config gcc

- sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 10
- sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 50
- sudo update-alternatives --config g++ 

Building, Compiling
- just cd to Src dir and run make (no config, no install in make)
- Linking: -lgmp -lstdc++fs -lpthread -lcurl -ljsoncpp
- In case custom GMP is needed: -L/gmp_install/lib 

Linux packages for development
- sudo apt-get install libjsoncpp-dev libcurl4-openssl-dev libgmp3-dev libncurses5-dev libncursesw5-dev
- and other basic packages: git, latest gcc and g++, make, gdb, 
- debmake

Fresh install of MSYS2 (64 bit)
- https://www.msys2.org/
- https://stackoverflow.com/questions/45836650/how-do-i-integrate-msys2-shell-into-visual-studio-code-on-window
- pacman -Syu
- pacman -Su
- pacman -S --needed base-devel mingw-w64-x86_64-toolchain
- pacman -S jsoncpp-devel

Certificates 
- libcurl on Windows typically cannot find certificates for ssl communication so local certificates should be included in bin folder
- install certificates to MSYS using pacman and select one version (hopefully the latest one should be in C:\msys64\mingw64\ssl)  
- newer version of libcurl have a better windows cert support
- pacman -S ca-certificates
- pacman -S mingw-w64-x86_64-ca-certificates
- https://curl.se/docs/sslcerts.html    -this page contains link to latest and also historic ca-bundle.trust.crt

Differences between MSYS2 and Linux
- conditional compiling is based on #ifdef __MINGW64__ and #ifdef __unix__
- default conversion of objects from std::filesystem to std::string fails under MSYS2 and explicit conversion via .string() must be supplemented

Adding execute right under Linux:
- chmod a+rx my-script.sh

Counting number of lines:
- cat */*.c */*.cpp */*.hpp */*.sql */*php */*.txt | wc -l

Links:
- Readme in GoogleDocs https://docs.google.com/document/d/1zrb4HBXrz3D-NUTpIV1X1VHL79Y44St39ds0p36apJ4/edit


**Git Help**

Clone
- git config --global user.email "Martin.Zeman17@gmail.com"
- git config --global user.name "MartinZeman17" 
- git clone https://github.com/MartinZeman17/prime17.git 
- SSH: git@github.com:MartinZeman17/prime17.git

Standard workflow
http://udacity.github.io/git-styleguide/
feat: A new feature
fix: A bug fix
docs: Changes to documentation
style: Formatting, missing semi colons, etc; no code change
refactor: Refactoring production code
test: Adding tests, refactoring test; no production code change
chore: Updating build tasks, package manager configs, etc; no production code change

git checkout -b "taskX"
git add .
git commit - m "feat: new feature" -m "optional long description"
git checkout main
git merge taskX
smazání lokální větve
git branch -d taskX
git push

git check-ignore
git check-ignore -v example.o Readme.md
Plus the rule why the file is being ingored

git status --ignored
git status --ignored --untracked-files=all
including all subdirs

deletion of the remote branch taskX
git push origin --delete taskX

overwrite local changes with the last commit
git reset HEAD --hard

push the local branch to remote, -u flag creates a branch
git push -u origin branchname


git tag v1.0
git tag -a v1.4 -m "my version 1.4"
git push main --tags


Changelog
git log --pretty="- %cs : %s" > CHANGELOG.md

Git SSH
https://docs.github.com/en/github/authenticating-to-github/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent#
Copy SSH private and public keys to ~/.ssh folder
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519


**Packaging**

Packaging - Debian:
- packaging is not working under WLS unless fakeroot is tweaked a bit: sudo update-alternatives --set fakeroot /usr/bin/fakeroot-tcp
- sudo apt install devscripts build-essential lintian debhelper debmake
- update changelog, manpage,...
- building a package - run from prime17 folder: debuild -us -uc   OR debuild -us -uc   &>log.txt
- installing: sudo apt install ./prime17_1.0_amd64.deb
- uninstalling: sudo apt remove prime17
- 
- 'hithere' example for debianization of some upstream: https://wiki.debian.org/Packaging/Intro?action=show&redirect=IntroDebianPackaging
- simple example https://www.debian.org/doc/manuals/debmake-doc/ch04.en.html
- more examples  https://www.debian.org/doc/manuals/debmake-doc/ch08.en.html
- https://wiki.debian.org/HowToPackageForDebian
- https://wiki.debian.org/Packaging/Intro


**Azure:**
tmux
tmux detach or you can use the shortcut (Ctrl+b then d)
tmux attach

**C++**
- http://thbecker.net/articles/rvalue_references/section_01.html
- https://isocpp.org/blog/2012/11/universal-references-in-c11-scott-meyers
